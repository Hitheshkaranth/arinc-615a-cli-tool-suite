# Code trace — `main()` to the wire, function by function

A function-by-function walkthrough of the command-line data loader: every
function on the path from `main()` down to a byte on the network, and back up
through the handler callbacks that print to the console.

Each function block carries its source location as `file:line`, relative to this
repository. Line numbers refer to the tree as it stands here.

> **Scope.** This describes the **host** side — the ground data loader that
> initiates. Where the target's behaviour matters to the host state machine it is
> described as an observed stimulus, not as an implementation.

**Read it in this order.** Sections 04–07 are a linear walkthrough of startup and
dispatch. Sections 08–13 are one per protocol operation and independent of one
another, except that §09 is shared machinery §10–§13 all build on — read §09
first. Sections 14–17 are the layers underneath and work as reference.

Companion documents: [ARCHITECTURE.md](ARCHITECTURE.md) for the layer map and
directory layout, [BUILD.md](BUILD.md) for building and dependencies.

---


---

## 01 — Scope

### What this document covers

A function-by-function trace of the command-line data loader in this repository: every function on the path from `main()` to a byte on the wire, and back up through the handler callbacks that print to the console.

This repository is the command-line distribution of the tool suite: `app/CMakeLists.txt` wires in a single application. The table below reflects that state — see §20 for what it excludes and how to re-enable a target.

| Target | Role | Covered |
| --- | --- | --- |
| arinc_615a_operation | **Built.** Host data loader — nine sub-commands, all protocol operations | §4–§17 |
| arinc_615a_download_request_file | On disk, not wired into the build. Offline generator for the `.LNR` request file | §18 |
| arinc_615a_test_tha | On disk, not wired into the build. Simulated target hardware | Referenced only |
| arinc_615a_unit_test | On disk, not wired into the build. Boost.Test suite | Out of scope |

#### How to read it

Sections 4 through 7 are a linear walkthrough of process startup and command dispatch — read them in order. Sections 8 through 13 are one per protocol operation and are independent of each other; §9 is shared machinery that §10–§13 all build on, so read §9 before any of them. Sections 14 through 17 are the layers underneath, and can be consulted as reference.

Every function block carries its source location as `file:line`. Line numbers refer to the tree as it stands in this working copy. Collapsed blocks expand on click.

**The target side is not the subject here.** Everything below describes the *host* data loader — the machine that initiates. Where the target's behaviour matters to the host's state machine, it is described as an observed stimulus, not as an implementation.

---

## 02 — Architecture

### Six layers, one direction

Control flows downward through six layers on the way out and returns through registered handler callbacks on the way in. No layer calls back into the layer above it directly; the upward path is always a `std::function` or an abstract handler interface bound during setup. That inversion is what lets the same host protocol library serve any front end built on top of it. Calls descend the stack; results climb it only through handlers bound at setup time. The command layer implements the host layer's handler interfaces, which is why a command object *is* its own callback sink.

#### The dependency that shapes everything

Five of this project's dependencies are pulled at configure time by `FetchContent` from `git.thomas-vogt.de` — `helper`, `arinc_649`, `arinc_665`, `tftp`, and `commands`. Two of them carry weight in the CLI path and are worth naming:

- `commands` supplies `Commands::CommandRegistry` and `Commands::Utils_commandLineHandler` — the entire sub-command dispatch mechanism. It is not in this repository.
- `tftp` supplies the generic TFTP client and server. The `Arinc615a::Tftp` namespace in this repository is a thin decorator over it, not a reimplementation.

`arinc_665` matters for upload only: it resolves load headers and media sets. `arinc_649` supplies `CheckValue` and the check-value generators used for the checksum TFTP option.

---

## 03 — Concurrency

### Two threads and a latch

The process runs exactly two threads of interest. The main thread parses the command line and then *blocks*; a single `std::jthread` runs the Boost.Asio `io_context` and executes every protocol handler. All protocol state lives on the I/O thread and is never touched concurrently, because the main thread does nothing but wait once the operation starts.

The handshake between them is a `std::latch{1}` named `done`, a member of each command object. The command's `finished()` callback — invoked on the I/O thread — counts it down; the main thread's `done.wait()` returns. Once `start()` hands off, the main thread contributes nothing until the latch releases. Every box on the right runs on the single I/O thread, serialised by the `io_context`. Consequence

Because `done` is a `std::latch` initialised to 1, a second call to the command's `finished()` decrements it below zero, which is undefined behaviour. Nothing in `OperationImpl::finished()` guards against being called twice. §19 documents two concrete paths that reach it twice.

#### Signal handling

SIGINT and SIGTERM are handled asynchronously on the same `io_context` via `boost::asio::signal_set`. The handler is re-armed after each delivery, and uses a function-local `static bool abortIndicator` to escalate: the first signal raises `abortSignal` (graceful protocol abort), any subsequent signal raises `terminateSignal` (immediate teardown). Both are `boost::signals2::signal<void()>`, and each command connects its own operation's `abort`/`terminate` to them with a `scoped_connection` so the wiring dies with the command invocation.

---

## 04 — Entry point

### main(), line by line

The entry point is 74 lines with three nested exception frames. Its job is narrow: set log levels, build a registry, start an I/O thread, hand the argument vector to the dispatcher, then unwind cleanly.

#### `int` main( int argc, char *argv[] ) — `arinc_615a_operation.cpp:76`

Wrapped in a function-try-block whose handler prints `"Very bad exception"` — this catches throws from the initialisation of function-local statics and from the inner handlers themselves.

1. **Log levels.** Nine separate `setLogLevel()` calls, one per library — `spdlog`, `Arinc615aCommands`, `Arinc615a`, `Tftp`, `Arinc665Commands`, `Arinc665`, `Arinc649`, `Commands`, `Helper`. Each library owns a private spdlog level; setting one does not set the others. All start at `warn`.
1. **Banner.** Prints `"ARINC 615A Operation - {version}"` from `Arinc615a::Version::VersionInformation`, a generated header.
1. **Registry.** `Commands::CommandRegistry::instance()` returns a shared pointer to the command table.
1. **I/O primitives.** Constructs `io_context`, a `signal_set` bound to SIGINT and SIGTERM, and the two `AbortTerminateSignal` objects.
1. **Command registration.** `Arinc615aCommands::registerCommands(...)` then `Arinc665Commands::registerCommands(registry)`. The second call adds the ARINC 665 media-set management commands that appear in the same CLI.
1. **Arm signals.** `signals.async_wait(...)` with a `bind_front` onto `signalHandler`, capturing the signal set by reference so the handler can re-arm itself.
1. **Start I/O.** A `std::jthread` running `ioContext.run()`. Note there is no `work_guard` — `run()` stays alive only because the `signal_set` has a pending async operation.
1. **Dispatch.** `Commands::Utils_commandLineHandler( registry )( argc, argv )` — blocking, returns the process exit code.
1. **Teardown.** `signals.cancel()`, `ioContext.stop()`, `ioRunner.join()`, then `return result`.

The inner `try` has three handlers: `boost::exception` and `std::exception` both print via `boost::diagnostic_information()`, and a catch-all prints `"Unknown exception occurred"`. All three return `EXIT_FAILURE`.

#### `static void` signalHandler( signal_set&, abortSignal, terminateSignal, error_code, int ) — `arinc_615a_operation.cpp:151`

Returns immediately if the error code is `operation_aborted` — that is the normal teardown path from `signals.cancel()`. Otherwise it logs which signal arrived, **re-arms itself** with another `async_wait`, and then escalates.

Escalation uses a function-local `static bool abortIndicator{ false }`. First signal: prints `"Abort request from user"`, calls `abortSignal()`, sets the flag. Every subsequent signal: prints `"Terminate request from user"`, calls `terminateSignal()`. Design intent

Abort is *protocol-level*: the host stops initiating and sends an ARINC 615A ABORT error message the next time the target opens a status transfer, letting the target close out cleanly with a final status file. Terminate is *immediate*: it fabricates a final status locally and releases the latch. One Ctrl-C is polite; two is not.

---

## 05 — Registration

### How a name becomes a handler

Registration is a two-level fan-out. `Arinc615aCommands::registerCommands()` delegates to two namespace-level functions, each of which constructs command objects and binds two member functions per command into the registry.

#### `void` Arinc615aCommands::registerCommands( registry, ioContext, abortSignal, terminateSignal ) — `Arinc615aCommands.cpp:23`

Two calls, nothing else: `Targets::registerCommands(...)` for the discovery commands and `Operations::registerCommands(...)` for the protocol operations. All four arguments are forwarded unchanged.

#### `void` Targets::registerCommands( … ) — `targets/Targets.cpp:22`

Constructs `FindQueryCommand` and `ListTargetsCommand` as `shared_ptr`s, then registers each with `registry->command( name, description, executeFn, helpFn )`. The two function objects are `std::bind_front( &T::execute, ptr )` and `std::bind_front( &T::help, ptr )` — the `shared_ptr` is copied into the binder, so the command object outlives this function and lives as long as the registry.

Registered names: `Find`, `Targets`.

#### `void` Operations::registerCommands( … ) — `operations/Operations.cpp:27`

The same pattern, seven times over. Registered names in order: `Information`, `Upload`, `AdhocUpload`, `UploadLoads`, `BatchUpload`, `MedDownload`, `OpDownload`.

Every command object is constructed *eagerly*, at startup, whether or not it will be used. Each constructor builds a full `boost::program_options::options_description`. That is why running any sub-command pays the cost of building all nine option tables.

#### `shared_ptr<option_description>` Arinc615aCommands::targetAddress( address*, bool defaultIsBroadcast ) — `Arinc615aCommands.cpp:34`

Factory for the shared `--target-address,-a` option. Builds a `value_semantic` bound to the caller's address variable, marks it `required()`, and — when `defaultIsBroadcast` is true — defaults it to `address_v4::broadcast()`. Only `FindQueryCommand` passes `true`.

The other commands do *not* use this factory; they declare `target-address` inline and **without** `required()`, because the address may instead be resolved from a targets-list JSON file. See §7.

#### The dispatcher itself

`Commands::Utils_commandLineHandler()` lives in the external `commands` dependency and is not part of this repository. What it does is inferable from the registry API and the documentation: it reads a command-selection argument, looks the name up in the registry, and invokes the bound `execute` with the remaining arguments as a `Commands::Parameters` (a string vector). `--help` routes to the bound `help` instead. Documentation conflict

The repository disagrees with itself on the selection flag. `app/arinc_615a_operation/arinc_615a_operation.adoc` documents `-c|--command=<Name>`; every manpage under `doc/arinc_615a_manpage/` documents `--operation=<Name>`. Only one can be right, and the deciding code is in the fetched `commands` library. Confirm against `--help` on your built binary before scripting around either form — §21 shows how.

---

## 06 — Catalogue

### The nine commands

Seven perform protocol operations against a target; two are local utilities. The *Initiates* column names the protocol file the host reads first, which is what actually selects the operation on the target side.

| Name | Class | Initiates | Purpose |
| --- | --- | --- | --- |
| Find | FindQueryCommand | UDP 1001 | Broadcast discovery of targets on the network |
| Targets | ListTargetsCommand | — | Reads a saved targets JSON and prints it; no network |
| Information | InformationOperationCommand | .LCI | Reads the Load Configuration List — part numbers, serials |
| Upload | UploadOperationCommand | .LUI | Uploads loads from a registered ARINC 665 media set manager |
| AdhocUpload | AdhocUploadOperationCommand | .LUI | Uploads from raw media source directories, no manager needed |
| UploadLoads | UploadLoadsOperationCommand | .LUI | Uploads loads addressed by load-header file path |
| BatchUpload | BatchUploadOperationCommand | .LUI | Drives an ARINC 665 batch file across multiple targets |
| MedDownload | MediaDefinedDownloadOperationCommand | .LND | Download where the host names the files it wants |
| OpDownload | OperatorDefinedDownloadOperationCommand | .LNO | Download where the target advertises a list first |

The `Arinc665Commands::registerCommands()` call in `main()` adds more names to the same binary — `Create`, `ImportMediaSet`, `ImportMediaSetXml`, `ListBatches`, `ListLoads`, `ListMediaSets`, `RemoveMediaSet` — which manage the media set store that `Upload` and `BatchUpload` read from. They belong to the `arinc_665` dependency and are outside this document.

---

## 07 — Skeleton

### The shape every command shares

All seven protocol commands are built to one template. Learning it once means §10 through §13 only have to describe what differs.

#### Constructor — build the option table

Each constructor stores references to `ioContext`, `abortSignal`, `terminateSignal`, then assembles `optionsDescriptionV` in a fixed order:

1. `--log-level,-l` with a `notifier` lambda that calls `setLogLevel` on every library at once. This is the only way to raise verbosity across all layers from the command line.
1. `optionsDescriptionV.add( configurationV.options() )` — splices in the whole `Arinc615aConfiguration` option group, which itself splices in the TFTP and TFTP-option groups. §19 lists the fields these expose.
1. Target addressing: `--target-address,-a`, `--targets-list,-l`, `--target-id,-i` (required).
1. `--dlp-timeout`, defaulting to `DefaultArinc615aDlpTimeout` = 13 s, with a notifier that converts the integer to `std::chrono::seconds`.
1. `--port-option`, a `bool_switch` enabling the ARINC 615A-3 Port Option.
1. Command-specific options — media set paths, load headers, download directories.
Confirmed defect — affects all nine commands

The short flag `-l` is declared twice in every command: once for `log-level,l` and once for `targets-list,l`. Eighteen sites across nine files, e.g. `InformationOperationCommand.cpp:58` and `:80`. Boost.Program_options cannot disambiguate; `-l` is unusable and the long forms must be spelled out. Fix by renaming the second to a free letter — `targets-list,T` is unclaimed. See §22.

#### execute() — seven steps

1. **Parse.** `command_line_parser( parameters ).options( optionsDescriptionV ).run()` into a `variables_map`, then `notify()` to fire the notifiers and enforce `required()`.
1. **Resolve the address.** If `--targets-list` was given, read the JSON with `read_json`, convert it with `TargetInformation::targetsAddressInformation()`, index it by Target ID via `TargetsAddressInformationMap_fromTargetsAddressInformation()`, and look up `targetIdV`. A hit overwrites `targetAddressV`. A miss leaves whatever `--target-address` supplied.
1. **Guard.** If `targetAddressV.is_unspecified()`, throw `program_options::error{ "Target IP address invalid, not provided, or not in target list." }`.
1. **Build the operation.** `Protocol::instance( ioContextV )`, then the matching factory method with a designated-initialiser configuration struct carrying `dataLoaderConfiguration`, `handler = *this`, `targetAddress`, `targetId`, `dlpTimeout`, `portOption`.
1. **Wire signals.** Two `boost::signals2::scoped_connection` objects bind `abort` and `terminate` with `AbortReason::Operator` pre-bound. They disconnect when `execute()` returns.
1. **Run and block.** `operationV->start()`, then `done.wait()`.
1. **Report.** Print `Tftp::Packets::PacketStatistic::globalReceive()` and `globalTransmit()`.

The catch chain re-throws `program_options::error` so the dispatcher can print usage, and swallows everything else after printing to `stderr`. **Note the consequence:** a failed operation still returns success unless the parse failed — the process exit code does not reflect protocol outcome. If you are scripting these commands, parse the printed final status code instead.

#### The handler interface

Each command privately inherits the matching handler interface — `InformationOperationHandler`, `UploadOperationHandler`, and so on — and passes `*this` as the handler. The overrides are the console output layer. All of them run on the I/O thread.

| Override | When it fires | Present on |
| --- | --- | --- |
| initialisationDeferred(s) | Target answered the init read with a WAIT error | all operations |
| initialisationResponse(r) | Init file decoded; carries accept / deny / unsupported | all operations |
| status(s) | Each status file the target writes to the host | all operations |
| finished(code, desc) | Terminal — counts down the latch | all operations |
| targetInformation(t, ok) | Load Configuration List decoded | Information |
| fileRequest(…) | Target asks for a data file | Upload, both downloads |
| downloadingList(files) | Target advertised its downloadable files | OpDownload |

---

## 08 — FIND

### Discovery: one broadcast, N answers

FIND is the only operation that does not use TFTP. It is a single UDP broadcast on port 1001 followed by a fixed listening window, default three seconds. Every target that answers is printed and optionally appended to a JSON file that the other commands can then use to resolve Target IDs to addresses.

#### Packet format
Both packet forms carry a 16-bit opcode, one or more NUL-terminated strings, and a single `0x10` packet terminator. Constants live at `find/packets/Packet.hpp:72–78`; the parameter order is fixed by the `ParameterList` enum at `find/packets/Packets.hpp:58`.

#### Call sequence

#### `void` FindQueryCommand::execute( const Commands::Parameters& ) — `targets/FindQueryCommand.cpp:84`

Diverges from the §7 skeleton because there is no Target ID and no host TFTP server.

1. Parse options into a `variables_map` and `notify()`.
1. `Find::Clients::Client::instance( ioContextV )` then `client->query()` — a `ClientImpl` that exists only to hand back a `QueryImpl`.
1. Connect *both* abort and terminate signals to the same slot, `Query::abort`. FIND has no graceful mode — the distinction collapses.
1. Configure by fluent chain: `responseHandler`, `completionHandler`, `localAddress`, `remoteAddress`, `port`, `dynamicLocalPort`, `timeout`.
1. `query->start()`, then `done.wait()`.
1. If `--targets-list` was given and at least one target answered, serialise `targetsV` with `write_json`.

#### `void` QueryImpl::start() — `find/clients/implementation/QueryImpl.cpp:83`

Opens the UDP socket, sets `SO_BROADCAST`, and binds to `localAddressV`. The local port is `0` when `--dynamic-port` is set, otherwise the same 1001 as the destination — binding the well-known port locally is what lets a target reply to a fixed source port.

Then, in order: `send( remote, Packet{ Opcode::InformationRequest, 1U } )` — a packet with exactly one empty parameter, which encodes to the four fixed bytes above; `receive()` to arm the first async read; `timerV.expires_after( timeoutV )` and `async_wait` onto `timerHandler`.

A `boost::system::system_error` from any socket call invokes the completion handler and rethrows as `FindClientException` — so a bind failure still releases the latch rather than hanging the CLI.

#### `void` QueryImpl::receiveHandler( const error_code&, size_t bytesTransferred ) — `find/clients/implementation/QueryImpl.cpp:203`

Returns silently on `operation_aborted`. Logs and continues on any other error. On success, forwards the received span to `PacketHandler::packet()`, then **unconditionally re-arms** `receive()`. The listening window is bounded by the timer alone, never by packet count — which is what allows N targets to answer one broadcast.

#### `void` PacketHandler::packet( const udp::endpoint&, ConstRawDataSpan ) — `find/packets/PacketHandler.cpp:23`

The dispatch point. Calls `Packet::packetType()` to peek the opcode without full decoding, then switches: `InformationRequest` → `informationRequestPacket()`, `InformationAnswer` → `informationAnswerPacket()`, anything else → `invalidPacket()`. Each branch constructs a full `Packet` inside a `try`; an `InvalidFindPacket` throw is caught, counted as invalid in the statistics, and rerouted to `invalidPacket()`.

Every branch updates `PacketStatistic::globalReceive()` — the counters the CLI does *not* print for FIND, though it does for TFTP.

#### `Packet::Packet`( Helper::ConstRawDataSpan rawPacket ) — `find/packets/Packet.cpp:67`

The decoding constructor. Four validation gates, each throwing `InvalidFindPacket` with a distinct message:

- Size must exceed 2 bytes — `"Packet to small"`.
- Last byte must equal `PacketTerminator` (0x10) — `"Packet is not terminated by packet terminator"`. The terminator is then trimmed off.
- Opcode must be 1 or 2 — `"Invalid opcode"`.
- The remaining bytes are split on `'\0'`; a segment with no terminator throws `"String Terminator missing"`. At least one parameter must result — `"Packet must contain at least one (empty) parameter"`.

#### `void` QueryImpl::informationAnswerPacket( const udp::endpoint& remote, const Packet& answer ) — `find/clients/implementation/QueryImpl.cpp:233`

Rejects any answer whose parameter count is not exactly `ParameterList::Last` (5), logging `"invalid number of parameters"` and returning — a five-field contract with no tolerance for extension. Otherwise builds a `TargetInformation` by indexing the parameters in enum order and invokes `responseHandlerV`.

#### `void` FindQueryCommand::response( const address&, const TargetInformation& ) — `targets/FindQueryCommand.cpp:150`

Prints the six fields — THW ID, type name, position, literal name, manufacturer code, and the derived Target ID — then appends the pair to `targetsV`. There is no de-duplication: a target that answers twice appears twice in the printout and twice in the saved JSON.

#### `void` QueryImpl::timerHandler( const error_code& ) — `find/clients/implementation/QueryImpl.cpp:168`

The normal exit. Returns on `operation_aborted`. Otherwise cancels and closes the socket, then calls `completionHandlerV()` → `FindQueryCommand::finishedFind()` → `done.count_down()`. Latent double-release

`timerHandler()` and `abort()` both call `completionHandlerV()` unconditionally, and neither sets a guard flag. The common orderings are safe because each cancels the other's pending operation, but a Ctrl-C that lands after the timer has already fired reaches `abort()` on a latch that is already at zero — `count_down()` below zero is undefined behaviour. A `bool completedV` checked in both is the minimal fix.

#### Target ID construction

`TargetInformation::targetId()` at `find/TargetInformation.cpp:150` returns `TargetId{ thwId, thwPosition }` — the ID used for every protocol filename in every other operation. Its validation rules, from `TargetId.cpp` and `TargetId.hpp:39–48`, are worth knowing because they constrain what your hardware may report:

| Field | Length | Character set | Source |
| --- | --- | --- | --- |
| THW ID | 4 … 15 | alphanumeric only, C locale | TargetId.hpp:39,42 |
| Position | 0 … 8 | alphanumeric only, C locale | TargetId.hpp:45,48 |
| Separator | 1 | first `_` in the string splits the two | TargetId.cpp splitTargetId |

So `ABCDEF_123` is a valid Target ID and yields protocol filenames like `ABCDEF_123.LCI`. A THW ID with a hyphen, a space, or fewer than four characters fails `isCompliant()` and every protocol filename built from it will be rejected by the host's own `checkRequest()`. §19 covers what to do when your hardware does not follow this.

---

## 09 — Operation base

### OperationImpl: the shared state machine

Every TFTP-based operation derives from `Arinc615a::Host::OperationImpl`. It owns the TFTP client, the TFTP server, the DLP timeout timer, the abort state, and the protocol file logger — and it implements the initialisation handshake that all four operations begin with. The derived classes supply only `start()`, `tftpRequest()`, and their own file handling.

The asymmetry to hold onto: the host is a TFTP **client** when it reads the initialisation file and writes request files, and a TFTP **server** for everything the target pushes back — status files, list files, and data files. Both run simultaneously. Initialisation is a gate: the operation body is only entered on `OperationAccepted`. Once inside, the DLP watchdog is re-armed by every status file, and its expiry is the only unconditional exit.

#### `OperationImpl::OperationImpl`( ioContext, configuration, handler, targetAddress, targetId, dlpTimeout, portOption ) — `host/implementation/OperationImpl.cpp:50`

Stores all six parameters as `const` members, constructs a TFTP client and server from the shared `io_context`, and constructs the timer. The one active step is configuring the server:

```cpp
( *tftpServerV )
.serverAddress( { configurationV.localInterfaceAddress,
portOptionV ? 0U : configurationV.tftpConfiguration.tftpServerPort } )
.requestHandler( std::bind_front( &OperationImpl::receivedTftpRequest, this ) );
```

**Port 0 when the Port Option is enabled** — the OS picks an ephemeral port, whose actual value is read back later via `tftpServerV->localEndpoint().port()` and advertised to the target inside the TFTP option set. That is the whole mechanism of the ARINC 615A-3 Port Option.

#### `void` OperationImpl::initialise( Files::ProtocolFileType fileType ) — `host/implementation/OperationImpl.cpp:249`

Starts the operation proper. In order: `tftpServerV->start()` so the host is listening *before* it asks for anything; allocate a `MemoryFile` as the sink; create a TFTP read operation; configure it by fluent chain; call `request()`; bump `ProtocolFileStatistic::globalReceive()`.

The configuration chain is where the whole `Arinc615aConfiguration` reaches the wire — `tftpTimeout`, `tftpRetries`, `dally`, `optionsConfiguration`, `dlpRetries`, the three handlers, the filename from `protocolFilename( fileType )`, the port option, and the remote endpoint built from `targetAddress()` and `tftpServerPort`.

Catches `Arinc615aException` and only logs it. Note what that means: an exception here leaves the operation with no pending work and no `finished()` call, so the CLI blocks on the latch until the DLP timer fires.

#### `void` OperationImpl::initialisationFileCompleted( fileType, MemoryFilePtr, TransferStatus ) — `host/implementation/OperationImpl.cpp:376`

The gate in the diagram above. Resets `initialisationOperationV`, then:

1. Any status other than `Successful` → `finished( OperationAbortedByDlp, "Initialisation File could not be received" )`.
1. Log the raw bytes through the protocol file logger, then decode as `Files::InitializationFile`.
1. Invoke `handlerV.initialisationResponse( initFile.response() )` — this is what prints the initialisation code to the console.
1. Switch on the acceptance code:

- `OperationAccepted` → **store `protocolVersionV` from the file**, then `triggerDlpTimeout()`. Every protocol file the host writes afterwards uses this version.
- `OperationDenied` / `OperationNotSupported` → `finished()` with that code and the target's description.
- anything else → `finished( OperationAbortedByDlp, "unknown status code" )`.

A decode failure is caught and mapped to `finished( OperationAbortedByDlp, "Initialisation File could not be received" )`.

#### `bool` OperationImpl::initialisationFileOptionsNegotiation( const Arinc615aOptions& ) — `host/implementation/OperationImpl.cpp:342`

Strict validation of what the target echoed back. Returns `false` — which aborts the transfer — in four cases: a checksum option arrived when none was requested; a port option arrived when `--port-option` was off; the echoed port differs from the host's actual listening port; or the host requested the port option and the target did not echo it at all.

That last case logs `"Port option not accepted - Operation must be restarted with default port"`, which is the actionable message: rerun without `--port-option` against targets that predate ARINC 615A-3.

#### `void` OperationImpl::triggerDlpTimeout( seconds exceptionTime = 0s ) — `host/implementation/OperationImpl.cpp:149`

One line of policy: `timerV.expires_after( std::max( exceptionTime, dlpTimeoutV ) )`, then re-arm `timerHandler`. The target's advertised exception timer can only ever *extend* the window, never shorten it below the configured `--dlp-timeout`. Calling `expires_after` on a pending timer cancels it, which is what makes each status file reset the watchdog.

#### `void` OperationImpl::timerHandler( const error_code& ) — `host/implementation/OperationImpl.cpp:312`

Returns on `operation_aborted` (the re-arm path). A genuine timer error yields `finished( OperationAbortedByDlp, "Internal timer error" )`. Expiry yields `finished( OperationAbortedByDlp, "DLP Timeout" )`.

Carries a `//! @todo cancel active transfers`. In-flight TFTP transfers are *not* cancelled on DLP timeout; the server is stopped by `finished()`, but individual operations unwind on their own schedule.

#### `bool` OperationImpl::isAborted( const udp::endpoint& remote ) — `host/implementation/OperationImpl.cpp:186`

Called at the top of every status-file request handler — the designated moment to inject an abort. Returns `false` immediately if `abortReasonV == NoAbort`. Otherwise it maps the reason to a status code (`Operator` → `OperationAbortedByOperator`, `Protocol` → `OperationAbortedByDlp`), sends it via `tftpServerV->abortOperation( remote, statusCode )`, **resets `abortReasonV` to `NoAbort`**, re-arms the DLP timeout, and returns `true`.

The reset is deliberate: the abort is delivered exactly once, and the host then waits for the target to close the operation with its own final status file. That is why a single Ctrl-C does not end the process immediately.

#### `bool` OperationImpl::checkRequest( string_view filename, const udp::endpoint& remote ) — `host/implementation/OperationImpl.cpp:228`

The host's only authorisation check. If the filename parses as a protocol filename *and* its embedded Target ID differs from this operation's, the request is refused with TFTP error `FileNotFound` and the message `"Wrong target ID for protocol file"`. Non-protocol filenames — data files during upload and download — pass through unchecked.

#### `void` OperationImpl::finished( StatusCode status, string_view description = {} ) — `host/implementation/OperationImpl.cpp:159`

Three statements: `handlerV.finished( status, description )`, `timerV.cancel()`, `tftpServerV->stop()`. No idempotence guard, no state check. Every re-entry counts the command's latch down again. §22 lists the two confirmed paths that reach it twice.

#### `void` OperationImpl::doAbort( AbortReason ) · void doTerminate( AbortReason ) — `host/implementation/OperationImpl.cpp:77, 96`

`doAbort` is idempotent — it returns early if `abortReasonV != NoAbort`. It records the reason and, if the initialisation read is still in flight, calls `gracefulAbort()` on it. It does **not** call `finished()`; delivery is deferred to the next `isAborted()` check.

`doTerminate` has no such guard. It aborts the initialisation transfer if present, maps the reason to a status code, and calls `finished()` directly. Two SIGINTs in quick succession therefore reach `finished()` twice.

#### `string` OperationImpl::protocolFilename( ProtocolFileType ) const — `host/implementation/OperationImpl.cpp:171`

`static_cast<std::string>( Files::ProtocolFilename{ targetIdV, fileType } )` — the single place protocol filenames are built. See §14 for the extension table.

---

## 10 — Information

### Information: the reference operation

The simplest complete operation, and the one to understand first — the other three are variations on its structure. The host reads `.LCI`, the target pushes back `.LCL` (the Load Configuration List) and zero or more `.LCS` status files, and the host prints the target's part numbers. The host initiates exactly one transfer — the `.LCI` read. Everything after that is the target pushing files to the host TFTP server, which is why the server is started before the read is issued. Ordering is not guaranteed

A conforming target may send the `.LCL` with no status file at all, or bracket it with status files. The implementation handles both through the `waitForFinalStatusV` flag described below — do not assume the sequence above is the only legal one.

#### `void` InformationOperationImpl::start() — `host/implementation/InformationOperationImpl.cpp:70`

Two statements: clear `waitForFinalStatusV`, then `initialise( ProtocolFileType::LoadConfigurationInitialization )`. The file type is the only thing that distinguishes this operation from the other three at startup.

#### `void` InformationOperationImpl::tftpRequest( remote, requestType, filename, tftpOptions, arinc615aOptions ) — `host/implementation/InformationOperationImpl.cpp:87`

The inbound router. Parses the filename into a `ProtocolFilename`, runs `checkRequest()` and returns on failure, rejects non-write requests, then switches on file type: `LoadConfigurationList` → `listFileRequest()`; `LoadConfigurationStatus` → `statusFileRequest()`; anything else → TFTP `FileNotFound` with `"Wrong filename"`. Each accepted branch bumps `ProtocolFileStatistic::globalReceive()`. Confirmed defect — missing return

At `InformationOperationImpl.cpp:103–107` the non-`Write` branch sends `IllegalTftpOperation` but has no `return`. Control falls into the file-type switch and a second TFTP operation is created for a request already refused. Compare `UploadOperationImpl.cpp:180`, where the same check is a `switch` with proper `break`s. Add `return;` after the `errorOperation()` call.

#### `void` InformationOperationImpl::listFileRequest( remote, tftpOptions, arinc615aOptions ) — `host/implementation/InformationOperationImpl.cpp:260`

Accepts the target's push of the Load Configuration List. If a previous list transfer is still open, it is closed with `gracefulAbort( NotDefined, "New list file received" )` — last write wins.

The **checksum option is accepted here and nowhere else in this operation**: the value is captured, echoed back in `negotiatedArinc615aOptions`, and bound into the completion handler for verification. Part-number and port options are logged as unexpected and dropped.

A thrown `Arinc615aException` triggers `abort( AbortReason::Protocol )`.

#### `void` InformationOperationImpl::listFileCompleted( MemoryFilePtr, CheckValue, TransferStatus ) — `host/implementation/InformationOperationImpl.cpp:319`

Where the payload finally becomes console output.

1. On non-success: `finished( OperationAbortedByDlp, "Invalid Protocol file" )` if no status file has been seen, otherwise `abort( Protocol )` and let the target close out.
1. **Integrity check.** Build an `Arinc649::CheckValueGenerator` for the negotiated type, run it over the received bytes, compare to the advertised value. A mismatch sets `integrityInformation = false` and logs a warning — *it does not abort*.
1. Log the raw file, decode as `Files::LoadConfigurationListFile`.
1. Warn if the file's protocol version differs from the one latched at initialisation. Warning only.
1. `handlerV.targetInformation( listFile.targetsHardware(), integrityInformation )` — the command prints `Information Integrity: Valid|Invalid` and then each target hardware block.
1. If no status file was ever received, `finished( OperationCompleted )`. Otherwise `triggerDlpTimeout()` and wait for the target's final status.
Worth knowing

A checksum mismatch surfaces only as the word `Invalid` on one console line and a warning in the log. The exit code is unaffected and the part numbers are printed regardless. If integrity matters in your workflow, key on that line explicitly.

#### `void` InformationOperationImpl::statusFileRequest( remote, tftpOptions, arinc615aOptions ) — `host/implementation/InformationOperationImpl.cpp:135`

Calls `isAborted( remote )` first and returns if it fired — this is the abort injection point. Otherwise logs any unexpected ARINC 615A options, creates a TFTP server write operation, and pushes it onto `statusFileOperationsV`, a `std::forward_list` that allows concurrent status transfers.

#### `void` InformationOperationImpl::statusFileCompleted( MemoryFilePtr, WriteOperationPtr, TransferStatus ) — `host/implementation/InformationOperationImpl.cpp:180`

Removes the operation from the list; if it was not found, calls `finished()` and **returns** — this is the correct form of the pattern that is broken in the upload and download variants (§22).

Decodes as `Files::InformationOperationStatusFile`, warns on version mismatch, invokes `handlerV.status()`, then dispatches on the status code:

| Status code | Action |
| --- | --- |
| OperationAccepted | set `waitForFinalStatusV`, re-arm DLP timeout |
| OperationInProgress …AdditionalInfo | set `waitForFinalStatusV`, re-arm with the file's **exception timer** |
| OperationCompleted | `finished()` — terminal |
| OperationNotAccepted OperationNotSupported …AbortedByTargetHw …AbortedByDlp …AbortedByOperator | `finished()` with that code — terminal |
| anything else | warn, `finished( OperationAbortedByDlp )` |

The exception timer is honoured only for the two in-progress codes. That is the mechanism by which a target performing a long internal operation keeps the host from timing out.

#### Console output path

Four overrides in `InformationOperationCommand` produce everything the user sees, all on the I/O thread:

- `initialisationDeferred` — `:209` — prints the WAIT duration.
- `initialisationResponse` — `:214` — prints the initialisation code and description.
- `status` — `:237` — prints counter, code, description, exception timer, estimated time.
- `targetInformation` — `:255` — prints the integrity verdict, then `targetHardware.toString()` per entry.
- `finished` — `:225` — prints the final code and description, then `done.count_down()`.

---

## 11 — Upload

### Upload: host as file server

Upload inverts the data direction. After initialisation the host writes a `.LUR` request file naming the loads it intends to send, and then serves the target's read requests for the actual load files out of an ARINC 665 media set. The host is a TFTP server for both the status pushes and the data pulls.

The critical detail is *when* the load list is sent: not at `start()`, but from inside the `status()` handler, on the first status file carrying `OperationAccepted`.

#### `void` UploadOperationCommand::execute( … ) — `operations/UploadOperationCommand.cpp:158`

Follows the §7 skeleton with three extra steps between address resolution and operation construction:

1. `Arinc665::Utils::MediaSetManager::load( mediaSetManagerDirectoryV, checkMediaSetManagerIntegrityV, loadProgress )` — opens the media set store, optionally verifying every file. The progress callback prints `"Loading: n/N pn medium:total"`.
1. `mediaSetManagerV->mediaSet( mediaSetPartNumberV )`; a miss throws `program_options::error{ "Media Set '…' does not exist" }`. The returned pair yields the media set and its check values, stored in `checkValuesV`.
1. For each `--load-header`: `mediaSet->first->recursiveLoads( loadHeader )`. Zero matches and multiple matches both throw. Survivors are appended to `loadsV`.

All three failures are `program_options::error`, so they are re-thrown to the dispatcher and reported as usage errors before any network traffic occurs.

#### `void` UploadOperationCommand::status( const Information::UploadStatus& ) — `operations/UploadOperationCommand.cpp:334`

Prints the aggregate status — counter, code, description, list ratio, exception timer, estimated time — then one block per load with its header filename, part number, ratio, and per-load status. Flushes explicitly, because progress output matters here.

Then the trigger:

```cpp
if ( status.code() == StatusCode::OperationAccepted && !loadListTransmittedV )
{
// build UploadLoads from loadsV
operationV->loadList( std::move( loadsList ) );
loadListTransmittedV = true;
}
```

Each entry is an `Information::UploadLoad` holding only `headerFilename` and `partNumber` — taken from the ARINC 665 load object, not from anything the user typed. **This is the field pair the target matches against**; see §19 if your loads are named differently.

#### `void` UploadOperationImpl::loadList( Information::UploadLoads loads ) — `host/implementation/UploadOperationImpl.cpp:88`

Guarded by `loadListSentV` — a second call logs `"Load list can be only transmitted once"` and returns.

Builds a `Files::UploadOperationRequestFile` at the latched protocol version, encodes it into a `MemoryFile`, logs it via the protocol file logger, and issues a TFTP **write** to `<TargetId>.LUR` on the target's TFTP port. Completion routes to `loadListCompleted()`; a failed transfer calls `abort( AbortReason::Protocol )`.

#### `void` UploadOperationImpl::tftpRequest( … ) — `host/implementation/UploadOperationImpl.cpp:180`

Splits on request type rather than filename, which is the structural difference from the Information operation.

- **Read** — the target pulling a load file. Forwarded straight to `handlerV.fileRequest()` with the part number and check value extracted from the ARINC 615A options. The host layer creates no TFTP operation itself; the command does.
- **Write** — only `UploadStatus` (`.LUS`) is accepted; everything else is refused with `FileNotFound` and `"Wrong filename"`.

#### `void` UploadOperationCommand::fileRequest( remote, filename, tftpOptions, loadPartNumber, checkValue ) — `operations/UploadOperationCommand.cpp:391`

Resolves and serves one load file.

1. `Arinc665::Media::Loads_file( loadsV, checkValuesV, filename, loadPartNumber, checkValue )` — matches on all three of filename, part number, and check value. A miss sends TFTP `FileNotFound` and returns.
1. `mediaSetManagerV->filePath( file )` maps the logical file to a path on disk; a non-regular file also sends `FileNotFound`.
1. Opens a `Tftp::Files::StreamFile` in `Transmit` mode with the known file size — this is what populates the TFTP transfer-size option.
1. `operationV->fileTransfer( … )` builds the server read operation with the part number and checksum echoed back as negotiated options, then `start()`.

Concurrent transfers are held in `fileOperationsV`, a `forward_list`. Completion removes the entry; a failed transfer logs `"File transfer failed - Ignore it from the host side"` and does nothing else — the target is expected to report the failure in its own status file, and the per-load status codes in `status()` are where it surfaces.

#### `Tftp::Servers::ReadOperationPtr` UploadOperationImpl::fileTransfer( dataHandler, remote, tftpOptions, partNumber, checkValue ) — `host/implementation/UploadOperationImpl.cpp:128`

Builds but does *not* start the operation — the caller starts it, so it can attach its own completion handler first. Populates `Arinc615aOptions` with the part number when non-empty and the check value when its type is not `NotUsed`. The port option is explicitly never used for data transfers.

#### `void` UploadOperationImpl::statusFileCompleted( … ) — `host/implementation/UploadOperationImpl.cpp:321`

Structurally identical to the Information variant, decoding `Files::UploadOperationStatusFile` and dispatching on the same status-code table — except that `OperationAccepted` here only re-arms the timer; the `waitForFinalStatus` concept does not exist in this operation. Confirmed defect — missing return

At `UploadOperationImpl.cpp:326–330`, the "operation completed which was not initiated" branch calls `finished()` and then **falls through** into the rest of the function, which may call `finished()` again. `InformationOperationImpl.cpp:189` has the `return`; this one does not, and neither does `DownloadOperationImpl.cpp:162`. Two `finished()` calls means two `count_down()` on a latch of 1.

#### The three upload variants

`AdhocUpload`, `UploadLoads`, and `BatchUpload` reuse `UploadOperationImpl` unchanged. They differ only in how `loadsV` is populated during `execute()`:

| Command | Load source | Key options |
| --- | --- | --- |
| Upload | Registered media set manager, load looked up by header name inside a named media set | --media-set-manager-dir --media-set-pn --load-header |
| AdhocUpload | Raw media source directories imported on the fly, no persistent store | --source-directory --check-file-integrity --load-header |
| UploadLoads | Load header addressed directly by filesystem path | --load-header (path) --check-file-integrity |
| BatchUpload | ARINC 665 batch file; drives multiple targets, resolving each through the targets list | --batch-file --targets-list (required) --media-set-pn |

`BatchUpload` is the only command with no `--target-id` and no `--target-address`: targets come from the batch file, and `--targets-list` is `required()` because it is the only way to resolve them. It drives operations through `Host::BatchUploadOperationProxy`.

---

## 12 — Media download

### Media Defined Download: host names the files

The host initialises with `.LND`, writes a `.LNR` request file listing the filenames it wants plus optional user-defined data, and then receives those files as TFTP writes from the target. `DownloadOperationImpl` holds the machinery shared with the operator-defined variant; `MediaDefinedDownloadOperationImpl` adds the request file.

#### `void` MediaDefinedDownloadOperationImpl::request( DownloadFiles files, RawData userDefinedData ) — `host/implementation/MediaDefinedDownloadOperationImpl.cpp:106`

Guarded by `requestSentV`. Builds a `Files::DownloadOperationRequestFile` at the latched protocol version, logs it, and TFTP-writes it to `<TargetId>.LNR`. Called from the command's `status()` handler on the first `OperationAccepted`, exactly mirroring the upload load-list trigger.

The option negotiation handler rejects *any* ARINC 615A option echoed by the target on this transfer — the request file carries no part number, checksum, or port.

#### `void` MediaDefinedDownloadOperationImpl::tftpRequest( … ) — `host/implementation/MediaDefinedDownloadOperationImpl.cpp:150`

Read requests are always wrong here and are refused with `IllegalTftpOperation`. Write requests split on file type: `DownloadStatus` (`.LNS`) goes to `statusFileRequest()`; **everything else is treated as a data file** and forwarded to `handlerV.fileRequest()` with the part number and check value from the options.

That default branch is the important one — downloaded filenames are arbitrary and target-defined, so the router cannot whitelist them.

#### `void` MediaDefinedDownloadOperationCommand::fileRequest( remote, filename, tftpOptions, partNumber, checkValue ) — `operations/MediaDefinedDownloadOperationCommand.cpp:358`

Builds the destination path as `downloadDataPathV / Helper::normaliseFilename( filename )`. The normalisation call is the path-traversal guard — the target supplies this filename, and it is not otherwise validated.

Registers the transfer with `downloadInformationV.fileStart()`, opens a `StreamFile` in `Receive` mode, creates the server write operation via `operationV->fileTransfer()`, binds a completion handler carrying the filename, path, and expected transfer size, and starts it.

#### `void` MediaDefinedDownloadOperationCommand::fileCompleted( filename, filePath, expectedFileSize, operation, status ) — `operations/MediaDefinedDownloadOperationCommand.cpp:396`

On failure, records `DownloadInformation::TransferError` and returns — the download continues for other files. On success, compares the on-disk size against the TFTP transfer-size option and **warns only** if they differ, then records `TransferOk` with the actual size.

A size mismatch does not fail the operation, delete the file, or change the exit code.

#### Where downloaded files land

Controlled by three options. `--download-base-directory` sets the root. By default a per-operation sub-directory is created beneath it; `--no-download-directory,-n` disables that and writes directly into the base. `--no-check-validity,-c` disables the post-transfer validity check. If `--file` is given no values, the operation is cancelled after the list transfer rather than downloading anything.

---

## 13 — Operator download

### Operator Defined Download: target advertises first

The negotiation runs one step longer. The host initialises with `.LNO`; the target pushes a `.LNL` list file advertising what it can provide; the host replies with a `.LNA` answer file selecting from that list; the selected files then transfer. Everything else matches §12.

#### `void` OperatorDefinedDownloadOperationImpl::listFileRequest( … ) · listFileCompleted( … ) — `host/implementation/OperatorDefinedDownloadOperationImpl.cpp:224, 271`

`listFileRequest` closes any in-flight list transfer with `gracefulAbort`, logs unexpected ARINC 615A options, and accepts the write into a `MemoryFile`. Unlike the Information operation's list handler, **no checksum option is accepted** — the `.LNL` is not integrity-checked.

`listFileCompleted` decodes a `Files::DownloadOperationListFile`, warns on version mismatch, and calls `handlerV.downloadingList( listFile.files() )`. A transfer failure or decode failure both call `abort( AbortReason::Protocol )`.

#### `void` OperatorDefinedDownloadOperationImpl::answer( Information::DownloadFiles files ) — `host/implementation/OperatorDefinedDownloadOperationImpl.cpp:112`

Guarded by `answerSentV`. Builds a `Files::DownloadOperationAnswerFile` and TFTP-writes it to `<TargetId>.LNA`. The command calls this from its `downloadingList` handler: with `--download-all` it echoes the whole advertised list, otherwise it intersects the advertisement with the `--file` values.

Like the media-defined request, the answer transfer rejects any echoed ARINC 615A option.

#### `void` OperatorDefinedDownloadOperationImpl::tftpRequest( … ) — `host/implementation/OperatorDefinedDownloadOperationImpl.cpp:154`

Same shape as §12 with one extra case: `OperatorDefinedDownloadList` routes to `listFileRequest()`. `DownloadStatus` routes to the shared status handler; every other filename is a data file.

#### `WriteOperationPtr` DownloadOperationImpl::doFileTransfer( … ) · void statusFileCompleted( … ) — `host/implementation/DownloadOperationImpl.cpp:57, 157`

Shared by both download variants. `doFileTransfer` builds a server write operation with part number and check value echoed as negotiated options. `statusFileCompleted` decodes `Files::DownloadOperationStatusFile` and runs the same status-code dispatch table as §10 — including the same missing `return` at `:162` flagged in §22.

---

## 14 — Protocol files

### Protocol file codec

Every ARINC 615A protocol file shares a six-byte header and is built from three primitive encodings: big-endian integers, length-prefixed NUL-terminated strings, and three-character ASCII ratios. All of it lives under `lib/arinc_615a/files/`. The length field is redundant with the transfer size but is validated strictly, which makes truncated protocol files fail fast rather than decode into garbage.

#### `void` ProtocolFile::insertHeader( RawDataSpan ) const · ConstRawDataSpan decodeHeader( ConstRawDataSpan ) — `files/ProtocolFile.cpp:43, 52`

`insertHeader` is called *last* by every `encode()` — the body is built first, then the now-known total length and the version are written into the reserved leading six bytes.

`decodeHeader` enforces three things and throws `Arinc615aException` on each: the buffer is at least `HeaderSize`; the embedded length equals the actual buffer size (`"internal length field and data size differs"`); the version is one of `Arinc615a2` (0x4133) or `Arinc615a34` (0x4134). **`Arinc615a1` (0x4132) is deliberately rejected** — supplement 1 protocol files are not supported.

#### `RawData` String_encode( string_view, uint8_t fixedLength = 0 ) · tuple<span,string_view> String_decode( ConstRawDataSpan ) — `files/String.cpp:64, 24`

`String_encode` computes `rawStringSize` as zero for an empty string, otherwise `size() + 1` for the terminator. Throws if that reaches 255 or exceeds `fixedLength` when one is given. Writes the length byte, then the characters, then forces the final byte to NUL.

`String_decode` reads the length byte, validates the remaining buffer is long enough (`"string length inconsistent"`), and for non-zero lengths requires an embedded NUL (`"string not NULL terminated"`). The returned string is truncated at that NUL — **a declared length longer than the actual text is legal**, which is exactly the ARINC 615A-2 change noted in the project README.

#### `tuple<span,Ratio>` Ratio_decode( ConstRawDataSpan ) · RawData Ratio_encode( const Ratio& ) — `files/Ratio.cpp`

Ratios are three ASCII digits, space-padded via `std::format( "{:3}", value )`. Decode parses with `std::stoul` and rejects values above 100. A parse failure becomes `Arinc615aException`. These carry upload and download completion percentages in status files.

#### Filename mapping

`ProtocolFilename` splits on the first `.`, validates the stem as a Target ID, and maps the extension through a Boost.MultiIndex table at `files/ProtocolFilename.cpp:148`. Both directions are supported, which is why `isProtocolFilename()` can reject unknown extensions cheaply.

| Ext | ProtocolFileType | Operation | Direction |
| --- | --- | --- | --- |
| LCI | LoadConfigurationInitialization | Information | host reads |
| LCL | LoadConfigurationList | Information | target writes |
| LCS | LoadConfigurationStatus | Information | target writes |
| LUI | UploadInitialization | Upload | host reads |
| LUR | UploadRequest | Upload | host writes |
| LUS | UploadStatus | Upload | target writes |
| LND | MediaDefinedDownloadInitialization | Med download | host reads |
| LNR | MediaDefinedDownloadRequest | Med download | host writes |
| LNO | OperatorDefinedDownloadInitialization | Op download | host reads |
| LNL | OperatorDefinedDownloadList | Op download | target writes |
| LNA | OperatorDefinedDownloadAnswer | Op download | host writes |
| LNS | DownloadStatus | both downloads | target writes |

#### File bodies

| Class | Body layout after the 6-byte header |
| --- | --- |
| InitializationFile | uint16 acceptance code, then description string |
| LoadConfigurationListFile | uint16 THW count; per THW: literal name, serial number, uint16 P/N count; per P/N: part number, amendment, part designation |
| InformationOperationStatusFile | uint16 counter, uint16 status code, uint16 exception timer, int16 estimated time, description string |
| DownloadOperationRequestFile | uint16 file count, N filename strings, uint8 UDD length, UDD bytes |
| UploadOperationRequestFile | uint16 load count; per load: header filename, part number |
| UploadOperationStatusFile | counter, status, ratio, exception timer, estimated time, description, then per-load status blocks |

Every `decode()` ends with a check that no bytes remain (`"More data then expected"`), so a target sending vendor extensions past the defined fields will have its file rejected outright. §19 covers this if you need to tolerate it.

#### `void` ProtocolFileLogger::logProtocolFile( prefix, filename, file ) — `files/ProtocolFileLogger.cpp`

Enabled by `--log-protocol-files`. Writes every protocol file, in both directions, as a raw binary dump named `{ISO-8601 timestamp}_{operation}_{RX|TX}_{protocol filename}` into `loggingDirectoryV`. The directory defaults to the process working directory — the CLI never calls `loggingDirectory()`, so files land wherever the binary was run.

This is the single most useful diagnostic in the tool. When a target rejects a file or the host rejects a decode, the exact bytes are on disk.

---

## 15 — TFTP layer

### What ARINC 615A adds to TFTP

`lib/arinc_615a/tftp/` is a decorator over the generic `tftp` dependency. It adds three things and nothing else: extra option names, two overloaded uses of the TFTP error packet, and a retry layer above the TFTP retry layer.

#### Option names on the wire

`Arinc615aOptions_name()` at `tftp/Arinc615aOptions.cpp` is the entire mapping. These strings go into the TFTP option-negotiation fields verbatim:

| Wire name | Meaning | Check value type |
| --- | --- | --- |
| port | ARINC 615A-3 Port Option — host's dynamic TFTP server port | — |
| part number | Load part number for a data transfer (note the space) | — |
| checksum_1 | CRC8 | Crc8 |
| checksum_2 | CRC16 | Crc16 |
| checksum_3 | CRC32 | Crc32 |
| checksum_4 | MD5 | Md5 |
| checksum_5 | SHA1 | Sha1 |
| checksum_6 | SHA256 | Sha256 |
| checksum_7 | SHA512 | Sha512 |
| checksum_8 | CRC64 | Crc64 |

`Arinc615aOptions_checksum()` extracts whichever checksum option is present, and returns `{ false, {} }` — a negotiation failure — if **more than one** checksum option appears, or if the value fails to parse into a valid `Arinc649::CheckValue`. Exactly zero or one is legal.

#### Error packets carry protocol semantics

ARINC 615A overloads the TFTP error packet, with error code `NotDefined` and a structured message, to carry two protocol events. `ErrorMessage_type()` classifies them:

| Message | Meaning | Host reaction |
| --- | --- | --- |
| WAIT:<seconds> | Target is busy; retry after the stated delay | Arm a timer, call `operationDeferredHandler`, retry **without** consuming a retry |
| ABORT:<4 hex digits> | Target is terminating with this ARINC 615A status code | Map to `OperationAbortedByDlp` or `…ByOperator` and complete |

`ErrorMessage_abort()` parses the four hex digits with `std::from_chars` base 16 and validates the result against the known status codes, returning `StatusCode::Invalid` for anything unrecognised. `ErrorMessage_wait()` parses base 10 into a `uint16_t` and returns `std::nullopt` on failure. Both are `noexcept`; malformed messages degrade to "not an ARINC 615A message" rather than throwing.

#### The DLP retry layer
The teal path is what makes WAIT different from every other failure: it re-issues the request after the target's stated delay without spending a retry, so a busy target cannot exhaust the counter.

#### `void` Tftp::Clients::OperationImpl::handleCompletion( ::Tftp::TransferStatus status ) — `tftp/clients/implementation/OperationImpl.cpp:155`

The function the diagram describes. Success clears the operation and error information and reports `TransferStatus::Successful`. `Aborted` reports a local abort. `RequestError` is classified as ABORT, WAIT, or neither.

ABORT handling is conditional on `handleAbortV`, and only two status codes are honoured — `OperationAbortedByDlp` and `OperationAbortedByOperator`. Any other abort code logs `"Invalid ABORT status code"` and **falls through to the retry path**, which is why the diagram has that long return edge.

Everything that reaches the bottom increments `retriesV` and re-issues via `tftpOperation()` until `retriesV > dlpRetriesV`, at which point it reports `CommunicationError`.

#### `bool` Tftp::Clients::OperationImpl::handleOptionNegotiation( ::Tftp::Packets::Options& serverOptions ) — `tftp/clients/implementation/OperationImpl.cpp:115`

Destructively extracts the ARINC 615A options from the server's option map — port, part number, checksum — leaving only generic TFTP options behind. Anything left over that the generic TFTP layer does not recognise will abort the transfer, which is how unknown options are rejected without an explicit check here.

The assembled `Arinc615aOptions` is then handed to the operation-specific `optionNegotiationHandlerV` — the strict validators in §9 and §11.

#### `::Tftp::Clients::OperationPtr` ReadOperationImpl::tftpOperation() — `tftp/clients/implementation/ReadOperationImpl.cpp`

Called once per attempt — including every retry — so each retry rebuilds the underlying TFTP operation from scratch. Assembles `additionalOptions` from the port, part number, and checksum if set, then configures the real TFTP read with `TransferMode::OCTET`, the two ARINC 615A handlers, the data sink, and a local endpoint bound to port 0.

---

## 16 — Timeout & abort

### Three timers, two abort modes

Timeouts nest. Understanding which layer fired is most of diagnosing a stalled load.

| Timer | Default | Scope | Option |
| --- | --- | --- | --- |
| TFTP packet timeout | 2 s | One TFTP packet; retried `tftpRetries` times | --tftp-timeout |
| DLP retry | 1 | Whole TFTP transfer, re-issued from scratch | --dlp-retries |
| DLP timeout | 13 s | Watchdog on the whole operation; re-armed by each status file | --dlp-timeout |
| FIND receive window | 3 s | FIND only; fixed listening period | --timeout |

The DLP timeout is the only one that ends an operation unconditionally. Its effective value is `max( exceptionTimer, dlpTimeout )`, so a target that advertises a long exception timer extends the window but can never shrink it below the configured value. Defaults are declared in `Arinc615a.hpp` as `DefaultArinc615aTftpTimeout`, `DefaultArinc615aTftpRetries`, `DefaultArinc615aDlpTimeout`, and `DefaultArinc615aDlpRetries`. Documentation mismatch

The manpage states `--dlp-retries` defaults to 2; the source constant `DefaultArinc615aDlpRetries` is `1`. The source is authoritative.

#### Abort versus terminate

- **Abort** (first Ctrl-C, or `AbortReason::Protocol` raised internally) records the reason and waits. Delivery happens at the next status-file request via `isAborted()`, which sends an ARINC 615A ABORT error and lets the target close the operation with its own final status. Idempotent.
- **Terminate** (second Ctrl-C) calls `finished()` directly with a locally synthesised status. Not idempotent — see §22.

If the target has gone silent, abort never delivers, and the operation ends on the DLP timeout instead. Pressing Ctrl-C twice is the intended escape.

---

## 17 — Status codes

### Status code reference

Declared in `Arinc615a.hpp`. These values appear in status files, in initialisation responses, and inside `ABORT:` error messages.

| Value | Enumerator | Meaning | Terminal |
| --- | --- | --- | --- |
| 0x0001 | OperationAccepted | Accepted, not yet started | no |
| 0x0002 | OperationInProgress | Running; exception timer valid | no |
| 0x0003 | OperationCompleted | Completed without error | yes |
| 0x0004 | OperationInProgressAdditionalInfo | Running, with description text (615A-3+) | no |
| 0x1000 | OperationNotAccepted | Denied — initialisation response only | yes |
| 0x1002 | OperationNotSupported | Not supported — initialisation response only | yes |
| 0x1003 | OperationAbortedByTargetHw | Target aborted | yes |
| 0x1004 | OperationAbortedByDlp | Data loader aborted | yes |
| 0x1005 | OperationAbortedByOperator | Operator aborted | yes |
| 0x1007 | LoadPartNumberOrDownloadFileFailed | Per-file or per-load failure inside a status file | per item |
| 0xFFFE | OperationDeferred | **Internal only** — signals a WAIT response, never on the wire | no |
| 0xFFFF | Invalid | Sentinel for parse failure | — |

`statusCode( uint16_t )` at `StatusCode.cpp` validates on decode and **throws `Arinc615aException`** for any value not in this table — including `OperationDeferred`, which is why that code can never legally arrive from a target. `status( OperationClass, StatusCode, description, … )` renders the human-readable sentences used in log output.

---

## 18 — DRF generator

### arinc_615a_download_request_file

A single-file utility that writes a `.LNR` download request file to disk without touching the network. Useful for preparing request files that a different tool or a media-based workflow will deliver, and for producing known-good inputs when testing a target.

Its `main()` at `arinc_615a_download_request_file.cpp` parses five options, builds a `Files::DownloadOperationRequestFile`, appends each `--download-file` via `requestFile.file()`, attaches `--user-defined-data` as raw bytes, and writes the encoded result with a binary `std::fstream`.

| Option | Required | Notes |
| --- | --- | --- |
| -f, --filename | yes | Output path for the generated file |
| --download-file | yes | Multitoken — repeat or list to add several filenames |
| --user-defined-data | no | Copied verbatim as bytes; max 255 |
| --arinc615a-version | no | `A3` = 615A-2 (default), `A4` = 615A-3/4 |
| -l, --log-level | no | Defaults to `warn` |
Not built in this variant

`app/CMakeLists.txt` no longer contains `add_subdirectory( arinc_615a_download_request_file )`, so this tool is not produced by the build described in §20. The sources are still present under `app/arinc_615a_download_request_file/`; add that line back to `app/CMakeLists.txt` and reconfigure to build it. The same applies to `arinc_615a_test_tha` and `arinc_615a_unit_test`. Quirk

`--help` returns `EXIT_FAILURE`, not success. Scripts that treat a zero exit from `--help` as a capability probe will misread this binary.

---

## 19 — Customisation

### Where to change things for your application

Most adaptation is a command-line option and needs no rebuild. The rest is a small number of specific constants and tables. This section is ordered by how invasive the change is.

#### Tier 1 — runtime options, no rebuild

Every field below is a member of `Arinc615aConfiguration` or a per-command variable, and every one is settable on the command line. The *Field* column names the C++ member, so you can trace it into the code from here.

| What you are changing | Option | Field | Default |
| --- | --- | --- | --- |
| Which target you talk to | --target-address | targetAddressV | — |
| Which target ID names the protocol files | --target-id | targetIdV | — (required) |
| Which NIC the host TFTP server binds to | --local-tftp-address | localInterfaceAddress | 0.0.0.0 |
| Target TFTP port | --server-port | tftpConfiguration.tftpServerPort | 59 |
| Per-packet TFTP timeout | --tftp-timeout | tftpConfiguration.tftpTimeout | 2 s |
| TFTP dally on last ACK | --dally | tftpConfiguration.dally | — |
| Whole-transfer retries | --dlp-retries | dlpRetries | 1 |
| Operation watchdog | --dlp-timeout | dlpTimeoutV | 13 s |
| TFTP block size negotiation | --block-size-option | tftpOptionsConfiguration | 1486 if bare |
| TFTP timeout option negotiation | --timeout-option | tftpOptionsConfiguration | 2 s if bare |
| Transfer-size option handling | --handle-transfer-size-option | tftpOptionsConfiguration | true |
| Dynamic host port (615A-3) | --port-option | portOptionV | off |
| Dump every protocol file | --log-protocol-files | protocolFileLogging | off |
| Log verbosity, all libraries | --log-level | — | warn |
There is no config file

`Arinc615aConfiguration::fromProperties()` and `toProperties()` exist and read JSON keys `local_tftp_address`, `dlp_retries`, `tftp`, `tftp_options`, `protocol_file_logging` — but **the CLI never calls them**. They serve the test target application. If you want a config file for the CLI, that is the hook to wire up: parse the JSON in `execute()` before `notify()` and let command-line options override it.

#### Tier 2 — your data, not the code

Three inputs are files you author, not code you edit:

- **Targets list JSON** — produced by `Find --targets-list`, consumed by every other command. Structure comes from `TargetInformation::toProperties()`: an array of `target` objects each with `address`, `thwId`, `thwTypeName`, `thwPosition`, `literalName`, `manufacturerCode`. You can hand-write this file for targets that do not answer FIND — only `address`, `thwId`, and `thwPosition` actually matter, since the lookup key is `TargetId{ thwId, thwPosition }`.
- **ARINC 665 media set** — the load headers and part numbers uploaded by `Upload`. Managed by the `arinc_665` commands in the same binary.
- **Download request file** — generate with the §18 tool, or let `MedDownload --file` build it in memory.

#### Tier 3 — source changes, rebuild required

Each row names the exact file and symbol. These are the points where the implementation encodes an assumption that your hardware might not share.

| If you need to… | Edit | What to change |
| --- | --- | --- |
| Accept Target IDs your hardware reports that are shorter, longer, or contain non-alphanumerics | TargetId.hpp:39–48 | `ThwIdSizeMin` 4, `ThwIdSizeMax` 15, `PositionSizeMin` 0, `PositionSizeMax` 8. For non-alphanumerics you must also relax `isCompliant()` in `TargetId.cpp`, which currently requires `isalnum` in the C locale. |
| Change the Target ID separator from `_` | TargetId.cpp | `splitTargetId()` hard-codes `find('_')`. Also update the formatter that rebuilds the string. |
| Use vendor-specific protocol file extensions | ProtocolFilename.cpp:148 | The `extensionTypeInfoIndex()` static table maps twelve extensions to `ProtocolFileType`. Both directions come from this one table — change it and filename generation and parsing stay consistent. |
| Change the default protocol version written into files you generate | Arinc615a.hpp | `Arinc615aVersion`: `Arinc615a2`=0x4133, `Arinc615a34`=0x4134. Note the host normally *latches* the version from the target's initialisation file, so this only affects the offline generator (§18) and its `--arinc615a-version` default. |
| Accept ARINC 615A-1 (`A2`, 0x4132) protocol files | ProtocolFile.cpp:77–86 | `decodeHeader()` whitelists only `Arinc615a2` and `Arinc615a34`. Add the case — but supplement 1 has different field semantics, so verify each decoder too. |
| Tolerate targets that append vendor data past the defined fields | every files/*.cpp decode() | Each `decode()` ends with a check that no bytes remain and throws `"More data then expected"`. Relax to a warning per file type you need to accept. |
| Change the FIND port or the answer field set | find/Find.hpp:47 find/packets/Packets.hpp:58 | `DefaultPort` = 1001 and `DefaultReceiveTimeout` = 3 s. The five answer fields and their order come from the `ParameterList` enum; `QueryImpl.cpp:235` rejects any answer whose count is not exactly `ParameterList::Last`. |
| Change the default timing constants baked into `--help` | Arinc615a.hpp | `DefaultArinc615aTftpPort` 59, `DefaultArinc615aTftpTimeout` 2 s, `DefaultArinc615aTftpRetries` 1, `DefaultArinc615aDlpTimeout` 13 s, `DefaultArinc615aDlpRetries` 1. |
| Change or extend the checksum option names | tftp/Arinc615aOptions.cpp | Both `Arinc615aOptions_name()` overloads. The `KnownOptions` enum in `Arinc615a.hpp` must gain a matching entry, and `Arinc615aOptions_checksum()` must include the new type in its scan list. |
| Send protocol file dumps somewhere other than the working directory | host/implementation/*Impl.cpp constructors | Each constructor calls `protocolFileLogger().loggingEnabled(…).operation(…)` but never `.loggingDirectory(…)`. Add that call — and expose it as an option alongside `--log-protocol-files`. |
| Fix the unusable `-l` short flag | 9 command .cpp files | Rename `"targets-list,l"` to a free letter in all nine. See §22. |

#### Adding a command of your own

The pattern is mechanical. Four steps:

1. Write `MyCommand` in `lib/arinc_615a_commands/operations/`, privately inheriting the handler interface for the operation class you need. Follow the §7 skeleton exactly.
1. Add both files to the `target_sources()` list in `lib/arinc_615a_commands/CMakeLists.txt`.
1. In `Operations::registerCommands()` (`operations/Operations.cpp:27`), construct it and call `registry->command( "MyName", "description", bind_front(&MyCommand::execute, ptr), bind_front(&MyCommand::help, ptr) )`.
1. Add a manpage `.adoc` under `doc/arinc_615a_manpage/` and register it in that directory's `CMakeLists.txt` if you want it installed.

If your command needs protocol behaviour that none of the four operations provide, the extension point is a new `OperationImpl` subclass plus a factory method on `Host::Protocol` and `ProtocolImpl` — the pattern in `host/implementation/ProtocolImpl.cpp`.

---

## 20 — Building

### Building the CLI, command by command

One executable is built: `arinc_615a_operation`. Everything below is a single command, what it does, and how to tell it worked before you run the next one. What the variant excludes

`lib/CMakeLists.txt` adds `arinc_615a` and `arinc_615a_commands`; `app/CMakeLists.txt` adds `arinc_615a_operation`. Five dependencies are fetched: `helper`, `arinc_649`, `arinc_665`, `tftp`, `commands`.

The sources for `arinc_615a_download_request_file`, `arinc_615a_test_tha`, and `arinc_615a_unit_test` are still on disk under `app/` but are no longer referenced by any `add_subdirectory()`, so they will not build. §18 explains how to re-enable the request-file generator if you want it.

#### Step 0 — check the toolchain, one command at a time

Run these four before configuring. Each failure has a distinct fix, and a bad result here produces a confusing error much later.

1. **CMake version.** Must report **4.3 or higher** — every `CMakeLists.txt` in this tree declares `cmake_minimum_required( VERSION 4.3 )`.

```
cmake --version
```
If this reports 3.x you cannot proceed. The CMake bundled inside Visual Studio 2022 is 3.31 and is *not* sufficient; install CMake separately and make sure it precedes the VS copy on `PATH`.
1. **Ninja.** Every preset in this repository sets `"generator": "Ninja"`, so Ninja must be on `PATH`. There is no Makefile or Visual Studio solution preset.

```
ninja --version
```

1. **Compiler.** The targets request `cxx_std_23`, so you need GCC 14+, Clang 18+, or MSVC 19.4x.

```
g++ --version        # or: clang++ --version, or: cl
```

1. **Dependency host reachability.** Configure will clone five repositories from `git.thomas-vogt.de`. Confirm you can reach it before waiting on a configure that will fail.

```
git ls-remote --heads https://git.thomas-vogt.de/thomas-vogt/helper.git
```
A commit hash and `refs/heads/main` means you are fine. A timeout or auth prompt means you need the offline path at the end of this section.

#### Track A — Linux with GCC

1. **Install the system dependencies.** Boost, libxml++, and spdlog are found by `find_package`, not fetched.

```
sudo apt install -y build-essential g++-14 ninja-build git pkg-config \
libboost-all-dev libxml++2.6-dev libspdlog-dev
```
On Fedora or RHEL substitute `dnf install gcc-c++ ninja-build boost-devel libxml++-devel spdlog-devel`.
1. **Change into the source root.** Presets are resolved relative to the directory holding `CMakePresets.json`, so every later command assumes you are here.

```
cd arinc_615a-main
```

1. **List the presets** to confirm CMake parses them and to see the exact names available to you.

```
cmake --list-presets
```
You should see sixteen configure presets: four each of `gcc-`, `clang-`, `msvc-`, and `mingw-cross-`. The `msvc-` entries are conditioned on a Windows host and will be hidden on Linux.
1. **Configure.** This is the long step: it clones the five dependencies, runs every `find_package`, and writes the Ninja build files.

```
cmake --preset gcc-shared-release
```
Creates `cmake-build-gcc-shared-release/`. Success ends with `-- Generating done` and `-- Build files have been written to: …`. Anything else, stop and read §"When a step fails" below rather than re-running.
1. **Build.** The build preset name matches the configure preset name.

```
cmake --build --preset gcc-shared-release
```
Add `-j$(nproc)` if Ninja is not already saturating your cores. To build the single executable and skip everything not needed to link it, target it by name instead:

```
cmake --build cmake-build-gcc-shared-release --target arinc_615a_operation
```

1. **Confirm the binary exists** and is the architecture you expect.

```
ls -l cmake-build-gcc-shared-release/app/arinc_615a_operation/arinc_615a_operation
file  cmake-build-gcc-shared-release/app/arinc_615a_operation/arinc_615a_operation
```

1. **Run it once with no arguments.** This prints the banner and the usage text — and settles the `--command` versus `--operation` question from §5 for your build.

```
./cmake-build-gcc-shared-release/app/arinc_615a_operation/arinc_615a_operation
```
Expect `ARINC 615A Operation - <version>` followed by the list of registered command names.
1. **Install** into the preset's install tree. This also gathers the shared libraries declared through `RUNTIME_DEPENDENCY_SET arinc_615a-runtime-deps`.

```
cmake --install cmake-build-gcc-shared-release
```
Lands in `cmake-install-gcc-shared-release/`. Pass `--prefix /usr/local` to install elsewhere, and `--component runtime` to take only the binaries and their libraries without the documentation.
Want one file you can copy to another machine?

Use the static preset instead. `cmake --preset gcc-static-release` then `cmake --build --preset gcc-static-release` leaves `BUILD_SHARED_LIBS` off, so the project's own libraries are archived into the executable and there is no `cmake-install` dance to get the `.so` files alongside it. System libraries such as Boost are still linked dynamically unless your distribution ships static variants.

#### Track B — Windows with MSVC and vcpkg

The `msvc-` presets set `toolchainFile` to `$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake`, so `VCPKG_ROOT` must be set before you configure — that is the single most common first-run failure on Windows.

1. **Clone vcpkg** somewhere permanent. Do this once per machine, not once per project.

```
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
```

1. **Bootstrap it.** Builds `vcpkg.exe` from the cloned sources.

```
C:\vcpkg\bootstrap-vcpkg.bat
```

1. **Set `VCPKG_ROOT` permanently** so the preset can expand it.

```
setx VCPKG_ROOT C:\vcpkg
```
`setx` affects *new* shells only. Close and reopen your terminal, then confirm with `echo %VCPKG_ROOT%` before continuing.
1. **Open a Developer Command Prompt for VS 2022**, or import the MSVC environment into your current shell. Without this, CMake will not find `cl.exe`.

```
"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
```

1. **Confirm the compiler is now visible.**

```
where cl
cl
```
`cl` with no arguments prints its version banner. Anything else means step 4 did not take effect in this shell.
1. **Change into the source root.**

```
cd arinc_615a-main
```

1. **Configure.** vcpkg reads `vcpkg.json` automatically in manifest mode and builds Boost, libxml++, pkgconf, and spdlog into the build tree. Expect this to take a long time on a cold vcpkg cache — tens of minutes is normal, and almost all of it is Boost.

```
cmake --preset msvc-shared-release
```

1. **Build.**

```
cmake --build --preset msvc-shared-release
```

1. **Confirm and run.**

```
dir cmake-build-msvc-shared-release\app\arinc_615a_operation\arinc_615a_operation.exe
cmake-build-msvc-shared-release\app\arinc_615a_operation\arinc_615a_operation.exe
```

1. **Install,** which is what collects the dependent DLLs next to the executable.

```
cmake --install cmake-build-msvc-shared-release
```
Note the presets set `VCPKG_APPLOCAL_DEPS=OFF`, so vcpkg does *not* copy DLLs beside the binary at build time. If you skip the install step and run straight from the build tree, expect missing-DLL errors — use the install tree, or build a `msvc-static-release` variant instead.

#### Track C — cross-compiling for Windows from Linux

1. **Install the MinGW-w64 toolchain.**

```
sudo apt install -y mingw-w64
```

1. **Configure with the cross preset,** which points CMake at `mingw64-cross-toolchain.cmake` in the repository root.

```
cmake --preset mingw-cross-shared-release
```

1. **Build.** The CLI target explicitly links `wsock32` and `ws2_32`, which MinGW requires and MSVC links implicitly — so this works where a naive cross build would fail at link time.

```
cmake --build --preset mingw-cross-shared-release
```

#### Choosing a variant

Each compiler family offers the same four. The only cache difference is `CMAKE_BUILD_TYPE` and `BUILD_SHARED_LIBS`.

| Preset suffix | Build type | BUILD_SHARED_LIBS | Use it when |
| --- | --- | --- | --- |
| -static-release | Release | off | Shipping a portable binary; simplest to deploy |
| -shared-release | Release | on | Normal development; faster relink |
| -static-debug | Debug | off | Stepping through with a debugger |
| -shared-debug | Debug | on | Iterating on library code with symbols |

Append `-with-doc` to any *build* preset name to additionally generate the Doxygen and AsciiDoc output — `cmake --build --preset gcc-shared-release-with-doc`. That requires Doxygen and Asciidoctor and is unrelated to producing the binary.

#### When a step fails

| Symptom | Failing step | Fix |
| --- | --- | --- |
| CMake 4.3 or higher is required | configure | Your `cmake` is too old. Check `cmake --version`; the VS-bundled 3.31 does not qualify. |
| Could not find toolchain file … vcpkg.cmake | configure (MSVC) | `VCPKG_ROOT` is unset in *this* shell. `setx` only affects new shells — reopen the terminal. |
| No CMAKE_CXX_COMPILER could be found | configure (MSVC) | You are not in a Developer Command Prompt. Run `vcvars64.bat`. |
| CMake Error … generator Ninja not found | configure | Install Ninja and put it on `PATH`. Every preset requires it. |
| Hang or failure inside `FetchContent` | configure | `git.thomas-vogt.de` is unreachable. Use the offline path below. |
| Could NOT find Boost / spdlog / LibXML++ | configure | System packages missing (Linux) or vcpkg manifest install failed (Windows). Read the vcpkg log path printed in the error. |
| Build stops on a warning | build | All presets set `CMAKE_COMPILE_WARNING_AS_ERROR=True`. A newer compiler may warn where the authors' did not. Reconfigure with `-DCMAKE_COMPILE_WARNING_AS_ERROR=OFF` to triage. |
| .dll not found | running (Windows) | `VCPKG_APPLOCAL_DEPS` is off. Run `cmake --install` and use the install tree, or build a static preset. |
| Configure succeeds but nothing builds | build | Confirm `app/CMakeLists.txt` still contains `add_subdirectory( arinc_615a_operation )`. In this variant it is the only application wired in. |

#### Building without network access

The root `CMakeLists.txt` has an escape hatch built into each dependency declaration. For every dependency it checks `if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/<name> )` and, when that directory exists, sets `FETCHCONTENT_SOURCE_DIR_<NAME>` to it instead of cloning.

So on a machine with network access, clone the five repositories into the source root using exactly these directory names:

```
cd arinc_615a-main
git clone https://git.thomas-vogt.de/thomas-vogt/helper.git      helper
git clone https://git.thomas-vogt.de/thomas-vogt/arinc-649.git   arinc-649
git clone https://git.thomas-vogt.de/thomas-vogt/arinc_665.git   arinc_665
git clone https://git.thomas-vogt.de/thomas-vogt/tftp.git        tftp
git clone https://git.thomas-vogt.de/thomas-vogt/commands.git    commands
```

Note `arinc-649` uses a hyphen while `arinc_665` uses an underscore — the directory names must match the `IS_DIRECTORY` checks exactly or the redirect silently does not happen and CMake clones anyway. Transfer the whole tree to the offline machine and configure normally. Not yet verified end to end

These commands are derived from the presets and manifests in this tree, not from a completed build — no compiler toolchain was installed on the machine this document was written on. Treat your first configure and build as verification steps.

---

## 21 — Running

### Using the binaries
Establish the selection flag first

Because the repository documents both `--command=` and `--operation=` (§5), run the bare binary once and read its own usage output before anything else. Every example below is written with `--operation=`, matching the manpages; substitute `--command=` if that is what your build reports.

```
# from the build tree produced in §20
cd cmake-build-gcc-shared-release/app/arinc_615a_operation

./arinc_615a_operation
./arinc_615a_operation --operation=Information --help
```

#### A typical session

Discover, then interrogate, then load — each step feeding the next through the targets list.

```
# 1. Discover every target on the subnet, saving what answers
./arinc_615a_operation --operation=Find \
--target-address=192.168.1.255 \
--timeout=5 \
--targets-list=targets.json

# 2. Re-read that list later without touching the network
./arinc_615a_operation --operation=Targets --targets-list=targets.json

# 3. Ask one target what software it is carrying
./arinc_615a_operation --operation=Information \
--targets-list=targets.json \
--target-id=ABCDEF_123

# 4. Upload two loads from a media set
./arinc_615a_operation --operation=Upload \
--targets-list=targets.json \
--target-id=ABCDEF_123 \
--media-set-manager-dir=/srv/media-sets \
--media-set-pn=MS-00042 \
--load-header=LOAD1.LUH \
--load-header=LOAD2.LUH

# 5. Pull files off the target
./arinc_615a_operation --operation=MedDownload \
--targets-list=targets.json \
--target-id=ABCDEF_123 \
--file=FLIGHTLOG.DAT \
--download-base-directory=./downloads
```

#### Addressing a target directly

```
# --target-address instead of a targets list; --target-id is always required
./arinc_615a_operation --operation=Information \
--target-address=192.168.1.50 \
--target-id=ABCDEF_123
```

When both are supplied, a Target ID found in the list **overrides** `--target-address` (§7, step 2). Supplying neither a resolvable list entry nor an address is the one hard error before any traffic.

#### Diagnosing a failing load

```
# Full verbosity plus a binary dump of every protocol file exchanged
./arinc_615a_operation --operation=Information \
--target-address=192.168.1.50 \
--target-id=ABCDEF_123 \
--log-level=trace \
--log-protocol-files

# Dumps land in the working directory as:
# 2026-08-26T14-22-05+0000_Information_TX_ABCDEF_123.LCI
# 2026-08-26T14-22-05+0000_Information_RX_ABCDEF_123.LCL
```

Other levers worth knowing when a target misbehaves:

- `--dlp-timeout=60` — for targets that take a long time between status files. Remember the effective window is `max(exception timer, this)`.
- `--port-option` — only for ARINC 615A-3 and later targets. If the target does not echo the option, the transfer aborts with *"Port option not accepted"*; rerun without it.
- `--block-size-option=512` — narrow the TFTP block size when an intermediate network drops larger datagrams.
- `--local-tftp-address=192.168.1.10` — pin the host TFTP server to one NIC on a multi-homed machine. Without it the server binds `0.0.0.0` and the target may answer to an address you did not intend.
- `--server-port` — only if your target listens somewhere other than the standard port 59. Binding port 59 on the host side needs privilege on Linux.

#### Generating a download request file offline

```
./arinc_615a_download_request_file \
--filename=ABCDEF_123.LNR \
--download-file FLIGHTLOG.DAT MAINTLOG.DAT \
--user-defined-data="bench run 7" \
--arinc615a-version=A4
```
Exit codes do not report protocol outcome

A denied, aborted, or timed-out operation still exits `0` — only command-line parse failures produce a non-zero status (§7). To gate a script on success, match the printed `Final Status Code` line against `OperationCompleted` rather than testing `$?`.

#### Stopping an operation

One Ctrl-C requests a protocol abort: the host waits for the target's next status transfer, sends an ABORT error, and lets the target close out with a final status file. That can take up to the DLP timeout. A second Ctrl-C terminates immediately with a locally synthesised status. See §16.

---

## 22 — Findings

### Defects found while tracing

Five issues, each confirmed by reading the source rather than inferred. Ordered by consequence.

| # | Finding | Location | Effect |
| --- | --- | --- | --- |
| 1 | **Missing `return` after `finished()`** in the "operation completed which was not initiated" branch. Present correctly in the Information variant, absent in the other two. | UploadOperationImpl.cpp:326 DownloadOperationImpl.cpp:162 (cf. InformationOperationImpl.cpp:189) | Control falls through and can reach `finished()` a second time. Because `finished()` has no guard, the command's `std::latch{1}` is counted down twice — undefined behaviour. |
| 2 | **`OperationImpl::finished()` is not idempotent.** No state flag, no early return. | host/implementation/OperationImpl.cpp:159 | The amplifier for #1 and for a rapid double Ctrl-C via `doTerminate()`, which unlike `doAbort()` has no guard. A one-line `if (finishedV) return;` closes all of these. |
| 3 | **Duplicate `-l` short option** — `log-level,l` and `targets-list,l` declared in the same options description. | 18 sites across 9 command files, e.g. InformationOperationCommand.cpp:58 and :80 | `-l` is ambiguous and unusable; only the long forms work. Systemic — every command is affected. |
| 4 | **Missing `return` after refusing a non-write TFTP request.** The error is sent, then control falls into the file-type switch. | InformationOperationImpl.cpp:103–107 | A read request against the host during an Information operation is refused *and* processed, creating a TFTP operation for a transfer already rejected. |
| 5 | **Unguarded completion handler in the FIND query.**`timerHandler()` and `abort()` both invoke it unconditionally. | find/clients/implementation/QueryImpl.cpp:118, 168 | A Ctrl-C arriving after the FIND timer has already fired counts the latch down a second time. Narrow race, same UB as #1. |

##### Documentation defects

- **Command selection flag contradicts itself** — `arinc_615a_operation.adoc` says `-c|--command`, every manpage says `--operation`. One is wrong and users cannot tell which without running the binary.
- **`--dlp-retries` default** — manpage says 2, `DefaultArinc615aDlpRetries` is 1.

##### Observations, not defects

- `timerHandler()` carries `//! @todo cancel active transfers` — in-flight TFTP transfers are not cancelled when the DLP watchdog fires.
- Exceptions caught inside `initialise()` are logged but produce no `finished()` call, so the CLI blocks until the DLP timeout rather than failing fast.
- A list-file checksum mismatch is reported as one word on the console and never affects the exit code.
- `ProtocolFileLogger::loggingDirectory()` is never called by the CLI, so dumps land in the working directory.
- FIND responses are not de-duplicated; a target answering twice appears twice in `targets.json`.

---

## 23 — References

### Sources

##### Standards

- ARINC Report 615A-4 — *Software Data Loader Using Ethernet Interfaces*, 2023-07-24. §6.4 defines the protocol filenames in §14.
- ARINC Report 615A-3, 2007-06-30 — introduces the Port Option and Checksum Option handled in §15.
- ARINC Report 615A-2, 2002-05-10 — renames SNIP to FIND (§8) and permits a declared string length longer than the text (§14).
- ARINC Report 665-5 — *Loadable Software Standards*; the media set format behind §11.
- ARINC Report 649 — check values and part numbering.

##### Primary source paths in this repository

| Path | Contents |
| --- | --- |
| app/arinc_615a_operation/ | CLI entry point, signal handling (§4) |
| lib/arinc_615a_commands/ | Nine command classes, option tables, console output (§5–§13) |
| lib/arinc_615a/host/ | Operation state machines (§9–§13) |
| lib/arinc_615a/find/ | FIND client, packets (§8) |
| lib/arinc_615a/files/ | Protocol file codecs (§14) |
| lib/arinc_615a/tftp/ | ARINC 615A TFTP specialisation (§15) |
| lib/arinc_615a/information/ | Status, part number, and target hardware value types |
| doc/arinc_615a_manpage/ | Per-command manpages — authoritative on options, not on the selection flag |

##### Method and limits

This document was produced by static reading of the working copy. Line numbers reflect that tree. Nothing here was validated against a running target: no build was performed, no traffic was captured, and the behaviour of the three fetched dependencies — `commands`, `tftp`, and `arinc_665` — is described from their call sites in this repository, not from their own source. Where that mattered, notably the command selection flag in §5 and §21, it is called out rather than guessed.

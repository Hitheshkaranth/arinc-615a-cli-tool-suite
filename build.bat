@echo off
setlocal
rem ============================================================================
rem  ARINC 615A Tool Suite - one command: install dependencies, build, run.
rem
rem    build.bat                     install deps, build release, show --help
rem    build.bat -c Find             ...then run with these arguments
rem    build.bat debug -c Find       ...as a debug build
rem    build.bat --no-run            install and build only
rem
rem  First run on a clean machine takes a long time - it builds ~84 vcpkg
rem  packages and libiconv alone can run for hours. Later runs take seconds.
rem ============================================================================

set "REPO=%~dp0"
set "CFG=release"
set "RUN=1"

rem --- first argument may select the configuration --------------------------
if /i "%~1"=="debug"   ( set "CFG=debug"   & shift )
if /i "%~1"=="release" ( set "CFG=release" & shift )
if /i "%~1"=="--no-run" ( set "RUN=0" & shift )

rem --- collect the remaining arguments to forward to the executable ---------
set "ARGS="
:collect
if "%~1"=="" goto collected
set "ARGS=%ARGS% %1"
shift
goto collect
:collected

echo(
echo ==============================================================
echo  ARINC 615A Tool Suite
echo  install dependencies  -^>  build (%CFG%)  -^>  run
echo ==============================================================

call "%REPO%scripts\install-deps.bat" || (
  echo(
  echo Dependency installation failed. See docs\BUILD.md section 6.
  exit /b 1
)

call "%REPO%scripts\build.bat" %CFG% || (
  echo(
  echo Build failed. See docs\BUILD.md section 6.
  exit /b 1
)

set "EXE=%REPO%cmake-build-msvc-static-%CFG%\app\arinc_615a_operation\arinc_615a_operation.exe"
if not exist "%EXE%" (
  echo ERROR: build reported success but %EXE% is missing.
  exit /b 1
)

if "%RUN%"=="0" (
  echo(
  echo Built: %EXE%
  exit /b 0
)

rem --- run it, with the matching vcpkg DLLs on PATH --------------------------
if /i "%CFG%"=="debug" (
  set "PATH=C:\vi\x64-windows\debug\bin;%PATH%"
) else (
  set "PATH=C:\vi\x64-windows\bin;%PATH%"
)

echo(
echo ==============================================================
echo  Running
echo ==============================================================
if "%ARGS%"=="" (
  "%EXE%" --help
) else (
  "%EXE%" %ARGS%
)
set "RC=%ERRORLEVEL%"

echo(
echo --------------------------------------------------------------
echo  Executable: %EXE%
echo  Run again directly with the DLLs on PATH:
if /i "%CFG%"=="debug" (
  echo    set "PATH=C:\vi\x64-windows\debug\bin;%%PATH%%"
) else (
  echo    set "PATH=C:\vi\x64-windows\bin;%%PATH%%"
)
echo    "%EXE%" -c Find
echo --------------------------------------------------------------
exit /b %RC%

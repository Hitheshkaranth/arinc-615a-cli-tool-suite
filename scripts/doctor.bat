@echo off
setlocal EnableDelayedExpansion
rem ============================================================================
rem  ARINC 615A Tool Suite - preflight check (Windows)
rem
rem  Checks every precondition in seconds and tells you the exact fix, so you
rem  never discover a missing prerequisite forty minutes into a vcpkg build.
rem
rem  Usage:  scripts\doctor.bat
rem  Exit:   0 = ready to build, 1 = something needs attention
rem ============================================================================

set "REPO=%~dp0.."
pushd "%REPO%" >nul
set "REPO=%CD%"
set "PROBLEMS=0"
set "WARNINGS=0"

echo(
echo ==============================================================
echo  Preflight check
echo ==============================================================
echo(

call "%~dp0_env.bat" >nul 2>&1
set "ENVRC=%ERRORLEVEL%"

rem --- 1. compiler -------------------------------------------------------------
if defined VSPATH (
  call :ok "C++ toolset" "%VSPATH%"
) else (
  call :bad "C++ toolset" "not found"
  echo        FIX: winget install --id Microsoft.VisualStudio.2022.BuildTools -e ^
--override "--quiet --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
)

rem --- 2. cmake ----------------------------------------------------------------
if defined CMAKE_EXE (
  call :ok "CMake >= 4.3" "%CMAKE_EXE%"
) else (
  call :bad "CMake >= 4.3" "not found, or only the Visual Studio copy (3.31) is present"
  echo        FIX: winget install --id Kitware.CMake -e
)

rem --- 3. ninja ----------------------------------------------------------------
where ninja >nul 2>&1
if errorlevel 1 (
  call :bad "Ninja" "not on PATH"
  echo        FIX: install the "C++ CMake tools for Windows" Visual Studio component
) else (
  for /f "delims=" %%N in ('where ninja') do ( call :ok "Ninja" "%%N" & goto :ninja_done )
)
:ninja_done

rem --- 4. git ------------------------------------------------------------------
where git >nul 2>&1
if errorlevel 1 (
  call :bad "Git" "not on PATH"
  echo        FIX: winget install --id Git.Git -e
) else (
  call :ok "Git" "present"
)

rem --- 5. path length ----------------------------------------------------------
rem  The libiconv long-path failure is caused by these roots being too deep.
call :pathlen "%VCPKG_ROOT%" "vcpkg root"
call :pathlen "%VCPKG_INSTALLED%" "vcpkg installed"

rem --- 6. offline dependency sources -------------------------------------------
set "VENDORED=0"
for %%D in (helper arinc-649 arinc_665 tftp commands) do (
  if exist "%REPO%\%%D\CMakeLists.txt" set /a VENDORED+=1
)
if "%VENDORED%"=="5" (
  call :ok "Dependency sources" "all 5 vendored in-tree - configure works OFFLINE"
) else (
  if "%VENDORED%"=="0" (
    call :info "Dependency sources" "not vendored - configure will clone from git.thomas-vogt.de"
  ) else (
    call :warn "Dependency sources" "%VENDORED% of 5 vendored - the rest will be cloned"
  )
)

rem --- 7. network --------------------------------------------------------------
if not "%VENDORED%"=="5" (
  ping -n 1 -w 3000 git.thomas-vogt.de >nul 2>&1
  if errorlevel 1 (
    call :warn "git.thomas-vogt.de" "not reachable by ping - configure may fail"
    echo        FIX: run scripts\fetch-deps.bat on a connected machine, or check the network
  ) else (
    call :ok "git.thomas-vogt.de" "reachable"
  )
)

rem --- 8. vcpkg binary cache ---------------------------------------------------
set "CACHE=%LOCALAPPDATA%\vcpkg\archives"
if exist "%CACHE%" (
  call :ok "vcpkg binary cache" "%CACHE% - cached packages restore in seconds"
) else (
  call :info "vcpkg binary cache" "empty - the FIRST build compiles ~84 packages; libiconv alone can take hours"
)

rem --- 9. disk space -----------------------------------------------------------
for /f "tokens=1" %%F in ('powershell -NoProfile -Command "[math]::Floor((Get-PSDrive C).Free/1GB)" 2^>nul') do set "FREEGB=%%F"
if defined FREEGB (
  if !FREEGB! LSS 10 (
    call :warn "Disk space on C:" "!FREEGB! GB free - 10 GB recommended for vcpkg build trees"
  ) else (
    call :ok "Disk space on C:" "!FREEGB! GB free"
  )
)

echo(
echo ==============================================================
if "%PROBLEMS%"=="0" (
  if "%WARNINGS%"=="0" (
    echo  READY - run build.bat
  ) else (
    echo  READY with %WARNINGS% warning^(s^) - run build.bat
  )
  echo ==============================================================
  popd & exit /b 0
) else (
  echo  %PROBLEMS% problem^(s^) must be fixed before building
  echo ==============================================================
  popd & exit /b 1
)

rem ---------------------------------------------------------------------------
:ok
echo   [ OK ]   %~1
echo            %~2
exit /b 0
:bad
echo   [FAIL]   %~1
echo            %~2
set /a PROBLEMS+=1
exit /b 0
:warn
echo   [WARN]   %~1
echo            %~2
set /a WARNINGS+=1
exit /b 0
:info
echo   [INFO]   %~1
echo            %~2
exit /b 0
:pathlen
set "P=%~1"
if not defined P ( exit /b 0 )
call :strlen "%P%" LEN
if !LEN! GTR 20 (
  call :warn "%~2 path length" "%P% is !LEN! chars - keep it short; long paths break libiconv"
) else (
  call :ok "%~2 path length" "%P% (!LEN! chars)"
)
exit /b 0
:strlen
set "S=%~1#"
set "L=0"
:strlen_loop
if "!S:~%L%!"=="" goto :strlen_done
set /a L+=1
goto :strlen_loop
:strlen_done
set /a L-=1
set "%~2=%L%"
exit /b 0

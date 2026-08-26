@echo off
setlocal
rem ============================================================================
rem  Vendor the five FetchContent dependencies into the source tree.
rem
rem  Run this ONCE on a machine with network access. Afterwards the CMake
rem  configure step never contacts git.thomas-vogt.de, because the top-level
rem  CMakeLists.txt already prefers an in-tree checkout:
rem
rem      if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/helper )
rem        set( FETCHCONTENT_SOURCE_DIR_HELPER ${CMAKE_SOURCE_DIR}/helper )
rem      endif()
rem
rem  ...and the same for arinc-649, arinc_665, tftp and commands. That hook is
rem  built into the project; this script is just the thing that populates it.
rem
rem  Usage:  scripts\fetch-deps.bat            clone or update all five
rem          scripts\fetch-deps.bat --status   report what is vendored
rem ============================================================================

set "REPO=%~dp0.."
pushd "%REPO%" || exit /b 1
set "REPO=%CD%"
set "BASE=https://git.thomas-vogt.de/thomas-vogt"

if /i "%~1"=="--status" goto :status

echo(
echo ==============================================================
echo  Vendoring dependencies for offline builds
echo ==============================================================
echo  target: %REPO%
echo(

call :get helper     helper.git
call :get arinc-649  arinc-649.git
call :get arinc_665  arinc_665.git
call :get tftp       tftp.git
call :get commands   commands.git

echo(
echo ==============================================================
echo  Done. Configure will now use the in-tree checkouts.
echo  These directories are git-ignored; they are a local cache,
echo  not part of this repository.
echo ==============================================================
popd
exit /b 0

:get
set "DIR=%~1"
set "URL=%BASE%/%~2"
if exist "%REPO%\%DIR%\.git" (
  echo   [update] %DIR%
  git -C "%REPO%\%DIR%" pull --ff-only || echo            WARNING: update failed, keeping existing checkout
) else (
  echo   [clone ] %DIR%
  git clone --depth 1 "%URL%" "%REPO%\%DIR%" || (
    echo            ERROR: clone failed - %URL%
    exit /b 1
  )
)
exit /b 0

:status
echo(
echo  Vendored dependency status
echo  --------------------------
set "N=0"
for %%D in (helper arinc-649 arinc_665 tftp commands) do (
  if exist "%REPO%\%%D\CMakeLists.txt" (
    echo   [ yes ] %%D
    set /a N+=1
  ) else (
    echo   [  no ] %%D
  )
)
echo(
echo  %N% of 5 vendored.  5 of 5 means configure runs fully offline.
popd
exit /b 0

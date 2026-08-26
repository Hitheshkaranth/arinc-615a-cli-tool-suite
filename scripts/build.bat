@echo off
setlocal
rem ============================================================================
rem  ARINC 615A Tool Suite - build script (Windows / MSVC / Ninja)
rem
rem  Configures and builds every target, producing the arinc_615a_operation CLI.
rem
rem  Usage:  scripts\build.bat [debug|release]      (default: release)
rem
rem  Run scripts\install-deps.bat first.
rem ============================================================================

set "CFG=%~1"
if "%CFG%"=="" set "CFG=release"
if /i not "%CFG%"=="debug" if /i not "%CFG%"=="release" (
  echo ERROR: unknown configuration "%CFG%". Use "debug" or "release".
  exit /b 1
)
set "PRESET=msvc-static-%CFG%"

set "REPO=%~dp0.."
pushd "%REPO%" || exit /b 1
set "REPO=%CD%"

echo(
echo ==============================================================
echo  ARINC 615A Tool Suite - build
echo ==============================================================
echo  configuration : %CFG%
echo  preset        : %PRESET%
echo(

call "%~dp0_env.bat" || ( popd & exit /b 1 )
set "BUILDDIR=%REPO%\cmake-build-%PRESET%"
echo  build dir     : %BUILDDIR%
echo(

if not exist "%VCPKG_ROOT%\vcpkg.exe" (
  echo ERROR: vcpkg not found at %VCPKG_ROOT%.
  echo Run scripts\install-deps.bat first.
  popd & exit /b 1
)

rem --- Configure ----------------------------------------------------------------
echo [1/2] Configuring...
rem  CMAKE_COMPILE_WARNING_AS_ERROR is forced OFF: the presets enable /WX together
rem  with /external:templates-, which turns a C4127 inside Boost's own exception
rem  headers into a hard error while building the tftp dependency. See BUILD.md 6.2.
"%CMAKE_EXE%" --preset %PRESET% ^
  -DVCPKG_INSTALLED_DIR=%VCPKG_INSTALLED% ^
  -DCMAKE_COMPILE_WARNING_AS_ERROR=OFF ^
  "-DVCPKG_INSTALL_OPTIONS=--x-buildtrees-root=%VCPKG_BUILDTREES%;--x-packages-root=%VCPKG_PACKAGES%"
if errorlevel 1 ( echo( & echo ==== CONFIGURE FAILED ==== & popd & exit /b 1 )

rem --- Build --------------------------------------------------------------------
echo(
echo [2/2] Building all targets...
"%CMAKE_EXE%" --build "%BUILDDIR%"
if errorlevel 1 ( echo( & echo ==== BUILD FAILED ==== & popd & exit /b 1 )

set "EXE=%BUILDDIR%\app\arinc_615a_operation\arinc_615a_operation.exe"
echo(
echo ==============================================================
echo  BUILD OK
echo ==============================================================
if exist "%EXE%" (
  echo  Executable: %EXE%
  echo(
  echo  NOTE: links against the DYNAMIC x64-windows triplet with
  echo        VCPKG_APPLOCAL_DEPS=OFF, so dependency DLLs are NOT copied next
  echo        to the exe. Put the matching vcpkg bin directory on PATH:
  if /i "%CFG%"=="debug" (
    echo          set "PATH=%VCPKG_INSTALLED%\x64-windows\debug\bin;%%PATH%%"
  ) else (
    echo          set "PATH=%VCPKG_INSTALLED%\x64-windows\bin;%%PATH%%"
  )
  echo(
  echo  Then run:  "%EXE%" --help
) else (
  echo  WARNING: expected executable not found at %EXE%
)
popd
endlocal
exit /b 0

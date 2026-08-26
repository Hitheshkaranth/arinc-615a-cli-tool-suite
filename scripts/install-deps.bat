@echo off
setlocal
rem ============================================================================
rem  ARINC 615A Tool Suite - dependency installer (Windows / MSVC)
rem
rem    1. verifies the Visual Studio C++ build tools, CMake >= 4.3 and Ninja
rem    2. clones + bootstraps vcpkg into a SHORT path
rem    3. installs the vcpkg manifest dependencies (vcpkg.json)
rem
rem  Usage:  scripts\install-deps.bat
rem ============================================================================

set "REPO=%~dp0.."
pushd "%REPO%" || exit /b 1
set "REPO=%CD%"

echo(
echo ==============================================================
echo  ARINC 615A Tool Suite - dependency installer
echo ==============================================================

echo [1/3] Checking toolchain...
call "%~dp0_env.bat" || ( popd & exit /b 1 )
echo   OK: Visual Studio  %VSPATH%
echo   OK: CMake        %CMAKE_EXE%
echo   OK: Ninja
echo(
echo   vcpkg root      : %VCPKG_ROOT%
echo   vcpkg buildtrees: %VCPKG_BUILDTREES%
echo   vcpkg packages  : %VCPKG_PACKAGES%
echo   vcpkg installed : %VCPKG_INSTALLED%
echo(

rem --- vcpkg --------------------------------------------------------------------
echo [2/3] Setting up vcpkg at %VCPKG_ROOT%...
if exist "%VCPKG_ROOT%\.git" (
  echo   Existing vcpkg clone found - reusing it.
) else (
  if exist "%VCPKG_ROOT%" (
    rem  Never delete a directory we did not create.
    echo   ERROR: "%VCPKG_ROOT%" exists but is not a git clone of vcpkg.
    echo   Move or remove it yourself, then re-run this script.
    popd & exit /b 1
  )
  git clone --depth 1 https://github.com/microsoft/vcpkg.git "%VCPKG_ROOT%" || (
    echo   ERROR: git clone failed.
    popd & exit /b 1
  )
)
if not exist "%VCPKG_ROOT%\vcpkg.exe" (
  call "%VCPKG_ROOT%\bootstrap-vcpkg.bat" -disableMetrics || (
    echo   ERROR: vcpkg bootstrap failed.
    popd & exit /b 1
  )
)
echo   OK: vcpkg ready

rem --- manifest dependencies ----------------------------------------------------
echo(
echo [3/3] Installing dependencies from vcpkg.json...
echo   On a cold binary cache this takes a long time; libiconv alone can run for hours.
"%VCPKG_ROOT%\vcpkg.exe" install ^
  --triplet x64-windows ^
  --vcpkg-root "%VCPKG_ROOT%" ^
  --x-manifest-root="%REPO%" ^
  --x-buildtrees-root=%VCPKG_BUILDTREES% ^
  --x-packages-root=%VCPKG_PACKAGES% ^
  --x-install-root=%VCPKG_INSTALLED%
if errorlevel 1 (
  echo(
  echo   ERROR: dependency installation failed. See BUILD.md section 6.
  popd & exit /b 1
)

echo(
echo ==============================================================
echo  Dependencies installed successfully.
echo  Headers/libs: %VCPKG_INSTALLED%\x64-windows
echo  Next step   : scripts\build.bat  [debug^|release]
echo ==============================================================
popd
endlocal
exit /b 0

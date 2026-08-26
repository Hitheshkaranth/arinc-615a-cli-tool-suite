@echo off
rem ============================================================================
rem  Shared environment setup. Called by install-deps.bat and build.bat.
rem  Do NOT use setlocal here - the callers rely on these variables.
rem
rem  Sets: VSPATH, CMAKE_EXE, VCPKG_ROOT, VCPKG_BUILDTREES, VCPKG_PACKAGES,
rem        VCPKG_INSTALLED, and puts Ninja on PATH.
rem  Returns non-zero if a hard requirement is missing.
rem ============================================================================

rem --- Visual Studio C++ toolset ------------------------------------------------
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
  echo   ERROR: vswhere.exe not found - Visual Studio is not installed.
  echo   winget install --id Microsoft.VisualStudio.2022.BuildTools -e --override "--quiet --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
  exit /b 1
)
set "VSPATH="
for /f "usebackq delims=" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSPATH=%%I"
if not defined VSPATH (
  echo   ERROR: no Visual Studio install with the C++ toolset was found.
  echo   winget install --id Microsoft.VisualStudio.2022.BuildTools -e --override "--quiet --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
  exit /b 1
)
if not exist "%VSPATH%\VC\Auxiliary\Build\vcvars64.bat" (
  echo   ERROR: vcvars64.bat missing under "%VSPATH%".
  exit /b 1
)

rem --- Enter the MSVC environment ----------------------------------------------
rem  NOTE: vcvars64.bat OVERWRITES VCPKG_ROOT to the vcpkg bundled with Visual
rem  Studio (under Program Files). Every vcpkg variable must therefore be set
rem  AFTER this call, never before it.
call "%VSPATH%\VC\Auxiliary\Build\vcvars64.bat" >nul || (
  echo   ERROR: vcvars64.bat failed.
  exit /b 1
)

rem --- vcpkg roots. Short on purpose - see BUILD.md section 3. ------------------
set "VCPKG_ROOT=C:\vcpkg"
set "VCPKG_BUILDTREES=C:\vb"
set "VCPKG_PACKAGES=C:\vp"
set "VCPKG_INSTALLED=C:\vi"

rem --- Ninja --------------------------------------------------------------------
where ninja >nul 2>&1
if errorlevel 1 (
  if exist "%VSPATH%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe" (
    set "PATH=%VSPATH%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;%PATH%"
  ) else (
    echo   ERROR: ninja not found. Install the "C++ CMake tools for Windows" VS component.
    exit /b 1
  )
)

rem --- CMake >= 4.3 -------------------------------------------------------------
rem  The VS-bundled CMake is 3.31 and is placed on PATH by vcvars, but every
rem  CMakeLists.txt here declares cmake_minimum_required( VERSION 4.3 ). Search
rem  for a qualifying one instead of taking whatever is first on PATH.
set "CMAKE_EXE="
for /f "delims=" %%C in ('where cmake 2^>nul') do call :try_cmake "%%C"
if not defined CMAKE_EXE call :try_cmake "%ProgramFiles%\CMake\bin\cmake.exe"
if not defined CMAKE_EXE call :try_cmake "%ProgramFiles(x86)%\CMake\bin\cmake.exe"
if not defined CMAKE_EXE (
  echo   CMake ^>= 4.3 not found. Installing via winget...
  winget install --id Kitware.CMake -e --accept-source-agreements --accept-package-agreements
  call :try_cmake "%ProgramFiles%\CMake\bin\cmake.exe"
)
if not defined CMAKE_EXE (
  echo   ERROR: no CMake ^>= 4.3 available. Install it from https://cmake.org/download/ and re-run.
  exit /b 1
)
exit /b 0

rem ---------------------------------------------------------------------------
:try_cmake
if defined CMAKE_EXE exit /b 0
if not exist "%~1" exit /b 0
rem  Run the candidate on its own line and parse from a temp file. Quoting an
rem  executable path that contains spaces INSIDE a for /f command block does not
rem  parse correctly under cmd.exe, which silently yields no version at all.
set "_V="
set "_CMTMP=%TEMP%\_arinc615a_cmake_version.txt"
"%~1" --version > "%_CMTMP%" 2>nul
if errorlevel 1 ( del "%_CMTMP%" 2>nul & exit /b 0 )
for /f "tokens=3" %%V in ('findstr /r /c:"^cmake version" "%_CMTMP%"') do if not defined _V set "_V=%%V"
del "%_CMTMP%" 2>nul
if not defined _V exit /b 0
for /f "tokens=1,2 delims=.-" %%A in ("%_V%") do (
  if %%A GTR 4 ( set "CMAKE_EXE=%~1" & exit /b 0 )
  if %%A EQU 4 if %%B GEQ 3 ( set "CMAKE_EXE=%~1" & exit /b 0 )
)
exit /b 0

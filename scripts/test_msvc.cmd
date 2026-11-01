@echo off
setlocal
set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=Release
set ROOTDIR=%~dp0..
set BUILDDIR=%ROOTDIR%\build\%CONFIG%
set VSVARS=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat

echo [TEST] Initializing MSVC x64 Developer Tools...
call "%VSVARS%"
if errorlevel 1 exit /b 1

if not exist "%BUILDDIR%" (
    echo [TEST] Build directory does not exist. Triggering build first...
    call "%~dp0build_msvc.cmd" %CONFIG%
    if errorlevel 1 exit /b 1
)

echo [TEST] Executing CTest suite...
ctest --test-dir "%BUILDDIR%" --output-on-failure -C %CONFIG% --verbose
if errorlevel 1 (
    echo [ERROR] Test run failed.
    exit /b 1
)

echo [TEST] All automated tests passed.
exit /b 0

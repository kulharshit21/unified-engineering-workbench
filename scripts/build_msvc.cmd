@echo off
setlocal
set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=Release
set ROOTDIR=%~dp0..
set BUILDDIR=%ROOTDIR%\build\%CONFIG%
set QTPATH=C:\Qt\6.8.2\msvc2022_64
set VSVARS=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat

echo [BUILD] Initializing MSVC x64 Developer Tools...
call "%VSVARS%"
if errorlevel 1 (
    echo [ERROR] Failed to initialize MSVC vcvars64.bat
    exit /b 1
)

echo [BUILD] Configuring CMake (%CONFIG%)...
cmake -B "%BUILDDIR%" -S "%ROOTDIR%" -G Ninja -DCMAKE_BUILD_TYPE=%CONFIG% -DCMAKE_PREFIX_PATH="%QTPATH%"
if errorlevel 1 (
    echo [ERROR] CMake configuration failed.
    exit /b 1
)

echo [BUILD] Compiling targets with Ninja...
cmake --build "%BUILDDIR%" --config %CONFIG% -j
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build compilation failed with code %ERRORLEVEL%.
    exit /b %ERRORLEVEL%
)

echo [BUILD] Build completed successfully.
exit /b 0

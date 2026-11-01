@echo off
setlocal
set CANDIDATE_ID=%1
if "%CANDIDATE_ID%"=="" set CANDIDATE_ID=CandidateID
set ROOTDIR=%~dp0..
set BUILDDIR=%ROOTDIR%\build\Release
set DISTDIR=%ROOTDIR%\dist\UnifiedEngineeringWorkbench
set RELEASEDIR=%ROOTDIR%\release
set QTBIN=C:\Qt\6.8.2\msvc2022_64\bin
set WINDEPLOYQT=%QTBIN%\windeployqt.exe
set VSVARS=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat

echo ==========================================================
echo PASS SCIENCE WORKS -- ONE-CLICK PACKAGING PIPELINE
echo Target: %CANDIDATE_ID%_Workbench.zip
echo ==========================================================

rem 1. Build if needed
if not exist "%BUILDDIR%\bin\WorkbenchApp.exe" (
    echo [PACKAGE] Release binary missing. Building first...
    call "%~dp0build_msvc.cmd" Release
    if errorlevel 1 exit /b 1
)

rem 2. Clean and prepare staging
if exist "%DISTDIR%" rmdir /s /q "%DISTDIR%"
if not exist "%DISTDIR%\bin" mkdir "%DISTDIR%\bin"
if not exist "%RELEASEDIR%" mkdir "%RELEASEDIR%"

rem 3. Copy binaries
copy /y "%BUILDDIR%\bin\Launch.exe" "%DISTDIR%\Launch.exe"
copy /y "%BUILDDIR%\bin\WorkbenchApp.exe" "%DISTDIR%\bin\WorkbenchApp.exe"

rem 4. Run windeployqt
echo [PACKAGE] Deploying Qt libraries...
call "%VSVARS%" >nul 2>&1
"%WINDEPLOYQT%" --release --no-translations --no-compiler-runtime --dir "%DISTDIR%\bin" "%DISTDIR%\bin\WorkbenchApp.exe"

rem 5. Copy MSVC Runtime Redistributables
echo [PACKAGE] Deploying MSVC CRT Redistributable DLLs...
set REDIST_DIR=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Redist\MSVC\14.51.36231\x64\Microsoft.VC143.CRT
if exist "%REDIST_DIR%" (
    copy /y "%REDIST_DIR%\msvcp140.dll" "%DISTDIR%\bin\"
    copy /y "%REDIST_DIR%\vcruntime140.dll" "%DISTDIR%\bin\"
    copy /y "%REDIST_DIR%\vcruntime140_1.dll" "%DISTDIR%\bin\"
) else (
    copy /y "C:\Windows\System32\msvcp140.dll" "%DISTDIR%\bin\"
    copy /y "C:\Windows\System32\vcruntime140.dll" "%DISTDIR%\bin\"
)

rem 6. Copy source, documentation, test data, and configurations
echo [PACKAGE] Packaging source tree, test suite, and documentation...
if exist "%ROOTDIR%\data" xcopy /e /i /y "%ROOTDIR%\data" "%DISTDIR%\data"
if exist "%ROOTDIR%\docs" xcopy /e /i /y "%ROOTDIR%\docs" "%DISTDIR%\docs"
if exist "%ROOTDIR%\src" xcopy /e /i /y "%ROOTDIR%\src" "%DISTDIR%\src"
if exist "%ROOTDIR%\tests" xcopy /e /i /y "%ROOTDIR%\tests" "%DISTDIR%\tests"
if exist "%ROOTDIR%\results" xcopy /e /i /y "%ROOTDIR%\results" "%DISTDIR%\results"
if exist "%ROOTDIR%\.vscode" xcopy /e /i /y "%ROOTDIR%\.vscode" "%DISTDIR%\.vscode"
copy /y "%ROOTDIR%\CMakeLists.txt" "%DISTDIR%\"
copy /y "%ROOTDIR%\README.md" "%DISTDIR%\"
copy /y "%ROOTDIR%\LICENSE" "%DISTDIR%\"

rem 6b. Generate Git repository bundle for offline commit history inspection
echo [PACKAGE] Bundling Git commit history...
git bundle create "%DISTDIR%\repository_bundle.bundle" --all >nul 2>&1

rem 7. Compress into ZIP archive via PowerShell
set ZIPNAME=%CANDIDATE_ID%_Workbench.zip
set ZIPPATH=%RELEASEDIR%\%ZIPNAME%
if exist "%ZIPPATH%" del /f /q "%ZIPPATH%"

echo [PACKAGE] Compressing into %ZIPPATH%...
powershell -NoProfile -Command "Compress-Archive -Path '%DISTDIR%\*' -DestinationPath '%ZIPPATH%' -CompressionLevel Optimal"

rem 8. Generate SHA256 checksum
powershell -NoProfile -Command "$h = (Get-FileHash '%ZIPPATH%' -Algorithm SHA256).Hash; Set-Content '%RELEASEDIR%\%CANDIDATE_ID%_Workbench.sha256' -Value \"$h  %ZIPNAME%\""

echo ==========================================================
echo PACKAGE CREATED SUCCESSFULLY: %ZIPPATH%
echo ==========================================================
exit /b 0

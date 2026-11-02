@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "CANDIDATE_ID=%~1"
if not defined CANDIDATE_ID set "CANDIDATE_ID=CandidateID"
set "ROOTDIR=%~dp0.."
set "BUILDDIR=%ROOTDIR%\build\Release"
set "DISTDIR=%ROOTDIR%\dist\UnifiedEngineeringWorkbench"
set "RELEASEDIR=%ROOTDIR%\release"
rem Use the Qt installation injected by GitHub Actions when available.
if not defined QTBIN if defined Qt6_DIR if exist "%Qt6_DIR%\bin\windeployqt.exe" set "QTBIN=%Qt6_DIR%\bin"
if not defined QTBIN if defined Qt6_DIR for %%I in ("%Qt6_DIR%\..\..\..\bin") do if exist "%%~fI\windeployqt.exe" set "QTBIN=%%~fI"
if not defined QTBIN set "QTBIN=C:\Qt\6.8.2\msvc2022_64\bin"
if not defined WINDEPLOYQT set "WINDEPLOYQT=%QTBIN%\windeployqt.exe"
if not defined VSVARS set "VSVARS=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
echo ==========================================================
echo PASS SCIENCE WORKS - VERIFIED RELEASE PACKAGING
echo Candidate archive: %CANDIDATE_ID%_Workbench.zip
echo ==========================================================
if not exist "%WINDEPLOYQT%" (echo [ERROR] windeployqt not found: %WINDEPLOYQT% & exit /b 1)
rem CI's MSVC action already initializes the environment; otherwise invoke vcvars.
if not defined VCToolsRedistDir (
    if not exist "%VSVARS%" (echo [ERROR] MSVC toolchain not initialized; vcvars64.bat missing: %VSVARS% & exit /b 1)
    call "%VSVARS%" >nul
    if errorlevel 1 (echo [ERROR] Failed to initialize Visual C++ environment. & exit /b 1)
)
if not defined VCToolsRedistDir (echo [ERROR] Licensed Visual C++ redist path unavailable. & exit /b 1)
set "CRT_SOURCE="
set "REDIST_BASE=%VCToolsRedistDir%"
if not "%REDIST_BASE:~-1%"=="\" set "REDIST_BASE=%REDIST_BASE%\"
for /d %%D in ("%REDIST_BASE%x64\Microsoft.VC*.CRT") do (
    if exist "%%~fD\msvcp140.dll" if exist "%%~fD\vcruntime140.dll" set "CRT_SOURCE=%%~fD"
)
if not defined CRT_SOURCE (echo [ERROR] Licensed CRT directory not found below %REDIST_BASE%x64. & exit /b 1)
if not exist "%CRT_SOURCE%\vcruntime140.dll" (echo [ERROR] CRT source is missing vcruntime140.dll. & exit /b 1)
echo [PACKAGE] Checking Release targets...
if not exist "%BUILDDIR%\bin\WorkbenchApp.exe" (
    call "%~dp0build_msvc.cmd" Release
    if errorlevel 1 (echo [ERROR] Release build failed. & exit /b 1)
)
if not exist "%BUILDDIR%\bin\Launch.exe" (echo [ERROR] Launch.exe missing from Release build. & exit /b 1)
if not exist "%BUILDDIR%\bin\WorkbenchApp.exe" (echo [ERROR] WorkbenchApp.exe missing from Release build. & exit /b 1)
echo [PACKAGE] Preparing staging directory...
if exist "%DISTDIR%" rmdir /s /q "%DISTDIR%"
if exist "%DISTDIR%" (echo [ERROR] Failed to remove previous staging directory. & exit /b 1)
mkdir "%DISTDIR%\bin"
if errorlevel 1 (echo [ERROR] Failed to create staging directory. & exit /b 1)
if not exist "%RELEASEDIR%" mkdir "%RELEASEDIR%"
if not exist "%RELEASEDIR%" (echo [ERROR] Failed to create release directory. & exit /b 1)
copy /y "%BUILDDIR%\bin\Launch.exe" "%DISTDIR%\Launch.exe" >nul
if errorlevel 1 (echo [ERROR] Failed to stage Launch.exe. & exit /b 1)
copy /y "%BUILDDIR%\bin\WorkbenchApp.exe" "%DISTDIR%\bin\WorkbenchApp.exe" >nul
if errorlevel 1 (echo [ERROR] Failed to stage WorkbenchApp.exe. & exit /b 1)
echo [PACKAGE] Deploying Qt runtime...
"%WINDEPLOYQT%" --release --no-translations --no-compiler-runtime --dir "%DISTDIR%\bin" "%DISTDIR%\bin\WorkbenchApp.exe"
if errorlevel 1 (echo [ERROR] Qt deployment failed. & exit /b 1)
if not exist "%DISTDIR%\bin\Qt6Core.dll" (echo [ERROR] Qt6Core.dll missing after windeployqt. & exit /b 1)
if not exist "%DISTDIR%\bin\platforms\qwindows.dll" (echo [ERROR] qwindows.dll missing after windeployqt. & exit /b 1)
echo [PACKAGE] Deploying licensed Visual C++ runtime from %CRT_SOURCE%...
copy /y "%CRT_SOURCE%\*.dll" "%DISTDIR%\bin\" >nul
if errorlevel 1 (echo [ERROR] CRT deployment failed. & exit /b 1)
if not exist "%DISTDIR%\bin\msvcp140.dll" (echo [ERROR] msvcp140.dll missing. & exit /b 1)
if not exist "%DISTDIR%\bin\vcruntime140.dll" (echo [ERROR] vcruntime140.dll missing. & exit /b 1)
rem Never copy runtime libraries from C:\Windows\System32.
echo [PACKAGE] Copying required source, tests, data, evidence, and editor configuration...
if not exist "%ROOTDIR%\data\" (echo [ERROR] Required source directory missing: data. & exit /b 1)
xcopy /e /i /y "%ROOTDIR%\data" "%DISTDIR%\data" >nul
if errorlevel 1 (echo [ERROR] Failed to copy data. & exit /b 1)
if not exist "%ROOTDIR%\docs\" (echo [ERROR] Required source directory missing: docs. & exit /b 1)
xcopy /e /i /y "%ROOTDIR%\docs" "%DISTDIR%\docs" >nul
if errorlevel 1 (echo [ERROR] Failed to copy docs. & exit /b 1)
if not exist "%ROOTDIR%\src\" (echo [ERROR] Required source directory missing: src. & exit /b 1)
xcopy /e /i /y "%ROOTDIR%\src" "%DISTDIR%\src" >nul
if errorlevel 1 (echo [ERROR] Failed to copy src. & exit /b 1)
if not exist "%ROOTDIR%\tests\" (echo [ERROR] Required source directory missing: tests. & exit /b 1)
xcopy /e /i /y "%ROOTDIR%\tests" "%DISTDIR%\tests" >nul
if errorlevel 1 (echo [ERROR] Failed to copy tests. & exit /b 1)
if not exist "%ROOTDIR%\scripts\" (echo [ERROR] Required source directory missing: scripts. & exit /b 1)
xcopy /e /i /y "%ROOTDIR%\scripts" "%DISTDIR%\scripts" >nul
if errorlevel 1 (echo [ERROR] Failed to copy scripts. & exit /b 1)
if not exist "%ROOTDIR%\results\" (echo [ERROR] Required source directory missing: results. & exit /b 1)
xcopy /e /i /y "%ROOTDIR%\results" "%DISTDIR%\results" >nul
if errorlevel 1 (echo [ERROR] Failed to copy results. & exit /b 1)
if not exist "%ROOTDIR%\.vscode\" (echo [ERROR] Required source directory missing: .vscode. & exit /b 1)
xcopy /e /i /y "%ROOTDIR%\.vscode" "%DISTDIR%\.vscode" >nul
if errorlevel 1 (echo [ERROR] Failed to copy .vscode. & exit /b 1)
if not exist "%ROOTDIR%\CMakeLists.txt" (echo [ERROR] Required file missing: CMakeLists.txt. & exit /b 1)
copy /y "%ROOTDIR%\CMakeLists.txt" "%DISTDIR%\" >nul
if errorlevel 1 (echo [ERROR] Failed to stage CMakeLists.txt. & exit /b 1)
if not exist "%ROOTDIR%\CMakePresets.json" (echo [ERROR] Required file missing: CMakePresets.json. & exit /b 1)
copy /y "%ROOTDIR%\CMakePresets.json" "%DISTDIR%\" >nul
if errorlevel 1 (echo [ERROR] Failed to stage CMakePresets.json. & exit /b 1)
if not exist "%ROOTDIR%\README.md" (echo [ERROR] Required file missing: README.md. & exit /b 1)
copy /y "%ROOTDIR%\README.md" "%DISTDIR%\" >nul
if errorlevel 1 (echo [ERROR] Failed to stage README.md. & exit /b 1)
if not exist "%ROOTDIR%\LICENSE" (echo [ERROR] Required file missing: LICENSE. & exit /b 1)
copy /y "%ROOTDIR%\LICENSE" "%DISTDIR%\" >nul
if errorlevel 1 (echo [ERROR] Failed to stage LICENSE. & exit /b 1)
if not exist "%ROOTDIR%\FINAL_SUBMISSION_GATE.md" (echo [ERROR] Required file missing: FINAL_SUBMISSION_GATE.md. & exit /b 1)
copy /y "%ROOTDIR%\FINAL_SUBMISSION_GATE.md" "%DISTDIR%\" >nul
if errorlevel 1 (echo [ERROR] Failed to stage FINAL_SUBMISSION_GATE.md. & exit /b 1)
echo [PACKAGE] Creating offline Git history bundle...
git -C "%ROOTDIR%" bundle create "%DISTDIR%\repository_bundle.bundle" --all
if errorlevel 1 (echo [ERROR] Git bundle creation failed. & exit /b 1)
if not exist "%DISTDIR%\repository_bundle.bundle" (echo [ERROR] Git bundle is missing. & exit /b 1)
git -C "%ROOTDIR%" bundle verify "%DISTDIR%\repository_bundle.bundle" >nul
if errorlevel 1 (echo [ERROR] Git bundle verification failed. & exit /b 1)
set "ZIPNAME=%CANDIDATE_ID%_Workbench.zip"
set "ZIPPATH=%RELEASEDIR%\%ZIPNAME%"
if exist "%ZIPPATH%" del /f /q "%ZIPPATH%"
if exist "%ZIPPATH%" (echo [ERROR] Cannot replace previous ZIP. & exit /b 1)
echo [PACKAGE] Compressing verified distribution...
powershell.exe -NoProfile -NonInteractive -Command "$ErrorActionPreference='Stop'; Compress-Archive -Path '%DISTDIR%\*' -DestinationPath '%ZIPPATH%' -CompressionLevel Optimal; if (!(Test-Path -LiteralPath '%ZIPPATH%')) { throw 'ZIP was not created' }"
if errorlevel 1 (echo [ERROR] ZIP compression failed. & exit /b 1)
echo [PACKAGE] Computing SHA-256...
python "%ROOTDIR%\scripts\hash_release.py" "%ZIPPATH%" "%RELEASEDIR%\%CANDIDATE_ID%_Workbench.sha256"
if errorlevel 1 (echo [ERROR] SHA-256 generation failed. & exit /b 1)
if not exist "%RELEASEDIR%\%CANDIDATE_ID%_Workbench.sha256" (echo [ERROR] Checksum file missing. & exit /b 1)
echo [PACKAGE] Verifying archive integrity, binaries, and hash...
python "%ROOTDIR%\scripts\verify_extracted_archive.py" "%CANDIDATE_ID%"
if errorlevel 1 (echo [ERROR] Archive acceptance verification failed. & exit /b 1)
echo [PASS] Release archive and SHA-256 verified: %ZIPPATH%
exit /b 0

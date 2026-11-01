@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
echo --- DEPENDENCIES OF Launch.exe ---
dumpbin /dependents "%~dp0..\dist\UnifiedEngineeringWorkbench\Launch.exe" | findstr /i ".dll"
echo.
echo --- DEPENDENCIES OF WorkbenchApp.exe ---
dumpbin /dependents "%~dp0..\dist\UnifiedEngineeringWorkbench\bin\WorkbenchApp.exe" | findstr /i ".dll"

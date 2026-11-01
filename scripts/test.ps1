param(
    [string]$Config = "Release"
)

$ErrorActionPreference = "Stop"

$RootDir = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $RootDir "build\$Config"
$VsVars = "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"

Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host "PASS SCIENCE WORKS — AUTOMATED TEST RUNNER (CTEST)" -ForegroundColor Cyan
Write-Host "==========================================================" -ForegroundColor Cyan

if (-not (Test-Path $BuildDir)) {
    Write-Host "[TEST] Build directory does not exist. Triggering build first..." -ForegroundColor Yellow
    & "$PSScriptRoot\build.ps1" -Config $Config
}

$batPath = Join-Path $BuildDir "run_test.cmd"
$batContent = @"
@echo off
call "$VsVars"
if errorlevel 1 exit /b 1
ctest --test-dir "$BuildDir" --output-on-failure -C $Config --verbose
if errorlevel 1 exit /b 1
"@

Set-Content -Path $batPath -Value $batContent -Encoding ASCII
Write-Host "[TEST] Executing CTest suite..." -ForegroundColor Cyan

& cmd.exe /c $batPath
if ($LASTEXITCODE -ne 0) {
    throw "Test execution failed with exit code $LASTEXITCODE"
}

Write-Host "[TEST] All automated tests passed." -ForegroundColor Green

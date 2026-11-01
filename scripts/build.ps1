param(
    [string]$Config = "Release",
    [switch]$Clean = $false
)

$ErrorActionPreference = "Stop"

$RootDir = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $RootDir "build\$Config"
$QtPath = "C:/Qt/6.8.2/msvc2022_64"
$VsVars = "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"

Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host "PASS SCIENCE WORKS — UNIFIED WORKBENCH BUILD SYSTEM" -ForegroundColor Cyan
Write-Host "Configuration: $Config" -ForegroundColor Cyan
Write-Host "Target:        Windows 11 x64" -ForegroundColor Cyan
Write-Host "==========================================================" -ForegroundColor Cyan

if ($Clean -and (Test-Path $BuildDir)) {
    Write-Host "[BUILD] Cleaning directory: $BuildDir..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $BuildDir
}

if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
}

$batPath = Join-Path $BuildDir "run_build.cmd"
$lines = @(
    "@echo off",
    ("call `"" + $VsVars + "`""),
    "if errorlevel 1 exit /b 1",
    ("cmake -B `"" + $BuildDir + "`" -S `"" + $RootDir + "`" -G Ninja -DCMAKE_BUILD_TYPE=" + $Config + " -DCMAKE_PREFIX_PATH=`"" + $QtPath + "`""),
    "if errorlevel 1 exit /b 1",
    ("cmake --build `"" + $BuildDir + "`" --config " + $Config + " -j"),
    "if errorlevel 1 exit /b 1"
)
Set-Content -Path $batPath -Value $lines -Encoding ASCII

Write-Host "[BUILD] Starting build pipeline..." -ForegroundColor Cyan
& cmd.exe /c $batPath
if ($LASTEXITCODE -ne 0) {
    throw "Build failed with exit code $LASTEXITCODE"
}

Write-Host "[BUILD] Build completed successfully." -ForegroundColor Green

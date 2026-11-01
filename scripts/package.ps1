param(
    [string]$CandidateID = "CandidateID"
)

$ErrorActionPreference = "Stop"

$RootDir = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $RootDir "build\Release"
$DistDir = Join-Path $RootDir "dist\UnifiedEngineeringWorkbench"
$ReleaseDir = Join-Path $RootDir "release"
$QtBin = "C:\Qt\6.8.2\msvc2022_64\bin"
$WinDeployQt = Join-Path $QtBin "windeployqt.exe"
$VsVars = "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"

Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host "PASS SCIENCE WORKS — ONE-CLICK PACKAGING PIPELINE" -ForegroundColor Cyan
Write-Host "Target: $CandidateID`_Workbench.zip" -ForegroundColor Cyan
Write-Host "==========================================================" -ForegroundColor Cyan

# 1. Build project if needed
if (-not (Test-Path "$BuildDir\bin\WorkbenchApp.exe")) {
    Write-Host "[PACKAGE] Building Release targets..." -ForegroundColor Yellow
    & "$PSScriptRoot\build.ps1" -Config "Release"
}

# 2. Prepare staging directory
if (Test-Path $DistDir) {
    Remove-Item -Recurse -Force $DistDir
}
New-Item -ItemType Directory -Force -Path "$DistDir\bin" | Out-Null
New-Item -ItemType Directory -Force -Path $ReleaseDir | Out-Null

# 3. Copy binaries
Copy-Item "$BuildDir\bin\Launch.exe" -Destination "$DistDir\Launch.exe" -Force
Copy-Item "$BuildDir\bin\WorkbenchApp.exe" -Destination "$DistDir\bin\WorkbenchApp.exe" -Force

# 4. Deploy Qt runtime dependencies
Write-Host "[PACKAGE] Running windeployqt on WorkbenchApp.exe..." -ForegroundColor Cyan
$deployBat = Join-Path $DistDir "deploy.cmd"
$deployContent = @"
@echo off
call "$VsVars"
"$WinDeployQt" --release --no-translations --no-compiler-runtime --dir "$DistDir\bin" "$DistDir\bin\WorkbenchApp.exe"
"@
Set-Content -Path $deployBat -Value $deployContent -Encoding ASCII
& cmd.exe /c $deployBat
Remove-Item -Force $deployBat -ErrorAction SilentlyContinue

# 5. Copy MSVC Runtime Redistributable DLLs to bin/
$MsvcRedistDirs = @(
    "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Redist\MSVC",
    "C:\Program Files (x86)\Microsoft Visual Studio\18\Community\VC\Redist\MSVC"
)
$FoundRedist = $false
foreach ($base in $MsvcRedistDirs) {
    if (Test-Path $base) {
        $redistDlls = Get-ChildItem -Path $base -Filter "vcruntime140.dll" -Recurse -Depth 3 -ErrorAction SilentlyContinue
        if ($redistDlls) {
            $redistDir = $redistDlls[0].DirectoryName
            Write-Host "[PACKAGE] Found MSVC Runtime Redistributables in: $redistDir" -ForegroundColor Green
            Copy-Item "$redistDir\vcruntime140.dll" -Destination "$DistDir\bin\" -Force
            Copy-Item "$redistDir\vcruntime140_1.dll" -Destination "$DistDir\bin\" -Force -ErrorAction SilentlyContinue
            Copy-Item "$redistDir\msvcp140.dll" -Destination "$DistDir\bin\" -Force
            $FoundRedist = $true
            break
        }
    }
}
if (-not $FoundRedist) {
    Write-Host "[WARNING] Checking system path fallback for CRT DLLs..." -ForegroundColor Yellow
    Copy-Item "C:\Windows\System32\vcruntime140.dll" -Destination "$DistDir\bin\" -Force -ErrorAction SilentlyContinue
    Copy-Item "C:\Windows\System32\msvcp140.dll" -Destination "$DistDir\bin\" -Force -ErrorAction SilentlyContinue
}

# 6. Copy documentation, data, tests, results, and sources
Write-Host "[PACKAGE] Packaging source, data, tests, docs, and configurations..." -ForegroundColor Cyan
if (Test-Path "$RootDir\data") { Copy-Item -Recurse "$RootDir\data" -Destination "$DistDir\data" -Force }
if (Test-Path "$RootDir\docs") { Copy-Item -Recurse "$RootDir\docs" -Destination "$DistDir\docs" -Force }
if (Test-Path "$RootDir\src") { Copy-Item -Recurse "$RootDir\src" -Destination "$DistDir\src" -Force }
if (Test-Path "$RootDir\tests") { Copy-Item -Recurse "$RootDir\tests" -Destination "$DistDir\tests" -Force }
if (Test-Path "$RootDir\results") { Copy-Item -Recurse "$RootDir\results" -Destination "$DistDir\results" -Force }
if (Test-Path "$RootDir\.vscode") { Copy-Item -Recurse "$RootDir\.vscode" -Destination "$DistDir\.vscode" -Force }
Copy-Item "$RootDir\CMakeLists.txt" -Destination "$DistDir\CMakeLists.txt" -Force
Copy-Item "$RootDir\README.md" -Destination "$DistDir\README.md" -Force -ErrorAction SilentlyContinue
Copy-Item "$RootDir\LICENSE" -Destination "$DistDir\LICENSE" -Force

# 7. Compress into CandidateID_Workbench.zip
$ZipName = "$CandidateID`_Workbench.zip"
$ZipPath = Join-Path $ReleaseDir $ZipName
if (Test-Path $ZipPath) { Remove-Item -Force $ZipPath }

Write-Host "[PACKAGE] Creating ZIP archive: $ZipPath..." -ForegroundColor Cyan
Compress-Archive -Path "$DistDir\*" -DestinationPath $ZipPath -CompressionLevel Optimal

# 8. Compute and record SHA256 checksum
$Hash = (Get-FileHash -Path $ZipPath -Algorithm SHA256).Hash
Set-Content -Path "$ReleaseDir\$CandidateID`_Workbench.sha256" -Value "$Hash  $ZipName"

Write-Host "==========================================================" -ForegroundColor Green
Write-Host "PACKAGE CREATED SUCCESSFULLY:" -ForegroundColor Green
Write-Host "Archive:  $ZipPath" -ForegroundColor Green
Write-Host "Size:     $([math]::Round((Get-Item $ZipPath).Length / 1MB, 2)) MB" -ForegroundColor Green
Write-Host "SHA256:   $Hash" -ForegroundColor Green
Write-Host "==========================================================" -ForegroundColor Green

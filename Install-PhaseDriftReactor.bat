@echo off
REM ============================================================
REM Phase Drift Reactor — One-Click Installer for SMM
REM ============================================================
REM
REM This copies the mod into your SMM development profile so it
REM shows up in the Installed tab. After running, click "Apply
REM Changes" or "Refresh" in SMM to make it appear.
REM
REM Requires: Satisfactory Mod Manager 3.1.1+ installed.
REM Profile:  development
REM
echo.
echo === Phase Drift Reactor — One-Click Installer ===
echo.

set SOURCE="/mnt/c/Users/Jonathan/Documents/My Games/SatisfactoryMods/PhaseDriftReactor/PhaseDriftReactor_v1.0.0.smmod"
set PROFILE="%APPDATA%\SatisfactoryModManager\profiles\development"

if not exist %SOURCE% (
    echo ERROR: source mod not found: %SOURCE%
    echo Re-extract the mod first.
    pause
    exit /b 1
)

if not exist %PROFILE% (
    echo ERROR: SMM development profile not found: %PROFILE%
    echo Make sure you've created the 'development' profile in SMM first.
    pause
    exit /b 1
)

echo Copying mod to profile...
copy /Y %SOURCE% %PROFILE%\PhaseDriftReactor_v1.0.0.smmod >nul
if errorlevel 1 (
    echo ERROR: copy failed.
    pause
    exit /b 1
)

echo Updating manifest...
powershell.exe -NoProfile -Command ^
  "$path = '%PROFILE%\manifest.json';" ^
  "$m = Get-Content $path -Raw | ConvertFrom-Json;" ^
  "$existing = $m.items | Where-Object { $_.id -eq 'PhaseDriftReactor' };" ^
  "if (-not $existing) { $m.items += [pscustomobject]@{ id='PhaseDriftReactor'; enabled=$true }; $m | ConvertTo-Json -Depth 5 | Set-Content $path }" ^
  "else { Write-Host 'Mod already in manifest' }"

echo Updating lock...
powershell.exe -NoProfile -Command ^
  "$lockPath = Get-ChildItem '%PROFILE%\lock-*.json' | Select-Object -First 1;" ^
  "$lock = Get-Content $lockPath.FullName -Raw | ConvertFrom-Json;" ^
  "if (-not $lock.PSObject.Properties['PhaseDriftReactor']) {" ^
  "  $lock | Add-Member -MemberType NoteProperty -Name 'PhaseDriftReactor' -Value ([pscustomobject]@{ version='1.0.0'; dependencies=[pscustomobject]@{ FactoryGame='>=493833.0.0' } });" ^
  "  $lock | ConvertTo-Json -Depth 5 | Set-Content $lockPath.FullName" ^
  "}"

echo.
echo === DONE ===
echo.
echo NEXT STEPS:
echo   1. Open Satisfactory Mod Manager
echo   2. Switch to 'development' profile
echo   3. Click the 'Apply Changes' or refresh button
echo   4. Phase Drift Reactor should appear in the Installed list
echo.
echo If it still does not show up, drag this file onto the SMM window:
echo   %SOURCE%
echo.
pause

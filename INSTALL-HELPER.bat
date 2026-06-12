@echo off
title Phase Drift Reactor — Install Helper
echo.
echo ===========================================================
echo  Phase Drift Reactor — One-step Installer
echo ===========================================================
echo.
echo  Step 1: Opening mod folder...
start "" "explorer.exe" "/mnt/c/Users/Jonathan/Documents/My Games/SatisfactoryMods/PhaseDriftReactor"
echo.
echo  Step 2: Launching SMM (in 3 sec)...
timeout /t 3 /nobreak >nul
start "" "%LOCALAPPDATA%\Programs\Satisfactory Mod Manager\Satisfactory Mod Manager.exe"
echo.
echo  Step 3: In SMM...
echo    a) Switch to 'development' profile
echo    b) Drag the file below onto the SMM window:
echo       /mnt/c/Users/Jonathan/Documents/My Games/SatisfactoryMods/PhaseDriftReactor/PhaseDriftReactor_v1.0.0.smmod
echo.
echo  After SMM accepts the drop, click Apply / Save, then Launch.
echo.
pause

@echo off
title iBurnMgr Install Batch Script
if exist "%~dp0Bin\Launcher.exe" (
copy "%~dp0Bin\Launcher.exe" "%~dp0Launcher.exe" ) else ( echo Error Not Found Lanucher.exe
PAUSE
goto :EOF)
if exist "%~dp0Bin\iBurnMgr.exe" ( echo [Launcher] >%~dp0Launcher.ini
echo LauncherAppPath=%~dp0Bin\iBurnMgr.exe >>%~dp0Launcher.ini ) else (
echo Error Not Found iBurnMgr )
pause
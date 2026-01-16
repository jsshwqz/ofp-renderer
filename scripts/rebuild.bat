@echo off
REM ================================================
REM OFP Renderer - Rebuild Script
REM ================================================

echo Rebuilding OFP Renderer
echo ================================================
echo.

REM Clean first
echo [1/3] Cleaning previous build...
if exist "build" rmdir /s /q build
echo   - Clean complete

REM Build
echo [2/3] Building...
call scripts\build.bat

REM Done
echo [3/3] Rebuild complete!
echo.
pause

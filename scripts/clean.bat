@echo off
REM ================================================
REM OFP Renderer - Clean Build Script
REM ================================================

echo Cleaning OFP Renderer Build
echo ================================================
echo.

REM Clean build directory
if exist "build" (
    echo [1/2] Removing build directory...
    rmdir /s /q build
    echo   - Done
) else (
    echo [1/2] No build directory to clean
)

REM Clean temporary files
echo [2/2] Cleaning temporary files...
del /s /q *.log 2>nul
del /s /q *.tmp 2>nul
del /s /q *.user 2>nul
del /s /q *.suo 2>nul
del /s /q *.opensdf 2>nul
del /s /q *.sdf 2>nul
echo   - Temporary files cleaned

echo.
echo ================================================
echo Build directory is clean!
echo ================================================
pause

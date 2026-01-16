@echo off
REM ================================================
REM OFP Renderer - Local Development Setup Script
REM ================================================

echo OFP Renderer Development Setup
echo ================================================
echo.

REM Check for required tools
echo [1/4] Checking prerequisites...

where git >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Git is not installed!
    echo Please install Git from https://git-scm.com/
    pause
    exit /b 1
)
echo   - Git: OK

where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo WARNING: CMake is not installed (optional for cloud compilation)
) else (
    echo   - CMake: OK
)

echo.
echo [2/4] Initializing submodules...
git submodule update --init --recursive 2>nul
echo   - Submodules updated

echo.
echo [3/4] Configuring build...
if exist "build" (
    echo   - Build directory exists, cleaning...
    rmdir /s /q build 2>nul
)

mkdir build >nul 2>&1
cd build

REM Try to detect Visual Studio
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release 2>nul
if %errorlevel% neq 0 (
    echo WARNING: Visual Studio not detected
    echo Please use GitHub Codespaces for cloud compilation
    cd ..
    rmdir /s /q build 2>nul
) else (
    echo   - CMake configured successfully
)

cd ..

echo.
echo [4/4] Ready for development!
echo ================================================
echo.
echo Next steps:
echo 1. Open this folder in VS Code
echo 2. Use GitHub Codespaces for full development
echo 3. Or push to GitHub for automatic compilation
echo.
echo Quick commands:
echo   - git add .           : Stage changes
echo   - git commit -m "msg"  : Commit changes
echo   - git push            : Push to GitHub
echo.
pause

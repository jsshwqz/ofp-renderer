@echo off
REM ================================================
REM OFP Renderer - Build Script (Windows)
REM ================================================

echo Building OFP Renderer
echo ================================================
echo.

REM Set working directory
cd /d "%~dp0"

REM Create build directory
if not exist "build" mkdir build
cd build

echo [1/3] Configuring with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (
    echo ERROR: CMake configuration failed!
    cd ..
    pause
    exit /b 1
)
echo   - Configuration complete

echo.
echo [2/3] Building...
cmake --build . --config Release --parallel
if %errorlevel% neq 0 (
    echo ERROR: Build failed!
    cd ..
    pause
    exit /b 1
)
echo   - Build complete

echo.
echo [3/3] Verifying output...
if exist "bin\ofp_renderer.dll" (
    echo   - ofp_renderer.dll created successfully!
    echo   - Location: %cd%\bin\ofp_renderer.dll
) else (
    echo WARNING: ofp_renderer.dll not found in bin directory
)

cd ..
echo.
echo ================================================
echo Build complete!
echo ================================================
echo.
echo To test:
echo 1. Copy bin\ofp_renderer.dll to your OFP game directory
echo 2. Run the game
echo 3. Check for any errors in debug output
echo.
pause

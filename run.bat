@echo off
echo Starting Hex Game...
echo.

if not exist "build\HexGame.exe" (
    echo Game not built yet. Building first...
    call build.bat
    exit /b
)

echo Running game...
start build\HexGame.exe
echo.
echo Game launched!

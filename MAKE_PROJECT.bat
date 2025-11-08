@echo off
setlocal

REM ================================
REM Project Build Script
REM Usage:
REM   MAKE_PROJECT.bat [--clean] [--build] [--config <Debug|Release>]
REM Example:
REM   MAKE_PROJECT.bat --clean --build --config Debug
REM ================================

set BUILD_DIR=build
set GENERATOR="Visual Studio 17 2022"
set ARCHITECTURE=x64
set CONFIGURATION=

REM --- Handle arguments ---
set CLEAN=false
set DO_BUILD=false

:parse_args
if "%~1"=="" goto args_done
if /I "%~1"=="--clean" set CLEAN=true
if /I "%~1"=="--build" set DO_BUILD=true
if /I "%~1"=="--config" (
    shift
    if not "%~1"=="" (
        set CONFIGURATION=%~1
    )
)
shift
goto parse_args
:args_done

REM --- Default configuration fallback ---
if "%CONFIGURATION%"=="" (
    set CONFIGURATION=Release
    echo No configuration specified. Defaulting to Release. (Specify using --config {Debug/Release})
)

REM --- Clean build folder if requested ---
if "%CLEAN%"=="true" (
    echo.
    echo Cleaning build folder...
    if exist "%BUILD_DIR%" (
        rmdir /s /q "%BUILD_DIR%"
    )
)

REM --- Create build folder if it doesn't exist ---
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

REM --- Run CMake configuration ---
echo.
echo Configuring CMake project...
cmake -G %GENERATOR% -A %ARCHITECTURE% -S . -B "%BUILD_DIR%"
if errorlevel 1 (
    echo.
    echo [ERROR] CMake configuration failed!
    exit /b 1
)

REM --- Build project if requested ---
if "%DO_BUILD%"=="true" (
    echo.
    echo Building project (%CONFIGURATION%)...
    cmake --build "%BUILD_DIR%" --config %CONFIGURATION% --verbose
    if errorlevel 1 (
        echo.
        echo [ERROR] Build failed!
        exit /b 1
    )
)

echo.
echo Done!
endlocal

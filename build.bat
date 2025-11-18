@echo off
REM Morphological Image Processing - Build Script
REM Supports both sequential and parallel versions

echo ============================================
echo Morphological Image Processing - Build
echo ============================================
echo.

REM Check if we want parallel or sequential
set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=seq

REM Try MinGW first (recommended)
where g++ >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Using MinGW g++ compiler
    goto :mingw_build
)

REM If no MinGW, try MSVC
echo MinGW not found, trying Visual Studio...
goto :msvc_build

:mingw_build
echo.
if "%BUILD_TYPE%"=="seq" (
    echo Building SEQUENTIAL version...
    g++ -std=c++11 -O3 -Wall -o morphology_seq.exe src\main.cpp
    if %ERRORLEVEL% EQU 0 (
        echo.
        echo === BUILD SUCCESSFUL ===
        echo Executable: morphology_seq.exe
        echo.
        echo Example usage:
        echo   morphology_seq.exe input.png output.png erosion 5
    ) else (
        echo === BUILD FAILED ===
    )
) else if "%BUILD_TYPE%"=="par" (
    echo Building PARALLEL version with OpenMP...
    g++ -std=c++11 -O3 -Wall -fopenmp -o morphology_par.exe src\main.cpp
    if %ERRORLEVEL% EQU 0 (
        echo.
        echo === BUILD SUCCESSFUL ===
        echo Executable: morphology_par.exe
        echo.
        echo Example usage:
        echo   set OMP_NUM_THREADS=8
        echo   morphology_par.exe input.png output.png erosion 5
    ) else (
        echo === BUILD FAILED ===
    )
) else (
    echo Unknown build type: %BUILD_TYPE%
    echo Use: build.bat [seq^|par]
)
goto :end

:msvc_build
REM Try different Visual Studio versions and editions
set VSWHERE="C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"

if exist %VSWHERE% (
    for /f "usebackq tokens=*" %%i in (`%VSWHERE% -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set VSINSTALLDIR=%%i
    )
)

if not defined VSINSTALLDIR (
    REM Try manual paths
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
        set VCVARS="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
        set VCVARS="C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
        set VCVARS="C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
        set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
    ) else (
        echo ERROR: No compiler found!
        echo.
        echo Options:
        echo 1. Install MinGW-w64 (RECOMMENDED): https://www.msys2.org/
        echo    Then run: pacman -S mingw-w64-x86_64-gcc
        echo    Add to PATH: C:\msys64\mingw64\bin
        echo.
        echo 2. Install Visual Studio 2019/2022 with C++ tools
        echo    https://visualstudio.microsoft.com/downloads/
        pause
        exit /b 1
    )
) else (
    set VCVARS="%VSINSTALLDIR%\VC\Auxiliary\Build\vcvars64.bat"
)

echo Initializing Visual Studio environment...
call %VCVARS%

if "%BUILD_TYPE%"=="seq" (
    echo Building SEQUENTIAL version...
    cl.exe /EHsc /O2 /Fe:morphology_seq.exe src\main.cpp
    if %ERRORLEVEL% EQU 0 (
        echo.
        echo === BUILD SUCCESSFUL ===
        echo Executable: morphology_seq.exe
    ) else (
        echo === BUILD FAILED ===
    )
) else if "%BUILD_TYPE%"=="par" (
    echo Building PARALLEL version with OpenMP...
    cl.exe /EHsc /O2 /openmp /Fe:morphology_par.exe src\main.cpp
    if %ERRORLEVEL% EQU 0 (
        echo.
        echo === BUILD SUCCESSFUL ===
        echo Executable: morphology_par.exe
    ) else (
        echo === BUILD FAILED ===
    )
)

:end
echo.
pause

@echo off
setlocal EnableDelayedExpansion

echo ============================================
echo   TankBattle - Environment Setup
echo ============================================
echo.
echo   build.bat now auto-installs everything.
echo   This script just checks what's already installed.
echo.

set "FOUND_MSYS2="
set "FOUND_VCPKG="

REM ============================================================
REM  1. Detect MSYS2 UCRT64
REM ============================================================
echo [1/3] Detecting MSYS2 UCRT64...

if defined MSYS2_UCRT64_ROOT (
    if exist "!MSYS2_UCRT64_ROOT!\bin\g++.exe" (
        set "FOUND_MSYS2=!MSYS2_UCRT64_ROOT!"
        echo   OK - from MSYS2_UCRT64_ROOT
        goto :msys2_ok
    )
)

for %%L in (C D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
    if exist "%%L:\msys64\ucrt64\bin\g++.exe" (
        set "FOUND_MSYS2=%%L:\msys64\ucrt64"
        echo   OK - found at %%L:\msys64\ucrt64
        goto :msys2_ok
    )
)

echo   Not found - build.bat will auto-install it.
goto :check_vcpkg

:msys2_ok
set "PATH=!FOUND_MSYS2!\bin;!FOUND_MSYS2!\..\usr\bin;!PATH!"
echo   Compiler: !FOUND_MSYS2!\bin\g++.exe

REM Check Qt6
if exist "!FOUND_MSYS2!\include\qt6\QtCore\QtCore" (
    echo   Qt6: OK
) else (
    echo   Qt6: NOT INSTALLED - build.bat will install it via pacman
)
echo.

:check_vcpkg
REM ============================================================
REM  2. Detect vcpkg
REM ============================================================
echo [2/3] Detecting vcpkg...

if defined VCPKG_ROOT (
    if exist "!VCPKG_ROOT!\scripts\buildsystems\vcpkg.cmake" (
        set "FOUND_VCPKG=!VCPKG_ROOT!"
        echo   OK - from VCPKG_ROOT
        goto :summary
    )
)

for %%D in (
    "%USERPROFILE%\vcpkg"
    "C:\vcpkg" "D:\vcpkg" "E:\vcpkg" "F:\vcpkg" "G:\vcpkg"
    "C:\dev\vcpkg" "D:\dev\vcpkg" "E:\dev\vcpkg"
) do (
    if exist "%%~D\scripts\buildsystems\vcpkg.cmake" (
        set "FOUND_VCPKG=%%~D"
        echo   OK - found at %%~D
        goto :summary
    )
)

echo   Not found - build.bat will auto-install it.
echo.

:summary
REM ============================================================
REM  3. Summary
REM ============================================================
echo [3/3] Summary:
echo.
if defined FOUND_MSYS2 (
    echo   MSYS2 UCRT64 : !FOUND_MSYS2!
) else (
    echo   MSYS2 UCRT64 : *** Will be auto-installed ***
)
if defined FOUND_VCPKG (
    echo   vcpkg        : !FOUND_VCPKG!
) else (
    echo   vcpkg        : *** Will be auto-installed ***
)
echo.
echo   Run build.bat to build and launch the game.
echo.
echo   For VS Code CMake Tools integration, set these env vars:
echo     MSYS2_UCRT64_ROOT=!FOUND_MSYS2!
echo     VCPKG_ROOT=!FOUND_VCPKG!
echo.

endlocal

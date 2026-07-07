@echo off
setlocal EnableDelayedExpansion

echo ============================================
echo   TankBattle - One-Click Build Script
echo ============================================
echo.

:: Set Qt SDK path (relative to project root)
set "QT_DIR=%~dp0qt-sdk"
set "PATH=%QT_DIR%\bin;%PATH%"

:: Auto-detect MSYS2 MinGW compiler
for %%d in (C:\msys64 D:\msys64 E:\msys64) do (
    if exist "%%d\ucrt64\bin\g++.exe" (
        set "PATH=%%d\ucrt64\bin;%%d\usr\bin;!PATH!"
        echo [INFO] Found MSYS2 at %%d
    )
)
:: Fallback: try mingw64
where g++ >nul 2>&1
if %ERRORLEVEL% neq 0 (
    for %%d in (C:\msys64 D:\msys64 E:\msys64) do (
        if exist "%%d\mingw64\bin\g++.exe" (
            set "PATH=%%d\mingw64\bin;%%d\usr\bin;!PATH!"
            echo [INFO] Found MSYS2 at %%d ^(mingw64^)
        )
    )
)

:: Check compiler
where g++ >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [ERROR] g++ compiler not found!
    echo.
    echo Please install MSYS2: https://www.msys2.org/
    echo Then run: pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make
    echo.
    pause
    exit /b 1
)

:: Check make tool
where mingw32-make >nul 2>&1
if %ERRORLEVEL% neq 0 (
    where make >nul 2>&1
    if %ERRORLEVEL% neq 0 (
        echo [ERROR] make tool not found!
        pause
        exit /b 1
    )
    set "MAKE=make"
) else (
    set "MAKE=mingw32-make"
)

echo [INFO] Qt SDK: %QT_DIR%
g++.exe --version 2>&1 | findstr /C:"g++"
echo.

:: Clean old build
echo [1/3] Cleaning old build...
if exist Makefile del /q Makefile 2>nul
if exist Makefile.Debug del /q Makefile.Debug 2>nul
if exist Makefile.Release del /q Makefile.Release 2>nul
if exist debug rmdir /s /q debug 2>nul
if exist release rmdir /s /q release 2>nul

:: Run qmake
echo [2/3] Running qmake...
qmake TankBattle.pro
if %ERRORLEVEL% neq 0 (
    echo [ERROR] qmake failed!
    pause
    exit /b 1
)

:: Compile Release
echo [3/3] Compiling Release build...
!MAKE! -f Makefile.Release
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

echo.
echo ============================================
echo   Build SUCCESS!
echo   Output: release\TankBattle.exe
echo ============================================
echo.
echo Copying runtime DLLs to output...
if not exist release mkdir release
xcopy /Y /Q "%~dp0deploy\*.dll" "release\" >nul 2>&1
xcopy /Y /Q /E "%~dp0deploy\platforms" "release\platforms\" >nul 2>&1
xcopy /Y /Q /E "%~dp0deploy\styles" "release\styles\" >nul 2>&1
xcopy /Y /Q /E "%~dp0deploy\imageformats" "release\imageformats\" >nul 2>&1
xcopy /Y /Q /E "%~dp0deploy\tls" "release\tls\" >nul 2>&1
xcopy /Y /Q /E "%~dp0deploy\generic" "release\generic\" >nul 2>&1
xcopy /Y /Q /E "%~dp0deploy\networkinformation" "release\networkinformation\" >nul 2>&1
copy /Y "%~dp0deploy\qt.conf" "release\" >nul 2>&1

echo Launching game...
start "" "release\TankBattle.exe"

endlocal

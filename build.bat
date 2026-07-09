@echo off
setlocal EnableDelayedExpansion

title TankBattle - Build

echo ============================================
echo   TankBattle - One-Click Build ^& Bootstrap
echo   CMake + vcpkg + MSYS2 MinGW Qt6
echo ============================================
echo.

REM ============================================================
REM  Phase 0: Bootstrap environment
REM ============================================================

REM --- Step 0.1: Locate or install MSYS2 UCRT64 ---
set "MSYS2_UCRT64="

if defined MSYS2_UCRT64_ROOT (
    if exist "!MSYS2_UCRT64_ROOT!\bin\g++.exe" (
        set "MSYS2_UCRT64=!MSYS2_UCRT64_ROOT!"
        echo [INFO] MSYS2 UCRT64 from MSYS2_UCRT64_ROOT = !MSYS2_UCRT64!
        goto :msys2_ready
    )
    echo [WARN] MSYS2_UCRT64_ROOT is set but g++.exe not found, scanning...
)

echo [INFO] Looking for MSYS2 UCRT64...
for %%L in (C D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
    if exist "%%L:\msys64\ucrt64\bin\g++.exe" (
        set "MSYS2_UCRT64=%%L:\msys64\ucrt64"
        echo [INFO] Found at %%L:\msys64\ucrt64
        goto :msys2_ready
    )
)
for %%L in (C D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
    if exist "%%L:\ucrt64\bin\g++.exe" (
        set "MSYS2_UCRT64=%%L:\ucrt64"
        echo [INFO] Found at %%L:\ucrt64
        goto :msys2_ready
    )
)

REM --- MSYS2 not found: download & install ---
echo.
echo ============================================
echo   MSYS2 NOT FOUND - Auto-installing...
echo   This only happens once (~100MB download).
echo ============================================
echo.

set "MSYS2_DIR=C:\msys64"

REM Check if MSYS2 partially installed (pacman exists but no g++)
if exist "!MSYS2_DIR!\usr\bin\pacman.exe" (
    echo [INFO] MSYS2 base found at !MSYS2_DIR!, will install toolchain...
    goto :msys2_install_packages
)

REM Download MSYS2 installer
set "INSTALLER=%TEMP%\msys2-installer.exe"
echo [1/3] Downloading MSYS2 installer...
echo   URL: https://github.com/msys2/msys2-installer/releases/latest/download/msys2-x86_64.exe

REM Try curl first (built into Win10+), fall back to PowerShell
where curl >nul 2>&1
if !ERRORLEVEL! equ 0 (
    curl -L --progress-bar -o "!INSTALLER!" "https://github.com/msys2/msys2-installer/releases/latest/download/msys2-x86_64.exe"
    set "DL_OK=!ERRORLEVEL!"
) else (
    echo   curl not found, using PowerShell...
    powershell -Command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri 'https://github.com/msys2/msys2-installer/releases/latest/download/msys2-x86_64.exe' -OutFile '!INSTALLER!'"
    set "DL_OK=!ERRORLEVEL!"
)

if !DL_OK! neq 0 (
    echo.
    echo [ERROR] Failed to download MSYS2 installer.
    echo Please download manually from: https://www.msys2.org/
    echo Then run this script again. MSYS2 should be installed to C:\msys64
    pause
    exit /b 1
)
echo   Download complete.

REM Install MSYS2 silently
echo [2/3] Installing MSYS2 to !MSYS2_DIR!...
echo   (this may take a minute, please wait...)
"!INSTALLER!" /VERYSILENT /SUPPRESSMSGBOXES /NORESTART /D=C:\msys64

REM Wait for installer to finish (InnoSetup /VERYSILENT should be synchronous)
REM Verify installation
set "WAIT=0"
:wait_msys2
if not exist "!MSYS2_DIR!\usr\bin\pacman.exe" (
    timeout /t 2 /nobreak >nul
    set /a "WAIT+=2"
    if !WAIT! lss 60 goto :wait_msys2
    echo [ERROR] MSYS2 installation did not complete correctly.
    echo Expected: !MSYS2_DIR!\usr\bin\pacman.exe
    pause
    exit /b 1
)
echo   MSYS2 installed successfully.

REM Clean up installer
del "!INSTALLER!" 2>nul

:msys2_install_packages
REM Initialize pacman keyring (pacman-key is a bash script, must run via bash)
echo [3/3] Initializing package manager...
set "PATH=!MSYS2_DIR!\usr\bin;!PATH!"

REM First-time keyring init - use bash -l for proper MSYS2 environment
"!MSYS2_DIR!\usr\bin\bash.exe" -lc "pacman-key --init" 2>nul
if !ERRORLEVEL! neq 0 (
    echo [WARN] pacman-key --init failed, trying to continue...
)
"!MSYS2_DIR!\usr\bin\bash.exe" -lc "pacman-key --populate msys2" 2>nul
if !ERRORLEVEL! neq 0 (
    echo [WARN] pacman-key --populate failed, may need network. Trying to continue...
)

echo   Installing toolchain packages (~200MB, this may take a while)...
echo   Packages: gcc cmake git qt6-base
echo.

REM Update package database first
"!MSYS2_DIR!\usr\bin\pacman" -Sy --noconfirm 2>nul

REM Install required packages
"!MSYS2_DIR!\usr\bin\pacman" -S --noconfirm --needed ^
    mingw-w64-ucrt-x86_64-gcc ^
    mingw-w64-ucrt-x86_64-cmake ^
    mingw-w64-ucrt-x86_64-qt6-base ^
    git
if !ERRORLEVEL! neq 0 (
    echo.
    echo [ERROR] Package installation failed.
    echo You can try manually in an MSYS2 UCRT64 terminal:
    echo   pacman -S --noconfirm mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-qt6-base git
    pause
    exit /b 1
)

REM Verify g++ exists now
if not exist "!MSYS2_DIR!\ucrt64\bin\g++.exe" (
    echo [ERROR] Toolchain installation succeeded but g++.exe not found.
    echo Expected: !MSYS2_DIR!\ucrt64\bin\g++.exe
    pause
    exit /b 1
)
set "MSYS2_UCRT64=!MSYS2_DIR!\ucrt64"
echo.
echo   Toolchain ready.
echo.

:msys2_ready
REM Add MSYS2 to PATH
set "PATH=!MSYS2_UCRT64!\bin;!MSYS2_UCRT64!\..\usr\bin;!PATH!"

REM Verify g++
where g++ >nul 2>&1
if !ERRORLEVEL! neq 0 (
    echo [ERROR] g++ not found in PATH!
    pause
    exit /b 1
)

REM Verify cmake
where cmake >nul 2>&1
if !ERRORLEVEL! neq 0 (
    echo [ERROR] cmake not found! Install: pacman -S mingw-w64-ucrt-x86_64-cmake
    pause
    exit /b 1
)

echo [INFO] MSYS2 UCRT64:  !MSYS2_UCRT64!
echo [INFO] Compiler:      !MSYS2_UCRT64!\bin\g++.exe
g++.exe --version 2>&1 | findstr /C:"g++"
echo.

REM ============================================================
REM  Step 0.2: Locate or install vcpkg
REM ============================================================
set "VCPKG_ROOT_DETECTED="

if defined VCPKG_ROOT (
    if exist "!VCPKG_ROOT!\scripts\buildsystems\vcpkg.cmake" (
        set "VCPKG_ROOT_DETECTED=!VCPKG_ROOT!"
        echo [INFO] vcpkg from VCPKG_ROOT = !VCPKG_ROOT!
        goto :vcpkg_ready
    )
    echo [WARN] VCPKG_ROOT is set but vcpkg.cmake not found, scanning...
)

echo [INFO] Looking for vcpkg...

REM Check common locations
for %%D in (
    "%USERPROFILE%\vcpkg"
    "C:\vcpkg" "D:\vcpkg" "E:\vcpkg" "F:\vcpkg" "G:\vcpkg"
    "C:\dev\vcpkg" "D:\dev\vcpkg" "E:\dev\vcpkg"
) do (
    if exist "%%~D\scripts\buildsystems\vcpkg.cmake" (
        set "VCPKG_ROOT_DETECTED=%%~D"
        echo [INFO] Found at %%~D
        goto :vcpkg_ready
    )
)

REM Scan all drives
for %%L in (C D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
    if exist "%%L:\vcpkg\scripts\buildsystems\vcpkg.cmake" (
        set "VCPKG_ROOT_DETECTED=%%L:\vcpkg"
        echo [INFO] Found at %%L:\vcpkg
        goto :vcpkg_ready
    )
)

REM --- vcpkg not found: git clone & bootstrap ---
echo.
echo ============================================
echo   vcpkg NOT FOUND - Auto-installing...
echo   This only happens once (~200MB clone).
echo ============================================
echo.

set "VCPKG_DIR=C:\vcpkg"

REM Check if git is available (MSYS2 provides it)
set "GIT_CMD="
if exist "!MSYS2_UCRT64!\..\usr\bin\git.exe" (
    set "GIT_CMD=!MSYS2_UCRT64!\..\usr\bin\git.exe"
) else (
    where git >nul 2>&1
    if !ERRORLEVEL! equ 0 set "GIT_CMD=git"
)

if "!GIT_CMD!"=="" (
    echo [ERROR] git not found. It should have been installed with MSYS2.
    echo Please install manually: pacman -S git
    pause
    exit /b 1
)

REM Clone vcpkg
if exist "!VCPKG_DIR!\.git" (
    echo [INFO] vcpkg repo already exists at !VCPKG_DIR!, updating...
    pushd "!VCPKG_DIR!"
    "!GIT_CMD!" pull --ff-only
    popd
) else (
    echo [1/2] Cloning vcpkg to !VCPKG_DIR!...
    "!GIT_CMD!" clone --depth 1 https://github.com/microsoft/vcpkg.git "!VCPKG_DIR!"
    if !ERRORLEVEL! neq 0 (
        echo.
        echo [ERROR] Failed to clone vcpkg.
        echo Please clone manually:
        echo   git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
        echo   cd C:\vcpkg ^&^& bootstrap-vcpkg.bat
        pause
        exit /b 1
    )
)

REM Bootstrap vcpkg
echo [2/2] Bootstrapping vcpkg...
pushd "!VCPKG_DIR!"
call bootstrap-vcpkg.bat
if !ERRORLEVEL! neq 0 (
    popd
    echo [ERROR] vcpkg bootstrap failed.
    pause
    exit /b 1
)
popd

REM Verify
if not exist "!VCPKG_DIR!\scripts\buildsystems\vcpkg.cmake" (
    echo [ERROR] vcpkg bootstrap completed but vcpkg.cmake not found.
    pause
    exit /b 1
)
set "VCPKG_ROOT_DETECTED=!VCPKG_DIR!"
echo.
echo   vcpkg ready.
echo.

:vcpkg_ready
echo [INFO] vcpkg: !VCPKG_ROOT_DETECTED!
echo.

REM ============================================================
REM  Step 0.3: Generate vcpkg-configuration.json
REM ============================================================
set "VCPKG_URI=!VCPKG_ROOT_DETECTED:\=/!"
echo [INFO] Generating vcpkg-configuration.json...
(
    echo {
    echo   "default-registry": {
    echo     "kind": "git",
    echo     "repository": "file:///!VCPKG_URI!",
    echo     "baseline": "cae2989df743645fb2510971b7a45febd9a2c361"
    echo   },
    echo   "overlay-ports": [
    echo     "./vcpkg-overlays/qtbase"
    echo   ]
    echo }
) > "vcpkg-configuration.json"

REM ============================================================
REM  Phase 1: Build
REM ============================================================
echo [1/2] Configuring CMake (vcpkg manifest mode)...
if exist build rmdir /s /q build 2>nul

cmake -B build ^
    -G "MinGW Makefiles" ^
    -DCMAKE_TOOLCHAIN_FILE="!VCPKG_ROOT_DETECTED!\scripts\buildsystems\vcpkg.cmake" ^
    -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_CXX_COMPILER="!MSYS2_UCRT64!\bin\g++.exe"

if !ERRORLEVEL! neq 0 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b 1
)

echo [2/2] Building Release...
cmake --build build --config Release
if !ERRORLEVEL! neq 0 (
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

echo.
echo ============================================
echo   Build SUCCESS!
echo   Output: build\TankBattle.exe
echo ============================================
echo.

REM ============================================================
REM  Deploy Qt plugins + qt.conf
REM ============================================================
echo Deploying Qt plugins...
set "VCPKG_INSTALLED=build\vcpkg_installed\x64-mingw-dynamic"
if exist "!VCPKG_INSTALLED!\plugins" (
    xcopy /Y /Q /E "!VCPKG_INSTALLED!\plugins\*" "build\" >nul 2>&1
)

(
    echo [Paths]
    echo Plugins = .
) > "build\qt.conf"

REM ============================================================
REM  Launch
REM ============================================================
echo Launching game...
start "" "build\TankBattle.exe"

endlocal

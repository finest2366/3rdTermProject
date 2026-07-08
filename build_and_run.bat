@echo off
REM ============================================
REM  TankGameMerged — Windows 构建脚本
REM  使用 Qt 自带的 CMake + MinGW，无需 vcpkg
REM ============================================

set "PATH=C:\msys64\ucrt64\bin;%PATH%"

echo.
echo === 清理旧构建 ===
if exist build rmdir /s /q build

echo.
echo === 配置 CMake (Debug) ===
cmake -B build -G "MinGW Makefiles" ^
    -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe ^
    -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe ^
    -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/mingw32-make.exe ^
    -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64 ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DBUILD_TESTS=OFF

if %ERRORLEVEL% NEQ 0 (
    echo === 配置失败! ===
    pause
    exit /b 1
)

echo.
echo === 编译中... ===
cmake --build build

if %ERRORLEVEL% NEQ 0 (
    echo === 编译失败! ===
    pause
    exit /b 1
)

echo.
echo === 构建成功! 运行游戏: ===
start build\TankGameMerged.exe

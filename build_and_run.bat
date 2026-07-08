@echo off
REM ============================================
REM  TankGameMerged — Windows 构建脚本
REM  使用 Qt 自带的 CMake + MinGW，无需 vcpkg
REM ============================================

set "PATH=C:\Qt\Tools\CMake_64\bin;C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.11.1\mingw_64\bin;%PATH%"

echo.
echo === 清理旧构建 ===
if exist build rmdir /s /q build

echo.
echo === 配置 CMake (Debug) ===
cmake -B build -G "MinGW Makefiles" ^
    -DCMAKE_C_COMPILER=C:/Qt/Tools/mingw1310_64/bin/gcc.exe ^
    -DCMAKE_CXX_COMPILER=C:/Qt/Tools/mingw1310_64/bin/g++.exe ^
    -DCMAKE_MAKE_PROGRAM=C:/Qt/Tools/mingw1310_64/bin/mingw32-make.exe ^
    -DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/mingw_64 ^
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

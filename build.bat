@echo off
chcp 65001 >nul
echo ============================================
echo   坦克大战 - 一键编译脚本
echo ============================================
echo.

:: 设置Qt SDK路径
set "QT_DIR=%~dp0qt-sdk"
set "PATH=%QT_DIR%\bin;%PATH%"

:: 检查编译器
where g++ >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [错误] 未找到 g++ 编译器!
    echo 请确保 MinGW-w64 (ucrt64) 已安装并添加到 PATH 环境变量中.
    echo.
    echo 推荐安装方式:
    echo   1. MSYS2: https://www.msys2.org/
    echo      pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make
    echo   2. 或 winlibs: https://winlibs.com/
    echo.
    pause
    exit /b 1
)

where mingw32-make >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [提示] 未找到 mingw32-make, 尝试使用 make...
    set "MAKE=make"
    where make >nul 2>&1
    if %ERRORLEVEL% neq 0 (
        echo [错误] 未找到 make 工具!
        pause
        exit /b 1
    )
) else (
    set "MAKE=mingw32-make"
)

echo [信息] Qt SDK: %QT_DIR%
echo [信息] 编译器:
g++ --version | findstr /C:"g++"
echo.

:: 清理旧的编译产物
echo [1/3] 清理旧构建...
if exist Makefile del /q Makefile
if exist Makefile.Debug del /q Makefile.Debug
if exist Makefile.Release del /q Makefile.Release
if exist debug rmdir /s /q debug
if exist release rmdir /s /q release

:: 运行 qmake
echo [2/3] 运行 qmake ...
qmake TankBattle.pro
if %ERRORLEVEL% neq 0 (
    echo [错误] qmake 失败!
    pause
    exit /b 1
)

:: 编译 Release 版本
echo [3/3] 编译 Release 版本 ...
%MAKE% -f Makefile.Release
if %ERRORLEVEL% neq 0 (
    echo [错误] 编译失败!
    pause
    exit /b 1
)

echo.
echo ============================================
echo   编译成功!
echo   输出: release\TankBattle.exe
echo ============================================
echo.
echo 复制运行时DLL到输出目录...
if not exist release mkdir release
xcopy /Y /Q "%~dp0deploy\*.dll" "release\" >nul 2>&1
xcopy /Y /Q /E "%~dp0deploy\platforms" "release\platforms\" >nul 2>&1
xcopy /Y /Q /E "%~dp0deploy\styles" "release\styles\" >nul 2>&1
xcopy /Y /Q /E "%~dp0deploy\imageformats" "release\imageformats\" >nul 2>&1
xcopy /Y /Q /E "%~dp0deploy\tls" "release\tls\" >nul 2>&1
xcopy /Y /Q /E "%~dp0deploy\generic" "release\generic\" >nul 2>&1
xcopy /Y /Q /E "%~dp0deploy\networkinformation" "release\networkinformation\" >nul 2>&1
copy /Y "%~dp0deploy\qt.conf" "release\" >nul 2>&1

echo.
echo 运行游戏...
start "" "release\TankBattle.exe"

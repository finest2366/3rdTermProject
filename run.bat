@echo off
:: 坦克大战 - 直接运行
:: 所有依赖DLL已在 deploy/ 文件夹中
cd /d "%~dp0deploy"
start "" TankBattle.exe

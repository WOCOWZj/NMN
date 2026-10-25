@echo off
rem 双击即可：增量编译后启动 NMN
rem 想跳过编译，直接运行 build\NMN.exe 即可
setlocal
cd /d "%~dp0"

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\run.ps1"
if errorlevel 1 (
    echo.
    echo 启动失败，按任意键关闭窗口...
    pause >nul
)

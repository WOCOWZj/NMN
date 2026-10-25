<#
.SYNOPSIS
    快速启动 NMN 用于测试。

.DESCRIPTION
    默认先做一次增量编译（没改动时代价极小），然后启动程序。
    工作目录固定为仓库根目录，这样 test.jpg 与 assets/ 都能被找到。

.PARAMETER NoBuild
    跳过编译，直接运行已有的 exe。

.PARAMETER Image
    启动时自动加载指定图片（覆盖默认的 test.jpg）。

.PARAMETER Wait
    阻塞等待程序退出，便于在终端里看日志与退出码。
    双击启动时不要加这个参数。

.PARAMETER DebugSeg
    开启切分调试图，运行后把结果框画到 assets/seg_debug.png。

.EXAMPLE
    .\scripts\run.ps1
    .\scripts\run.ps1 -NoBuild
    .\scripts\run.ps1 -Image .\test.jpg -DebugSeg -Wait
#>
[CmdletBinding()]
param(
    [switch]$NoBuild,
    [string]$Image,
    [switch]$Wait,
    [switch]$DebugSeg
)

$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $PSScriptRoot
$buildDir = Join-Path $root 'build'
$exe = Join-Path $buildDir 'NMN.exe'

if (-not $NoBuild) {
    & (Join-Path $PSScriptRoot 'build.ps1')
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}

if (-not (Test-Path $exe)) {
    Write-Host "[x] 找不到 $exe，先运行 scripts\build.ps1" -ForegroundColor Red
    exit 1
}

# 运行期依赖：Qt 与 Tesseract 的 DLL 目录
$qtDir = if ($env:NMN_QT_DIR) { $env:NMN_QT_DIR } else { 'D:/Qt/6.9.3/mingw_64' }
$mingwBin = if ($env:NMN_MINGW_DIR) { $env:NMN_MINGW_DIR } else { 'D:\Qt\Tools\mingw1310_64\bin' }
$env:PATH = "$buildDir;$qtDir\bin;$mingwBin;$env:PATH"

if ($DebugSeg) { $env:NMN_DEBUG_SEG = '1' } else { Remove-Item Env:\NMN_DEBUG_SEG -ErrorAction SilentlyContinue }

# 自动识别测试图：把指定图片复制成工作目录下的 test.jpg
if ($Image) {
    $resolved = Resolve-Path $Image -ErrorAction Stop
    $target = Join-Path $root 'test.jpg'
    if ((Resolve-Path $resolved).Path -ne (Join-Path $root 'test.jpg')) {
        Copy-Item $resolved.Path $target -Force
        Write-Host "[*] 已把 $($resolved.Path) 设为 test.jpg" -ForegroundColor Cyan
    }
}

Write-Host "[*] 启动 NMN" -ForegroundColor Cyan
$proc = Start-Process -FilePath $exe -WorkingDirectory $root -PassThru

if ($Wait) {
    Write-Host "    PID $($proc.Id)，等待退出…" -ForegroundColor DarkGray
    $proc.WaitForExit()
    Write-Host "[√] 退出码 $($proc.ExitCode)" -ForegroundColor Green
}
else {
    Write-Host "[√] 已启动，PID $($proc.Id)" -ForegroundColor Green
}

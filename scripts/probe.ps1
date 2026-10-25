<#
.SYNOPSIS
    运行视觉识别探针，对照人工标注算准确率。

.DESCRIPTION
    不开 GUI，直接用命令行跑同一套视觉组件。
    改动切分参数或模板后，用它快速验证效果。

.PARAMETER Image
    待测图片，默认仓库根目录的 test.jpg。

.EXAMPLE
    .\scripts\probe.ps1
    .\scripts\probe.ps1 -Image .\samples\another.jpg
#>
[CmdletBinding()]
param(
    [string]$Image
)

$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $PSScriptRoot
$buildDir = Join-Path $root 'build'

$qtDir    = if ($env:NMN_QT_DIR)    { $env:NMN_QT_DIR }    else { 'D:/Qt/6.9.3/mingw_64' }
$mingwBin = if ($env:NMN_MINGW_DIR) { $env:NMN_MINGW_DIR } else { 'D:\Qt\Tools\mingw1310_64\bin' }
$ninjaDir = if ($env:NMN_NINJA_DIR) { $env:NMN_NINJA_DIR } else { 'D:\Qt\Tools\Ninja' }
$env:PATH = "$mingwBin;$ninjaDir;$qtDir\bin;$env:PATH"

if (-not (Test-Path (Join-Path $buildDir 'build.ninja'))) {
    Write-Host '[*] 尚未配置，先跑一次 build.ps1' -ForegroundColor Yellow
    & (Join-Path $PSScriptRoot 'build.ps1')
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}

Write-Host '[*] 编译探针' -ForegroundColor Cyan
& cmake --build $buildDir --parallel --target NMN_probe
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$probe = Join-Path $buildDir 'NMN_probe.exe'
if (-not (Test-Path $probe)) {
    Write-Host "[x] 没生成 $probe" -ForegroundColor Red
    exit 1
}

if (-not $Image) { $Image = Join-Path $root 'test.jpg' }
if (-not (Test-Path $Image)) {
    Write-Host "[x] 找不到图片: $Image" -ForegroundColor Red
    exit 1
}

$templates = Join-Path $root 'resources\templates'
$env:PATH = "$buildDir;$env:PATH"

Write-Host ''
& $probe $Image $templates

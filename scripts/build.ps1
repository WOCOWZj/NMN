<#
.SYNOPSIS
    配置并编译 NMN。

.DESCRIPTION
    默认输出到 <repo>/build，使用 Ninja + MinGW。
    工具链路径可用环境变量覆盖，不必改脚本：
        NMN_QT_DIR        Qt6 安装前缀
        NMN_MINGW_DIR     MinGW 的 bin 目录
        NMN_NINJA_DIR     ninja.exe 所在目录
        NMN_OPENCV_DIR    OpenCV 的 CMake 配置目录
        NMN_TESSERACT_DIR Tesseract 运行时目录

.PARAMETER Config
    Release（默认）或 Debug。

.PARAMETER Clean
    先删除 build 目录再配置。

.PARAMETER Deploy
    构建后执行 windeployqt，把 Qt 运行库拷到输出目录，
    这样 exe 可以脱离 Qt 的 bin 目录独立运行。

.EXAMPLE
    .\scripts\build.ps1
    .\scripts\build.ps1 -Config Debug
    .\scripts\build.ps1 -Clean -Deploy
#>
[CmdletBinding()]
param(
    [ValidateSet('Release', 'Debug')]
    [string]$Config = 'Release',

    [switch]$Clean,

    [switch]$Deploy
)

$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $PSScriptRoot
$buildDir = Join-Path $root 'build'

# ---- 工具链定位（环境变量优先）---------------------------------------
function Resolve-Tool([string]$envName, [string]$fallback) {
    $value = [Environment]::GetEnvironmentVariable($envName)
    if ([string]::IsNullOrWhiteSpace($value)) { $value = $fallback }
    return $value
}

$qtDir    = Resolve-Tool 'NMN_QT_DIR'        'D:/Qt/6.9.3/mingw_64'
$mingwBin = Resolve-Tool 'NMN_MINGW_DIR'     'D:\Qt\Tools\mingw1310_64\bin'
$ninjaDir = Resolve-Tool 'NMN_NINJA_DIR'     'D:\Qt\Tools\Ninja'
$opencvDir = Resolve-Tool 'NMN_OPENCV_DIR'   'D:/opencv/opencv/build/x64/mingw/lib'
$tessDir  = Resolve-Tool 'NMN_TESSERACT_DIR' 'D:/Tesseract-OCR'

# ---- 前置检查 --------------------------------------------------------
$problems = @()
if (-not (Test-Path $qtDir))       { $problems += "Qt 不存在: $qtDir" }
if (-not (Test-Path $mingwBin))    { $problems += "MinGW 不存在: $mingwBin" }
if (-not (Test-Path $opencvDir))   { $problems += "OpenCV 不存在: $opencvDir" }
if (-not (Test-Path $tessDir))     { $problems += "Tesseract 不存在: $tessDir" }
if ($problems.Count -gt 0) {
    Write-Host '[x] 工具链缺失:' -ForegroundColor Red
    $problems | ForEach-Object { Write-Host "    $_" -ForegroundColor Red }
    Write-Host '    可用环境变量覆盖，见脚本头部说明。' -ForegroundColor DarkGray
    exit 1
}

$env:PATH = "$mingwBin;$ninjaDir;$qtDir\bin;$env:PATH"

# 构建目录若来自另一份源码（CMakeCache 里记录的路径与实际不符），
# 直接配置会报 "does not match the source" 而失败，这里检测并自动清理。
$cacheFile = Join-Path $buildDir 'CMakeCache.txt'
if ((Test-Path $cacheFile) -and -not $Clean) {
    $cachedSource = Select-String -Path $cacheFile -Pattern '^CMAKE_HOME_DIRECTORY:INTERNAL=(.+)$' |
        ForEach-Object { $_.Matches[0].Groups[1].Value.Trim() } |
        Select-Object -First 1

    if ($cachedSource) {
        $a = [System.IO.Path]::GetFullPath($cachedSource).TrimEnd('\', '/')
        $b = [System.IO.Path]::GetFullPath($root).TrimEnd('\', '/')
        if ($a -ne $b) {
            Write-Host '[!] 构建目录来自另一份源码，自动清理' -ForegroundColor Yellow
            Write-Host "    $cachedSource" -ForegroundColor DarkGray
            Remove-Item $buildDir -Recurse -Force
        }
    }
}

if ($Clean -and (Test-Path $buildDir)) {
    Write-Host "[*] 清理 $buildDir" -ForegroundColor Yellow
    Remove-Item $buildDir -Recurse -Force
}

# ---- 配置 ------------------------------------------------------------
Write-Host "[*] 配置 ($Config)" -ForegroundColor Cyan

# 参数逐个写成带引号的字符串：裸写 "-DFOO=$var" 在 PowerShell 里
# 变量可能不被展开，CMake 会收到字面量 $Config 从而生成坏掉的 ninja 规则。
$configureArgs = @(
    '-S', $root,
    '-B', $buildDir,
    '-GNinja',
    "-DCMAKE_BUILD_TYPE=$Config",
    "-DCMAKE_CXX_COMPILER=$mingwBin\g++.exe",
    "-DNMN_QT_DIR=$qtDir",
    "-DNMN_OPENCV_DIR=$opencvDir",
    "-DNMN_TESSERACT_RUNTIME=$tessDir"
)

& cmake @configureArgs
if ($LASTEXITCODE -ne 0) {
    Write-Host '[x] 配置失败' -ForegroundColor Red
    exit $LASTEXITCODE
}

# ---- 编译 ------------------------------------------------------------
Write-Host "[*] 编译" -ForegroundColor Cyan
& cmake --build $buildDir --parallel
if ($LASTEXITCODE -ne 0) {
    Write-Host '[x] 编译失败' -ForegroundColor Red
    exit $LASTEXITCODE
}

$exe = Join-Path $buildDir 'NMN.exe'
if (-not (Test-Path $exe)) {
    Write-Host '[x] 没生成 NMN.exe' -ForegroundColor Red
    exit 1
}

# ---- 可选：部署 Qt 运行库 --------------------------------------------
if ($Deploy) {
    $windeployqt = Join-Path $qtDir 'bin\windeployqt.exe'
    if (Test-Path $windeployqt) {
        Write-Host '[*] windeployqt' -ForegroundColor Cyan
        & $windeployqt --release --no-translations --no-system-d3d-compiler --no-opengl-sw $exe
    }
    else {
        Write-Host "[!] 找不到 windeployqt: $windeployqt" -ForegroundColor Yellow
    }
}

$size = [math]::Round((Get-Item $exe).Length / 1KB)
Write-Host "[√] 完成: $exe ($size KB)" -ForegroundColor Green

# third_party —— Tesseract 开发文件

本项目**直接链接** `libtesseract`，不再通过 `QProcess` 调用 `tesseract.exe`。

官方 Windows 安装包（UB-Mannheim 版）只带运行时 DLL，不含头文件和导入库，
所以这里放了补齐所需的文件。

## 目录内容

| 路径 | 说明 |
|---|---|
| `include/tesseract/*.h` | Tesseract **5.5.0** 官方公开头文件 |
| `include/tesseract/version.h` | 由 `version.h.in` 手工生成（官方源码里该文件由 CMake 产出） |
| `lib/libtesseract.dll.a` | 导入库，**从本机 `libtesseract-5.dll` 的导出表生成** |

头文件不需要 Leptonica：Tesseract 5.5 的 `baseapi.h` 在内部
`struct Pix; struct Pixa; struct Boxa;` 自行前向声明了这三个类型。

## 导入库是怎么来的

要求：`gendef` / `dlltool`（Qt 自带的 mingw 工具链里就有）。

```powershell
$env:PATH = "D:\Qt\Tools\mingw1310_64\bin;" + $env:PATH
cd $env:TEMP

# 1. 从实际要用的 DLL 导出符号表
gendef "D:\Tesseract-OCR\libtesseract-5.dll"

# 2. 生成 mingw 可链接的导入库
dlltool -d libtesseract-5.def -l libtesseract.dll.a -D libtesseract-5.dll
```

从**本机 DLL** 生成而不是用别人编译好的导入库，是为了保证符号集合与
运行时加载的 DLL 完全一致。

## 头文件是怎么来的

```powershell
curl -L -o tess.tar.gz "https://github.com/tesseract-ocr/tesseract/archive/refs/tags/5.5.0.tar.gz"
tar -xzf tess.tar.gz
# 取 tesseract-5.5.0/include/tesseract/*.h
```

## 版本对齐（重要）

头文件版本、导入库、运行时 DLL **三者必须同源**：

| 文件 | 版本 |
|---|---|
| `include/tesseract/*.h` | 5.5.0（源码 tag） |
| `lib/libtesseract.dll.a` | 由 `libtesseract-5.dll` 导出表生成 |
| 运行时 `D:\Tesseract-OCR\libtesseract-5.dll` | 5.5.0 |

C++ 类如果版本不匹配，成员布局会错位，运行时会静默读到脏数据而不是报错。
升级 Tesseract 时，这三个都要同步更新。

## 运行时

`CMakeLists.txt` 会把 `NMN_TESSERACT_RUNTIME`（默认 `D:/Tesseract-OCR`）下的
所有 DLL 拷到 exe 同级目录。语言包路径由 `vision::OcrConfig::dataPath`
指定，默认 `D:/Tesseract-OCR/tessdata`。

## 许可证

Tesseract 与 Leptonica 均为 Apache-2.0，本目录仅包含其公开头文件与导入库，
未修改原文件（`version.h` 为按官方模板生成的产物）。

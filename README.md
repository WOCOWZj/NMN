# NMN

简谱识别工具：拖入一张简谱/五线谱图片，自动切分并识别出数字音符，
支持人工校正、设定调号、移调，最后导出带标注的图片。

Qt 6 Widgets + OpenCV + Tesseract。

## 快速使用

**双击 `NMN.cmd`** —— 自动增量编译并启动。改完代码再双击一次即可。

也可以直接在终端里用脚本：

```powershell
.\scripts\build.ps1          # 只编译
.\scripts\run.ps1            # 编译并启动
.\scripts\run.ps1 -NoBuild   # 跳过编译，直接运行
.\scripts\probe.ps1          # 跑识别准确率探针（不开 GUI）
```

> 若 PowerShell 提示脚本被禁止执行，用 `powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1`，
> 或直接走 `NMN.cmd`（它已带 Bypass）。

## 脚本

| 脚本 | 作用 |
|---|---|
| `NMN.cmd` | 双击入口：增量编译 + 启动 |
| `scripts/build.ps1` | 配置并编译。`-Clean` 清理，`-Deploy` 附带 windeployqt |
| `scripts/run.ps1` | 启动程序。`-NoBuild` 跳过编译，`-Wait` 等待退出，`-DebugSeg` 输出切分调试图 |
| `scripts/probe.ps1` | 命令行跑视觉流水线，对照人工标注算准确率 |

工具链路径默认写死为本机环境，换机器时用环境变量覆盖，不必改脚本：

```
NMN_QT_DIR        D:/Qt/6.9.3/mingw_64
NMN_MINGW_DIR     D:\Qt\Tools\mingw1310_64\bin
NMN_NINJA_DIR     D:\Qt\Tools\Ninja
NMN_OPENCV_DIR    D:/opencv/opencv/build/x64/mingw/lib
NMN_TESSERACT_DIR D:/Tesseract-OCR
```

## 目录结构

```
src/
├── main.cpp        只做装配
├── core/           领域模型：Note（音高换算）、Score（音符集合 + 调号 + 移调）
├── vision/         视觉层，只依赖 OpenCV
│   ├── ImageNormalizer    二值化，统一"墨迹=255"极性
│   ├── GlyphExtractor     去谱线 → 选行 → 连通域 → 八度点归属
│   ├── TemplateClassifier 字形模板匹配（主分类器）
│   └── GlyphClassifier    Tesseract（低置信度兜底）
├── pipeline/       编排：RecognitionWorker / RecognitionPipeline / ScoreRenderer
├── ui/             MainWindow / ScoreView / NoteDialog
└── util/           Logger / FontFit
resources/templates/  35 个字形模板（5 种字体 × 数字 1-7），编入 qrc
third_party/          Tesseract 头文件与导入库，见其中 README
tools/probe.cpp       识别准确率探针
```

依赖方向单向：`ui → pipeline → vision → core`，`util` 为叶子。
`core/` 不依赖任何 Qt Widgets 或 OpenCV。

## 识别流程

```
图片 → 二值化（自适应阈值，Otsu 兜底）
     → 水平开运算剥离减时线
     → 垂直投影选出乐谱行（排除标题与词曲作者）
     → 行内连通域；过宽的按列投影谷点拆开
     → 滤除无主笔的弧线（反复记号括号）与偏离基线的标注（三连音数字）
     → 字形放大到约 40px 后模板匹配；洞数用于区分 5 与 6
     → 八度点归属
```

字号小于约 40px 时会先把整图放大再重跑一遍——低分辨率下二值化噪声会
打碎 6 的闭合回路，洞数判据会失效。

## 已知限制

- 粗体字体在低分辨率下可能把 `5` 的碗状部分粘连成闭合回路，与 `6` 混淆；
  当前样本上 29 个音符错 1 个，可在界面左键手动改。
- 减时线以外的节奏信息（附点、连音线、三连音）目前不参与识别。
- 模板为 5 种常见中文字体，特殊字体可能需要补充模板。

## 界面操作

| 操作 | 效果 |
|---|---|
| 拖入图片 | 重新识别 |
| 悬停 | 高亮该音符 |
| 左键点音符 | 修改音名 / 升降号 / 音区 |
| 右键点音符 | 删除 |
| Shift + 拖拽 | 框选区域新增音符 |
| 回车 | 设定调号 → 确认提交 → 另存图片 |

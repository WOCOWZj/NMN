#pragma once // 预编译指令，确保头文件只被包含一次

#include <QLabel>      // Qt标签控件
#include <QObject>     // Qt对象基类
#include <QList>       // Qt列表容器
#include <QRect>       // Qt矩形类
#include <QEvent>      // Qt事件基类
#include <QMouseEvent> // Qt鼠标事件
#include <QPainter>    // Qt绘图类
#include <QFont>       // Qt字体类
#include <qminmax.h>   // Qt最小最大值宏定义
#include <QColor>      // Qt颜色类
#include <QWindow>     // Qt窗口类
#include <QVBoxLayout> // Qt垂直布局
#include <QString>     // Qt字符串类
#include <QFile>       // Qt文件操作类
#include <QMessageBox> // Qt消息框类

#include "noteclass.h"    // 音符类定义
#include "modification.h" // 修改对话框类

// HighLightLabel类：用于高亮显示音符的自定义标签，继承自QLabel
class HighLightLabel : public QLabel
{
Q_OBJECT // Qt元对象系统宏，启用信号槽机制

    private : QPixmap pixmap; // 显示的图像
    QList<Note> notes;        // 音符列表
    // adaptFontSize方法：根据矩形大小自适应调整字体大小
    void adaptFontSize(QPainter &painter, const QString &text);
    int spacing;                          // 音符文本与矩形的间距
    const QColor outline_color = Qt::red; // 矩形边框颜色

    int hovered_ind = -1; // 当前鼠标悬停的音符索引

    int press_ind = -1;               // 鼠标按下时的音符索引
    int release_ind = -1;             // 鼠标释放时的音符索引
    bool leftbutton_pressed = false;  // 左键是否按下
    bool rightbutton_pressed = false; // 右键是否按下

    bool selecting = false;         // 是否正在进行框选
    QRect selecting_rect = QRect(); // 框选矩形

protected:
    // 重写鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event) override;
    // 重写鼠标离开事件
    void leaveEvent(QEvent *event) override;
    // 重写绘制事件
    void paintEvent(QPaintEvent *event) override;
    // 重写鼠标按下事件
    void mousePressEvent(QMouseEvent *event) override;
    // 重写鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event) override;
    // 重写键盘按下事件
    void keyPressEvent(QKeyEvent *event) override;

public:
    // 构造函数：创建高亮标签
    HighLightLabel(QWidget *parent = nullptr);
    // init方法：初始化标签
    // 参数：pixmap - 要显示的图像，notes - 音符列表
    void init(const QPixmap &pixmap, const QList<Note> &notes);

signals:
    // LeftClick信号：左键点击音符
    void LeftClick(int ind);
    // RightClick信号：右键点击音符
    void RightClick(int ind);
    // Framed信号：完成框选
    void Framed();
    // ResultReady信号：结果准备就绪
    // 参数：image - 图像，tune - 调号，notes - 音符列表
    void ResultReady(const QPixmap &image, const Note &tune, const QList<Note> &notes);

public slots:
    // onLeftClick槽：处理左键点击
    void onLeftClick(int ind);
    // onRightClick槽：处理右键点击
    void onRightClick(int ind);
    // onFramed槽：处理框选完成
    void onFramed();
};

// HighLight类：高亮显示窗口，继承自QWidget
class HighLight : public QWidget
{
Q_OBJECT // Qt元对象系统宏，启用信号槽机制

    public : HighLightLabel *label; // 高亮标签指针
    // 构造函数：创建高亮窗口
    HighLight(QWidget *parent = nullptr);

public slots:
    // onImageProcessed槽：处理图像处理完成信号
    // 参数：pixmap - 处理后的图像，notes - 音符列表
    void onImageProcessed(const QPixmap &pixmap, const QList<Note> &notes);
};
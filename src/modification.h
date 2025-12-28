#pragma once // 预编译指令，确保头文件只被包含一次

#include <QDialog>     // Qt对话框基类
#include <QObject>     // Qt对象基类
#include <QLabel>      // Qt标签控件
#include <QComboBox>   // Qt下拉框控件
#include <QBoxlayout>  // Qt布局管理器
#include <QPushButton> // Qt按钮控件
#include <QFile>       // Qt文件操作类

#include "noteclass.h" // 音符类定义

// Modification类：音符修改对话框，继承自QDialog
class Modification : public QDialog
{
Q_OBJECT // Qt元对象系统宏，启用信号槽机制

    public :
    // 模式枚举：定义对话框的不同使用模式
    enum class Mode {
        Mod8App, // 修改应用模式
        Tune     // 调音模式
    };

    // 构造函数：创建修改对话框
    // 参数：title - 对话框标题，parent - 父窗口，note - 要修改的音符，mode - 模式
    explicit Modification(const QString &title,
                          QWidget *parent = nullptr,
                          const Note &note = Note(),
                          const Mode &mode = Mode::Mod8App);

    // exec方法：执行对话框（重写基类方法）
    int exec() override;

    // getResult方法：获取修改后的音符结果
    Note getResult() const;

public slots:
    // onAccept槽：处理确认按钮点击事件
    void onAccept();
    // onCancel槽：处理取消按钮点击事件
    void onCancel();

private:
    QLabel *notename_label;         // 音名标签
    QComboBox *notename_combobox;   // 音名下拉框
    QLabel *accidental_label;       // 变音记号标签
    QComboBox *accidental_combobox; // 变音记号下拉框
    QLabel *octdot_label;           // 八度标签
    QComboBox *octdot_combobox;     // 八度下拉框
    QPushButton *accept_button;     // 确认按钮
    QPushButton *cancel_button;     // 取消按钮
    Note result;                    // 修改结果
};
#include "NMN.h" // 包含对应的头文件

// NMN构造函数：初始化主窗口
NMN::NMN(QWidget *parent)
    : QMainWindow(parent), ui(new Ui_NMN) // 初始化基类和UI指针
{
    ui->setupUi(this);          // 设置UI界面
    this->setAcceptDrops(true); // 启用拖拽功能
}

// NMN析构函数：清理资源
NMN::~NMN()
{
    delete ui; // 删除UI对象
}

// dragEnterEvent方法：处理拖拽进入事件
void NMN::dragEnterEvent(QDragEnterEvent *event)
{
    // 检查拖拽数据是否包含URL且数量为1
    if (event->mimeData()->hasUrls() and event->mimeData()->urls().size() == 1)
    {
        QString filepath = event->mimeData()->urls().at(0).path(); // 获取文件路径
        filepath = filepath.right(filepath.length() - 1);          // 去除路径开头的斜杠
        QPixmap pixmap(filepath);                                  // 尝试加载图像
        if (!pixmap.isNull())                                      // 如果图像加载成功
            event->acceptProposedAction();                         // 接受拖拽操作
        else
            event->ignore(); // 忽略拖拽操作
    }
    else
        event->ignore(); // 忽略拖拽操作

    return; // 返回
}

// dropEvent方法：处理拖拽释放事件
void NMN::dropEvent(QDropEvent *event)
{
    QString filepath = event->mimeData()->urls().at(0).path(); // 获取文件路径
    filepath = filepath.right(filepath.length() - 1);          // 去除路径开头的斜杠

    this->image.load(filepath); // 加载图像到成员变量

    ui->infoLabel->setText(filepath); // 在信息标签中显示文件路径

    // 设置滚动区域和图像标签的最小宽度
    ui->scrollarea->setMinimumWidth(800);
    ui->imageLabel->setMinimumWidth(800);
    // 设置滚动条策略
    ui->scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    // 缩放图像以适应标签宽度
    QPixmap scaledPixmap = this->image.scaledToWidth(ui->imageLabel->width() - 50, Qt::SmoothTransformation);
    ui->imageLabel->setPixmap(scaledPixmap); // 在标签中显示缩放后的图像

    emit ImageDropped(filepath); // 发射图像拖拽信号

    event->accept(); // 接受事件

    return; // 返回
}
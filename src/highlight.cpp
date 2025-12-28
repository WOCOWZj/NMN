#include "highlight.h" // 包含对应的头文件

// HighLight构造函数：创建高亮显示窗口
HighLight::HighLight(QWidget *parent)
    : QWidget(parent) // 调用父类构造函数
{
    this->label = new HighLightLabel(this);      // 创建高亮标签
    this->label->setParent(this);                // 设置标签的父窗口
    QVBoxLayout *layout = new QVBoxLayout(this); // 创建垂直布局
    layout->addWidget(this->label);              // 将标签添加到布局
    layout->setContentsMargins(0, 0, 0, 0);      // 设置布局边距
    setLayout(layout);                           // 设置窗口布局
}

// onImageProcessed槽：处理图像处理完成信号
void HighLight::onImageProcessed(const QPixmap &pixmap, const QList<Note> &notes)
{
    this->label->init(pixmap, notes); // 初始化高亮标签
    resize(label->size());            // 调整窗口大小
    show();                           // 显示窗口
    update();                         // 更新显示
    this->label->setFocus();          // 设置焦点
}

// HighLightLabel构造函数：创建高亮标签
HighLightLabel::HighLightLabel(QWidget *parent)
    : QLabel(parent) // 调用父类构造函数
{
    setMouseTracking(true);          // 启用鼠标跟踪
    setFocusPolicy(Qt::StrongFocus); // 设置焦点策略

    // 连接信号槽：将自定义信号连接到对应的槽函数
    connect(this, &HighLightLabel::LeftClick, this, &HighLightLabel::onLeftClick);
    connect(this, &HighLightLabel::RightClick, this, &HighLightLabel::onRightClick);
    connect(this, &HighLightLabel::Framed, this, &HighLightLabel::onFramed);
}

// init方法：初始化标签
void HighLightLabel::init(const QPixmap &pixmap, const QList<Note> &notes)
{
    this->pixmap = pixmap;                   // 设置图像
    this->notes = notes;                     // 设置音符列表
    this->setFixedSize(this->pixmap.size()); // 设置固定大小为图像大小
    this->setAlignment(Qt::AlignCenter);     // 设置对齐方式

    // 计算最大高度以确定间距
    int max_h = 0;
    for (const Note note : this->notes) // 遍历所有音符
    {
        const QRect rect = note.rect;       // 获取音符矩形
        max_h = qMax(max_h, rect.height()); // 找到最大高度
    }
    this->spacing = qRound(1.5 * max_h); // 设置间距为最大高度的1.5倍
}

// adaptFontSize方法：自适应调整字体大小
void HighLightLabel::adaptFontSize(QPainter &painter, const QString &text)
{
    int sum_w = 0, sum_h = 0;           // 总宽度和总高度
    for (const Note note : this->notes) // 遍历所有音符
    {
        const QRect rect = note.rect; // 获取音符矩形
        sum_w += rect.width();        // 累加宽度
        sum_h += rect.height();       // 累加高度
    }
    // 计算平均宽度和高度
    int ave_w = sum_w / this->notes.length(), ave_h = sum_h / this->notes.length();
    QRect ave_rect = QRect(0, 0, ave_w, ave_h); // 创建平均矩形

    QFont font = painter.font(); // 获取当前字体
    int fontSize = 12;           // 初始字体大小
    font.setPointSize(fontSize); // 设置字体大小
    painter.setFont(font);       // 应用字体

    QFontMetrics fm(font);                                       // 字体度量
    QRect br = fm.boundingRect(ave_rect, Qt::AlignCenter, text); // 获取文本边界矩形

    // 调整字体大小直到文本适合矩形
    while (fontSize > 1 && (br.width() > ave_rect.width() || br.height() > ave_rect.height()))
    {
        fontSize--;                                            // 减小字体大小
        font.setPointSize(fontSize);                           // 设置新字体大小
        painter.setFont(font);                                 // 应用字体
        fm = QFontMetrics(font);                               // 更新字体度量
        br = fm.boundingRect(ave_rect, Qt::AlignCenter, text); // 重新计算边界
    }
}

// paintEvent方法：绘制事件处理
void HighLightLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);                         // 创建绘图对象
    painter.setPen(QPen(this->outline_color));      // 设置画笔颜色
    adaptFontSize(painter, "F");                    // 调整字体大小
    painter.drawPixmap(QPoint(0, 0), this->pixmap); // 绘制图像

    // 绘制每个音符的文本
    for (const Note note : notes)
    {
        QString note_text = note.Note2QString();                                 // 获取音符文本表示
        painter.drawText(note.rect.translated(0, spacing).topLeft(), note_text); // 在音符下方绘制文本
    }

    // 如果有悬停的音符，绘制红色边框
    if (this->hovered_ind != -1)
        painter.drawRect(this->notes.at(this->hovered_ind).rect);

    // 如果正在进行框选，绘制选择框
    if (selecting)
        painter.drawRect(this->selecting_rect.normalized());
}

// mouseMoveEvent方法：鼠标移动事件处理
void HighLightLabel::mouseMoveEvent(QMouseEvent *event)
{
    QPoint Pos = event->pos(); // 获取鼠标位置
    if (this->selecting)       // 如果正在进行框选
    {
        this->selecting_rect.setBottomRight(Pos); // 更新选择框的右下角
        update();                                 // 更新显示
    }
    else // 如果不是框选模式
    {
        int new_hovered_ind = -1; // 新的悬停索引
        // 检查鼠标是否在某个音符矩形内
        for (int i = 0; i < this->notes.length(); i++)
            if (notes.at(i).rect.contains(Pos)) // 如果鼠标在音符矩形内
            {
                new_hovered_ind = i; // 设置悬停索引
                break;
            }

        if (this->hovered_ind != new_hovered_ind) // 如果悬停索引发生变化
        {
            this->hovered_ind = new_hovered_ind; // 更新悬停索引
            update();                            // 更新显示
        }
    }
    QLabel::mouseMoveEvent(event); // 调用父类的鼠标移动事件处理
}

// leaveEvent方法：鼠标离开事件处理
void HighLightLabel::leaveEvent(QEvent *event)
{
    this->hovered_ind = -1;    // 清除悬停索引
    this->press_ind = -1;      // 清除按下索引
    this->release_ind = -1;    // 清除释放索引
    this->selecting = false;   // 停止框选
    QLabel::leaveEvent(event); // 调用父类的离开事件处理
}

// mousePressEvent方法：鼠标按下事件处理
void HighLightLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton or event->button() == Qt::RightButton) // 如果是左键或右键
    {
        QPoint Pos = event->pos();                  // 获取鼠标位置
        if (event->modifiers() & Qt::ShiftModifier) // 如果按住Shift键
        {
            this->selecting = true;               // 开始框选
            this->selecting_rect.setTopLeft(Pos); // 设置选择框的左上角
            setCursor(Qt::CrossCursor);           // 设置十字光标
            update();                             // 更新显示
            this->press_ind = -1;                 // 清除按下索引
        }
        else // 如果没有按Shift键
        {
            this->press_ind = -1;                                             // 初始化按下索引
            this->leftbutton_pressed = (event->button() == Qt::LeftButton);   // 记录左键状态
            this->rightbutton_pressed = (event->button() == Qt::RightButton); // 记录右键状态
            // 查找鼠标位置对应的音符
            for (int i = 0; i < this->notes.length(); i++)
                if (notes.at(i).rect.contains(Pos)) // 如果鼠标在音符矩形内
                {
                    this->press_ind = i; // 记录按下索引
                    break;
                }
        }
    }
    QLabel::mousePressEvent(event); // 调用父类的鼠标按下事件处理
}

// mouseReleaseEvent方法：鼠标释放事件处理
void HighLightLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton or event->button() == Qt::RightButton) // 如果是左键或右键
    {
        if (this->selecting) // 如果正在进行框选
        {
            this->release_ind = -1;                // 清除释放索引
            if (event->button() == Qt::LeftButton) // 如果是左键释放
            {
                this->selecting = false; // 停止框选
                emit Framed();           // 发射框选完成信号
            }
        }
        else if (press_ind != -1) // 如果有按下索引
        {
            QPoint Pos = event->pos(); // 获取鼠标位置
            this->release_ind = -1;    // 初始化释放索引
            // 查找鼠标位置对应的音符
            for (int i = 0; i < this->notes.length(); i++)
                if (notes.at(i).rect.contains(Pos)) // 如果鼠标在音符矩形内
                {
                    this->release_ind = i; // 记录释放索引
                    break;
                }
            if (this->release_ind == this->press_ind) // 如果按下和释放的是同一个音符
            {
                int ind = this->press_ind;                                                  // 获取音符索引
                if (event->button() == Qt::LeftButton and this->leftbutton_pressed == true) // 如果是左键点击
                {
                    emit LeftClick(ind);              // 发射左键点击信号
                    this->leftbutton_pressed = false; // 重置左键状态
                }
                if (event->button() == Qt::RightButton and this->rightbutton_pressed == true) // 如果是右键点击
                {
                    emit RightClick(ind);              // 发射右键点击信号
                    this->rightbutton_pressed = false; // 重置右键状态
                }
            }
            this->press_ind = -1;   // 清除按下索引
            this->release_ind = -1; // 清除释放索引
        }
    }
    QLabel::mouseReleaseEvent(event); // 调用父类的鼠标释放事件处理
}

// keyPressEvent方法：键盘按下事件处理
void HighLightLabel::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return) // 如果按下了回车键
    {
        Modification modif("调号", this);      // 创建调号修改对话框
        if (modif.exec() == QDialog::Accepted) // 如果对话框确认
        {
            Note tune = modif.getResult(); // 获取调号结果
            {
                QFile logFile("NMN.log"); // 记录调号日志
                if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
                {
                    QTextStream out(&logFile);
                    out << QString("tune: %1\n").arg(tune.Note2QString()); // 记录调号信息
                }
                logFile.close();
            }
            // 显示确认对话框
            QMessageBox::StandardButton reply = QMessageBox::question(nullptr, "提交", "确定要提交吗");
            if (reply == QMessageBox::Yes) // 如果确认提交
            {
                emit ResultReady(this->pixmap, tune, this->notes); // 发射结果就绪信号
                this->parentWidget()->close();                     // 关闭父窗口
            }
        }
    }
    QLabel::keyPressEvent(event); // 调用父类的键盘按下事件处理
}

// onLeftClick槽：处理左键点击
void HighLightLabel::onLeftClick(int ind)
{
    {
        QFile logFile("NMN.log"); // 记录修改日志
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << QString("modifying ind %1, %2 (%3 %4 %5)\n")
                       .arg(ind)                                              // 音符索引
                       .arg(notes.at(ind).Note2QString())                     // 音符文本
                       .arg(static_cast<int>(this->notes.at(ind).noteName))   // 音名
                       .arg(static_cast<int>(this->notes.at(ind).accidental)) // 变音记号
                       .arg(static_cast<int>(this->notes.at(ind).octDot));    // 八度
        }
        logFile.close();
    }
    Modification modif("修改", this, this->notes.at(ind)); // 创建修改对话框
    if (modif.exec() == QDialog::Accepted)                 // 如果对话框确认
    {
        Note note = modif.getResult(); // 获取修改结果
        {
            QFile logFile("NMN.log"); // 记录修改完成日志
            if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
            {
                QTextStream out(&logFile);
                out << QString("modified note: %1\n").arg(note.Note2QString()); // 记录修改的音符
            }
            logFile.close();
        }
        note.rect = this->notes.at(ind).rect; // 保持矩形位置不变
        this->notes.replace(ind, note);       // 替换音符
    }
    update(); // 更新显示
}

// onRightClick槽：处理右键点击
void HighLightLabel::onRightClick(int ind)
{
    // 显示删除确认对话框
    QMessageBox::StandardButton reply = QMessageBox::question(nullptr, "删除", "确定要删除吗");
    if (reply == QMessageBox::Yes) // 如果确认删除
        this->notes.removeAt(ind); // 移除音符
}

// onFramed槽：处理框选完成
void HighLightLabel::onFramed()
{
    Modification modif("添加", this);      // 创建添加音符对话框
    if (modif.exec() == QDialog::Accepted) // 如果对话框确认
    {
        Note note = modif.getResult(); // 获取添加的音符
        {
            QFile logFile("NMN.log"); // 记录添加日志
            if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
            {
                QTextStream out(&logFile);
                out << QString("append note: %1\n").arg(note.Note2QString()); // 记录添加的音符
            }
            logFile.close();
        }
        note.rect = this->selecting_rect; // 设置音符矩形为选择框
        this->notes.append(note);         // 添加音符到列表
    }
    update(); // 更新显示
}
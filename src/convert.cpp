#include "convert.h" // 包含对应的头文件

// onResultReady槽：处理结果就绪信号
void Converter::onResultReady(const QPixmap &image, const Note &tune, const QList<Note> &notes)
{
    this->origin_image = image;       // 保存原始图像
    this->notes = notes;              // 保存音符列表
    this->tune2int = tune.Note2int(); // 将调号转换为整数值

    // 根据调号调整所有音符的音高
    for (Note &note : this->notes) // 遍历所有音符
    {
        // 将原始音符与调号结合，转换为新的音符
        Note newnote = Note::int2Note(tune2int + note.Note2int());
        newnote.rect = note.rect; // 保持矩形位置不变
        note = newnote;           // 更新音符
    }
    convert();    // 执行转换操作
    output(this); // 输出转换后的图像
}

// convert方法：执行转换操作，生成带有音符标注的图像
void Converter::convert()
{
    if (this->origin_image.isNull()) // 检查原始图像是否为空
        return;                      // 如果为空则返回

    this->converted_image = QPixmap(this->origin_image.size()); // 创建与原图相同大小的图像
    this->converted_image.fill(Qt::white);                      // 用白色填充图像

    QPainter painter(&this->converted_image);             // 创建绘图对象
    painter.drawPixmap(QPoint(0, 0), this->origin_image); // 绘制原始图像

    painter.setBrush(Qt::white);     // 设置画刷为白色
    painter.setPen(Qt::transparent); // 设置画笔为透明
    // 用白色矩形覆盖原始音符区域，为新音符文本提供背景
    for (const Note &note : this->notes)
    {
        QRect rect = note.rect;                      // 获取音符矩形
        rect.setTop(rect.top() + rect.height() / 8); // 调整矩形顶部位置
        rect.setHeight(rect.height() / 7 * 8);       // 调整矩形高度
        painter.drawRect(rect);                      // 绘制白色背景矩形
    }

    adaptFontSize(painter, "F"); // 自适应调整字体大小
    painter.setPen(Qt::red);     // 设置画笔为红色
    // 在每个音符位置绘制转换后的音符文本
    for (const Note &note : this->notes)
    {
        const QString note_text = note.Note2QString(); // 获取音符文本
        // 计算文本绘制位置（在矩形上3/4处）
        painter.drawText((note.rect.topLeft() + 3 * note.rect.bottomLeft()) / 4, note_text);
    }
}

// output方法：输出转换后的图像
bool Converter::output(QWidget *parent)
{
    if (this->converted_image.isNull()) // 检查转换后的图像是否为空
    {
        QMessageBox::warning(parent, "警告", "无法保存空图像。"); // 显示警告
        return false;                                             // 返回失败
    }

    // 显示文件保存对话框
    QString fileName = QFileDialog::getSaveFileName(
        parent,                                                                       // 父窗口
        QObject::tr("保存图像"),                                                      // 对话框标题
        "",                                                                           // 初始路径
        QObject::tr("PNG 图像 (*.png);;JPEG 图像 (*.jpg *.jpeg);;BMP 图像 (*.bmp)")); // 文件类型过滤器

    if (fileName.isEmpty()) // 如果没有选择文件
        return false;       // 返回失败

    bool success = this->converted_image.save(fileName);                   // 保存图像
    if (!success)                                                          // 如果保存失败
        QMessageBox::critical(parent, "Error", "Failed to Save");          // 显示错误消息
    else                                                                   // 如果保存成功
        QMessageBox::information(parent, "Success", "Successfully Saved"); // 显示成功消息

    return success; // 返回保存结果
}

// adaptFontSize方法：自适应调整字体大小
void Converter::adaptFontSize(QPainter &painter, const QString &text)
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
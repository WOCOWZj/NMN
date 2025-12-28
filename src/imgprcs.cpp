#include "imgprcs.h" // 包含对应的头文件

// onImageReceived方法：接收图像并开始处理
void ImgProcessor::onImageReceived(const QString &filepath)
{
    this->origin_img = cv::imread(filepath.toStdString()); // 读取图像文件
    this->origin_img_path = filepath;                      // 保存图像路径
    if (this->origin_img.empty())                          // 检查图像是否加载成功
    {
        QMessageBox::critical(nullptr, "Error", "cannot read image"); // 显示错误消息
        return;                                                       // 返回
    }
    this->rows = origin_img.rows, this->cols = origin_img.cols; // 获取图像尺寸

    pretreat(); // 执行预处理

    hsegment(); // 执行水平分割

    vsegment(); // 执行垂直分割

    ocr(); // 执行OCR识别

    return; // 返回
}

// pretreat方法：图像预处理
void ImgProcessor::pretreat()
{
    cv::Mat gray_image;                                             // 灰度图像
    cv::cvtColor(this->origin_img, gray_image, cv::COLOR_RGB2GRAY); // 将原图转换为灰度图

    cv::Mat blur_image;                                            // 模糊处理后的图像
    cv::GaussianBlur(gray_image, blur_image, cv::Size(3, 3), 0.5); // 高斯模糊去噪

    cv::Mat bin_image;        // 二值化图像
    int actualBlockSize = 21; // 自适应阈值块大小
    double C_value = 12;      // 自适应阈值常数
    // 自适应阈值二值化，将灰度图转换为黑白图
    cv::adaptiveThreshold(blur_image, bin_image, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, actualBlockSize, C_value);

    this->pretreated_image = bin_image; // 保存预处理后的图像

    // 记录预处理完成日志
    {
        QFile logFile("NMN.log");
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << QString("pretreat finished: %1 by %2\n").arg(this->rows).arg(this->cols); // 记录图像尺寸
            logFile.close();
        }
    }
}

// hsegment方法：水平分割，识别图像中的水平线段
void ImgProcessor::hsegment()
{
    cv::Mat hproj;                   // 水平投影矩阵
    hproj.create(rows, 1, CV_32SC1); // 创建投影矩阵

    // 计算每一行的投影值（非零像素数量）
    for (int i = 0; i < rows; i++)
        hproj.at<int>(i, 0) = cv::countNonZero(this->pretreated_image.row(i));

    // 根据投影值识别水平线段
    for (int i = 0; i < rows; i++)
    {
        int j = 0;
        // 找到连续的高投影值区域
        while (i + j < rows and hproj.at<int>(i + j, 0) > cols * 0.15) // 阈值为总列数的15%
            j++;
        if (j > 5) // 如果区域高度大于5像素
        {
            // 创建lineH结构体，调整顶部位置和高度
            lineH lineh = {.top = qMax(0, i - j / 4), .height = qMin(j / 2 * 3, rows - i)};
            this->lineh_list.append(lineh); // 添加到线段列表
        }
        i += j; // 移动到下一个区域
    }

    /*
    // 调试代码：显示水平投影可视化
    cv::Mat hproj_vis;
    hproj_vis.create(rows, cols / 10, CV_8UC1);

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < hproj.at<int>(i, 0) / 10; j++)
            hproj_vis.at<unsigned char>(i, j) = 255;

    cv::imshow("hproj", hproj_vis);
    */

    /*
    // 调试代码：在图像上标记识别的线段
    cv::Mat marked_image;
    cv::cvtColor(this->pretreated_image, marked_image, cv::COLOR_GRAY2BGR);
    for (lineH const &lineh : this->lineh_list)
        cv::rectangle(marked_image, cv::Rect(cols / 100, lineh.top, cols - cols / 100, lineh.height), cv::Scalar(0, 0, 255));

    {
        QFile logFile("NMN.log");
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << "hseg_line: ";
            for (const lineH &lineh : this->lineh_list)
                out << lineh.height << " ";
            out << "\n";
            logFile.close();
        }
    }

    cv::imshow("mark", marked_image);
    */

    // 记录水平分割完成日志
    QFile logFile("NMN.log");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&logFile);
        out << "hseg finished: " << this->lineh_list.length() << "lines\n"; // 记录识别到的线段数量
        logFile.close();
    }
}

// vsegment方法：垂直分割，识别每个水平线段中的字符矩形
void ImgProcessor::vsegment()
{
    // 遍历所有水平线段
    for (const lineH &lineh : lineh_list)
    {
        cv::Mat vproj;                         // 垂直投影矩阵
        vproj.create(1, this->cols, CV_32SC1); // 创建投影矩阵

        // 计算线段内每一列的投影值
        for (int i = 0; i < cols; i++)
            vproj.at<int>(0, i) = cv::countNonZero(this->pretreated_image.rowRange(lineh.top, lineh.top + lineh.height).col(i));

        // 根据投影值识别垂直方向的字符区域
        for (int i = 0; i < cols; i++)
        {
            int j = 0;
            // 找到连续的高投影值区域
            while (i + j < cols and vproj.at<int>(0, i + j) > lineh.height * 0.2) // 阈值为线段高度的20%
                j++;
            if (j > 5) // 如果区域宽度大于5像素
            {
                // 创建矩形区域，调整位置和大小
                this->rect_list.append(QRect(qMax(0, i - j / 4), lineh.top, qMin(cols, j * 3 / 2), lineh.height));
            }
            i += j; // 移动到下一个区域
        }
    }

    /*
    // 调试代码：在图像上标记识别的矩形区域
    cv::Mat marked_image;
    cv::cvtColor(this->pretreated_image, marked_image, cv::COLOR_GRAY2BGR);
    for (const QRect &rect : this->rect_list)
        cv::rectangle(marked_image, cv::Rect(rect.x(), rect.y(), rect.width(), rect.height()), cv::Scalar(255, 0, 0));

    cv::imshow("mark", marked_image);
    */

    // 记录垂直分割完成日志
    QFile logFile("NMN.log");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&logFile);
        out << "vseg finished: " << this->rect_list.length() << "rects\n"; // 记录识别到的矩形数量
        logFile.close();
    }
}

// ocr方法：执行光学字符识别
void ImgProcessor::ocr()
{
    Controller *controller = new Controller; // 创建OCR控制器
    // 连接结果信号到接收槽
    connect(controller, &Controller::ResultReady, this, &ImgProcessor::onResultReceived);
    // 发射操作信号，启动OCR处理
    emit controller->Operate(this->pretreated_image, rect_list, "01234567"); // 只识别数字0-7
}

// onResultReceived方法：处理OCR结果
void ImgProcessor::onResultReceived(const QList<QString> &result)
{
    // 解析OCR结果，将字符转换为音符
    for (int i = 0; i < result.length(); i++)
    {
        const QString s = result.at(i);          // 获取第i个识别结果
        if (s != "")                             // 如果结果不为空
            for (int j = 0; j < s.length(); j++) // 遍历结果字符串中的每个字符
            {
                const QChar ch = s.at(j); // 获取第j个字符
                if (ch == '0')            // 跳过字符'0'
                    continue;
                Note note;                                                        // 创建音符对象
                note.noteName = static_cast<Note::NoteName>(ch.digitValue() - 1); // 将数字字符转换为音名（1-7对应Do-Ti）
                note.accidental = Note::Accidental::NATURAL;                      // 设置为自然音
                note.octDot = Note::OctDot::NONE;                                 // 设置为中八度
                // 计算音符在图像中的精确位置
                note.rect = QRect(this->rect_list.at(i).x() + this->rect_list.at(i).width() / s.length() * j,
                                  this->rect_list.at(i).top(),
                                  this->rect_list.at(i).width() / s.length(),
                                  this->rect_list.at(i).height());
                this->note_list.append(note); // 添加到音符列表
            }
    }

    // 记录音符识别完成日志
    {
        QFile logFile("NMN.log");
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << QString("found %1 notes:").arg(this->note_list.length()); // 记录找到的音符数量
            for (const Note &note : this->note_list)
                out << " " << QString::number(static_cast<int>(note.noteName) + 1); // 记录音符值
            out << "\n";
        }
        logFile.close();
    }

    // 发射图像处理完成信号
    emit ImageProcessed(QPixmap(this->origin_img_path), this->note_list);
}
#include "tessthread.h" // 包含对应的头文件

// Worker类的doWork方法实现：执行OCR处理
void Worker::doWork(cv::Mat image, const QList<QRect> &rect_list, const QString &whitelist)
{
    // 记录OCR开始日志
    {
        QFile logFile("NMN.log");                                                     // 创建日志文件对象
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) // 以追加模式打开日志文件
        {
            QTextStream out(&logFile);       // 创建文本流
            out << "OCR started..." << "\n"; // 写入开始日志
            logFile.close();                 // 关闭文件
        }
    }

    QList<QString> result; // 存储OCR识别结果的列表

    QElapsedTimer timer; // 创建计时器
    timer.start();       // 开始计时

    // 遍历矩形列表，对每个矩形区域进行OCR处理
    for (const QRect &rect : rect_list)
    {
        // 从原图像中提取矩形区域
        cv::Mat rect_image = image(cv::Rect(rect.x(), rect.y(), rect.width(), rect.height())).clone();

        // 将矩形区域图像保存到临时文件
        if (!cv::imwrite(this->rect_path.toStdString(), rect_image))
        {
            // 保存失败时显示错误消息并记录日志
            QMessageBox::critical(nullptr, "Error", "Failed to Save");
            {
                QFile logFile("NMN.log");
                if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
                {
                    QTextStream out(&logFile);
                    out << QString("cannot save rect") << "\n";
                    logFile.close();
                }
            }
        }

        // 设置Tesseract OCR程序路径
        QString tessPath = "D:/Tesseract-OCR/tesseract.exe";
        QStringList args; // 创建参数列表
        // 添加Tesseract执行参数：输入图像路径、输出路径、页面分割模式
        args << this->rect_path << this->output_noext << "--psm" << "10";
        // 如果提供了字符白名单，则添加白名单参数
        if (whitelist != "")
            args << "-c" << "tessedit_char_whitelist=" + whitelist;

        QProcess process;              // 创建进程对象
        process.start(tessPath, args); // 启动Tesseract进程
        process.waitForFinished();     // 等待进程完成

        // 检查Tesseract执行是否成功
        if (process.exitCode() == 0)
        {
            QFile file(this->output_withext);   // 打开OCR输出文件
            if (file.open(QIODevice::ReadOnly)) // 以只读模式打开
            {
                // 读取文件内容，去除空格和换行符，添加到结果列表
                result.append(QString(file.readAll()).remove(" ").remove("\n"));
                file.close(); // 关闭文件
            }
        }
    }

    // 记录OCR完成日志
    {
        QFile logFile("NMN.log");
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << QString("OCR finished, taking %1 ms\n").arg(timer.elapsed()); // 记录耗时
        }
    }

    // 清理临时文件
    QFile::remove(this->rect_path);      // 删除临时图像文件
    QFile::remove(this->output_withext); // 删除临时文本文件

    // 发射结果就绪信号，传递识别结果
    emit ResultReady(result);
}

// Controller构造函数：初始化控制器并设置线程
Controller::Controller()
{
    Worker *worker = new Worker;         // 创建Worker对象
    worker->moveToThread(&workerThread); // 将Worker对象移动到工作线程
    // 连接线程结束信号到Worker的删除槽，确保线程结束后清理资源
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    // 连接控制器的Operate信号到Worker的doWork槽
    connect(this, &Controller::Operate, worker, &Worker::doWork);
    // 连接Worker的结果信号到控制器的结果信号
    connect(worker, &Worker::ResultReady, this, &Controller::ResultReady);
    workerThread.start(); // 启动工作线程
}

// Controller析构函数：清理线程资源
Controller::~Controller()
{
    workerThread.quit(); // 请求线程退出
    workerThread.wait(); // 等待线程结束
}
#include "imgprcs.h"

void imgProcessor::onImageReceived(const QString &filepath)
{
    this->origin_img = cv::imread(filepath.toStdString());

    pretreat();

    hsegment();

    QFile::remove(this->pretreated_image_path);

    for (int i = 0;; i++)
    {
        QString hsegpath = this->hseg_image_path_format.arg(i);
        if (!QFile::exists(hsegpath))
            break;
        Controller *controller = new Controller;
        emit controller->Operate(hsegpath, 7, "01234567");

        QFile logFile("NMN.log");
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << QString("working on line %1\n").arg(i + 1);
            logFile.close();
        }
    }

    return;
}

void imgProcessor::pretreat()
{
    cv::Mat gray_image;
    cv::cvtColor(this->origin_img, gray_image, cv::COLOR_RGB2GRAY);

    cv::Mat blur_image;
    cv::GaussianBlur(gray_image, blur_image, cv::Size(3, 3), 0.5);

    cv::Mat bin_image;
    int actualBlockSize = 21;
    double C_value = 12;
    cv::adaptiveThreshold(blur_image, bin_image, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, actualBlockSize, C_value);

    this->pretreated_image = bin_image;

    if (!cv::imwrite(pretreated_image_path.toStdString(), pretreated_image))
        QMessageBox::critical(nullptr, "Error", "Failed to Save");

    {
        QFile logFile("NMN.log");
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << QString("pretreat finished: %1 by %2\n").arg(this->pretreated_image.rows).arg(this->pretreated_image.cols);
            logFile.close();
        }
    }
}

void imgProcessor::hsegment()
{
    int rows = this->pretreated_image.rows, cols = this->pretreated_image.cols;
    cv::Mat hproj;
    hproj.create(rows, 1, CV_32SC1);

    for (int i = 0; i < rows; i++)
        hproj.at<int>(i, 0) = cv::countNonZero(this->pretreated_image.row(i));

    QList<lineH> lineh_list;
    for (int i = 0; i < rows; i++)
    {
        int j = 0;
        while (i + j < rows and hproj.at<int>(i + j, 0) > cols * 0.15)
            j++;
        if (j > 5)
        {
            lineH lineh = {.top = qMax(0, i - j / 4), .height = j / 2 * 3};
            lineh_list.append(lineh);
        }
        i += j;
    }

    for (int i = 0; i < lineh_list.length(); i++)
    {
        lineH lineh = lineh_list[i];
        cv::imwrite(this->hseg_image_path_format.arg(i).toStdString(),
                    this->pretreated_image.rowRange(lineh.top, lineh.top + lineh.height));
    }

    /*
    cv::Mat marked_image;
    cv::cvtColor(this->pretreated_image, marked_image, cv::COLOR_GRAY2BGR);
    QString hseg_min;
    for (lineH const &lineh : lineh_list)
    {
        cv::rectangle(marked_image, cv::Rect(cols / 100, lineh.top, cols - cols / 100, lineh.height), cv::Scalar(0, 0, 255));
        int mi = cols;
        for (int i = 0; i < lineh.height; i++)
            mi = qMin(mi, hproj.at<int>(lineh.top + i, 0));
        hseg_min += QString::number(mi) + " ";
    }

    {
        QFile logFile("NMN.log");
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << "hseg_line: ";
            for (const lineH &lineh : lineh_list)
                out << lineh.height << " ";
            out << "\n";
            out << "hseg_min: " << hseg_min << "\n";
            logFile.close();
        }
    }

    cv::imshow("mark", marked_image);
    */

    QFile logFile("NMN.log");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&logFile);
        out << "hseg finished: " << lineh_list.length() << "lines\n";
        logFile.close();
    }
}

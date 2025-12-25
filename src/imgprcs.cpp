#include "imgprcs.h"

void ImgProcessor::onImageReceived(const QString &filepath)
{
    this->origin_img = cv::imread(filepath.toStdString());
    this->origin_img_path = filepath;
    if (this->origin_img.empty())
    {
        QMessageBox::critical(nullptr, "Error", "cannot read image");
        return;
    }
    this->rows = origin_img.rows, this->cols = origin_img.cols;

    pretreat();

    hsegment();

    vsegment();

    ocr();

    return;
}

void ImgProcessor::pretreat()
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

    {
        QFile logFile("NMN.log");
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << QString("pretreat finished: %1 by %2\n").arg(this->rows).arg(this->cols);
            logFile.close();
        }
    }
}

void ImgProcessor::hsegment()
{
    cv::Mat hproj;
    hproj.create(rows, 1, CV_32SC1);

    for (int i = 0; i < rows; i++)
        hproj.at<int>(i, 0) = cv::countNonZero(this->pretreated_image.row(i));
    for (int i = 0; i < rows; i++)
    {
        int j = 0;
        while (i + j < rows and hproj.at<int>(i + j, 0) > cols * 0.15)
            j++;
        if (j > 5)
        {
            lineH lineh = {.top = qMax(0, i - j / 4), .height = qMin(j / 2 * 3, rows - i)};
            this->lineh_list.append(lineh);
        }
        i += j;
    }

    /*
    cv::Mat hproj_vis;
    hproj_vis.create(rows, cols / 10, CV_8UC1);

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < hproj.at<int>(i, 0) / 10; j++)
            hproj_vis.at<unsigned char>(i, j) = 255;

    cv::imshow("hproj", hproj_vis);
    */

    /*
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

    QFile logFile("NMN.log");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&logFile);
        out << "hseg finished: " << this->lineh_list.length() << "lines\n";
        logFile.close();
    }
}

void ImgProcessor::vsegment()
{
    for (const lineH &lineh : lineh_list)
    {
        cv::Mat vproj;
        vproj.create(1, this->cols, CV_32SC1);

        for (int i = 0; i < cols; i++)
            vproj.at<int>(0, i) = cv::countNonZero(this->pretreated_image.rowRange(lineh.top, lineh.top + lineh.height).col(i));
        for (int i = 0; i < cols; i++)
        {
            int j = 0;
            while (i + j < cols and vproj.at<int>(0, i + j) > lineh.height * 0.2)
                j++;
            if (j > 5)
            {
                this->rect_list.append(QRect(qMax(0, i - j / 4), lineh.top, qMin(cols, j * 3 / 2), lineh.height));
            }
            i += j;
        }
    }

    /*
   cv::Mat marked_image;
   cv::cvtColor(this->pretreated_image, marked_image, cv::COLOR_GRAY2BGR);
   for (const QRect &rect : this->rect_list)
       cv::rectangle(marked_image, cv::Rect(rect.x(), rect.y(), rect.width(), rect.height()), cv::Scalar(255, 0, 0));

   cv::imshow("mark", marked_image);
   */

    QFile logFile("NMN.log");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&logFile);
        out << "vseg finished: " << this->rect_list.length() << "rects\n";
        logFile.close();
    }
}

void ImgProcessor::ocr()
{
    Controller *controller = new Controller;
    connect(controller, &Controller::ResultReady, this, &ImgProcessor::onResultReceived);
    emit controller->Operate(this->pretreated_image, rect_list, "01234567");
}

void ImgProcessor::onResultReceived(const QList<QString> &result)
{
    for (int i = 0; i < result.length(); i++)
    {
        const QString s = result.at(i);
        if (s != "")
            for (int j = 0; j < s.length(); j++)
            {
                const QChar ch = s.at(j);
                if (ch == '0')
                    continue;
                Note note;
                note.noteName = static_cast<Note::NoteName>(ch.digitValue() - 1);
                note.accidental = Note::Accidental::NATURAL;
                note.octDot = Note::OctDot::NONE;
                note.rect = QRect(this->rect_list.at(i).x() + this->rect_list.at(i).width() / s.length() * j,
                                  this->rect_list.at(i).top(),
                                  this->rect_list.at(i).width() / s.length(),
                                  this->rect_list.at(i).height());
                this->note_list.append(note);
            }
    }

    {
        QFile logFile("NMN.log");
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << QString("found %1 notes:").arg(this->note_list.length());
            for (const Note &note : this->note_list)
                out << " " << QString::number(static_cast<int>(note.noteName) + 1);
            out << "\n";
        }
        logFile.close();
    }

    emit ImageProcessed(QPixmap(this->origin_img_path), this->note_list);
}

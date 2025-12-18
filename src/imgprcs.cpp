#include "imgprcs.h"

void Imgprcs::onImageReceived(const QString &filepath)
{
    this->img = cv::imread(filepath.toStdString());

    pretreat();

    Controller *controller = new Controller;
    connect(controller, &Controller::resultReady, this, &Imgprcs::onOCRReceicved);
    emit controller->operate(pretreated_image_path);

    return;
}

void Imgprcs::pretreat()
{
    cv::Mat gray_image;
    cv::cvtColor(this->img, gray_image, cv::COLOR_RGB2GRAY);

    cv::Mat blur_image;
    cv::GaussianBlur(gray_image, blur_image, cv::Size(3, 3), 0.5);

    cv::Mat bin_image;
    int actualBlockSize = 21;
    double C_value = 12;
    cv::adaptiveThreshold(blur_image, bin_image, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, actualBlockSize, C_value);

    pretreated_image = bin_image;
    if (!cv::imwrite(pretreated_image_path.toStdString(), pretreated_image))
        QMessageBox::about(nullptr, "Error", "Failed to Save");
}

void Imgprcs::onOCRReceicved(const QString &result)
{
    QFile::remove(pretreated_image_path);

    {
        QFile logFile("NMN.log");
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << "OCR result" << result << "\n";
            logFile.close();
        }
    }

    
}

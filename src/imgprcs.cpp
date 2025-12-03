#include "imgprcs.h"

void Imgprcs::onImageReceived(const QString filepath)
{
    this->img = cv::imread(filepath.toStdString());
    pretreat();
    locateElements();
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
    if (!cv::imwrite("build/assets/pretreated_image.png", pretreated_image))
        QMessageBox::about(nullptr, "Error", "Failed to Save");
}

void Imgprcs::locateElements()
{
    FILE *fp = fopen("build/assets/output.txt", "w");
    const char *datapath = "D:/Tesseract-OCR/tessdata/";
    const char *language = "eng";
    const char *char_whitelist = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int oem = cv::text::OEM_TESSERACT_ONLY;
    int psmode = cv::text::PSM_AUTO;
    cv::Ptr<cv::text::OCRTesseract> ocr = cv::text::OCRTesseract::create(datapath, language, char_whitelist, oem, psmode);

    std::string output;
    std::vector<cv::Rect> boxes;
    std::vector<std::string> words;
    std::vector<float> confidences;

    cv::Mat testImg = cv::imread("testImg.png");

    ocr->run(testImg, output, &boxes, &words, &confidences, cv::text::OCR_LEVEL_WORD);

    fprintf(fp, "OCR output = \"%s\" length = %d", output.c_str(), output.size());
    fclose(fp);
}

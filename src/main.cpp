#include "core/Score.h"
#include "ui/MainWindow.h"
#include "util/Logger.h"

#include <QApplication>
#include <QFile>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // core::Score 要跨线程通过信号传递，必须先注册元类型
    qRegisterMetaType<core::Score>("core::Score");

    util::Logger::beginSession();

    MainWindow window;
    window.show();

    // 便于快速验证：工作目录下有 test.jpg 时自动识别一次
    if (QFile::exists(QStringLiteral("test.jpg")))
        window.openImage(QStringLiteral("test.jpg"));

    return app.exec();
}

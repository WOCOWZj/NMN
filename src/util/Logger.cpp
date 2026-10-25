#include "util/Logger.h"

#include <QDateTime>
#include <QFile>
#include <QTextStream>

namespace util {

namespace {
constexpr char kLogFileName[] = "NMN.log";
}

bool Logger::s_enabled = true;

void Logger::setEnabled(bool enabled)
{
    s_enabled = enabled;
}

bool Logger::isEnabled()
{
    return s_enabled;
}

void Logger::beginSession()
{
    if (!s_enabled)
        return;

    QFile file(QString::fromLatin1(kLogFileName));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << "\n-----------------------\n"
        << QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss")) << "\n";
}

void Logger::info(const QString& message)
{
    write(QStringLiteral("INFO"), message);
}

void Logger::warn(const QString& message)
{
    write(QStringLiteral("WARN"), message);
}

void Logger::write(const QString& level, const QString& message)
{
    if (!s_enabled)
        return;

    QFile file(QString::fromLatin1(kLogFileName));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << '[' << level << "] " << message << '\n';
}

} // namespace util

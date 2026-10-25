#pragma once

#include <QString>

namespace util {

/// 统一的追加式日志。取代原先散落在 5 个文件里的 13 份手写 QFile 样板。
class Logger
{
public:
    /// 在日志开头写一条会话分隔线（含时间戳）
    static void beginSession();

    static void info(const QString& message);
    static void warn(const QString& message);

    static void setEnabled(bool enabled);
    static bool isEnabled();

private:
    static void write(const QString& level, const QString& message);

    static bool s_enabled;
};

} // namespace util

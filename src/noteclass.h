#pragma once // 预编译指令，确保头文件只被包含一次

#include <QRect>   // Qt矩形类，用于定义音符在图像中的位置
#include <QString> // Qt字符串类

// Note结构体：表示一个音符及其属性
struct Note
{
    QRect rect = QRect(); // 音符在图像中的矩形位置

    // 音名枚举：表示音符的基本音高（Do到Ti）
    enum class NoteName
    {
        Do,  // 1
        Re,  // 2
        Mi,  // 3
        Fa,  // 4
        Sol, // 5
        La,  // 6
        Ti   // 7
    };

    // 变音记号枚举：表示升降调符号
    enum class Accidental
    {
        SHARP,   // 升号(#)
        NATURAL, // 自然音(无符号)
        FLAT     // 降号(b)
    };

    // 八度点枚举：表示音符的八度位置
    enum class OctDot
    {
        LOWER, // 低八度
        NONE,  // 中八度
        UPPER  // 高八度
    };

    NoteName noteName = NoteName::Do;            // 音符名称，默认为Do
    Accidental accidental = Accidental::NATURAL; // 变音记号，默认为自然音
    OctDot octDot = OctDot::NONE;                // 八度位置，默认为中八度

    // Note2QString方法：将音符转换为字符串表示
    QString Note2QString() const
    {
        QString res; // 结果字符串

        // 根据音名设置基础数字
        switch (this->noteName)
        {
        case NoteName::Do:
            res = "1";
            break;
        case NoteName::Re:
            res = "2";
            break;
        case NoteName::Mi:
            res = "3";
            break;
        case NoteName::Fa:
            res = "4";
            break;
        case NoteName::Sol:
            res = "5";
            break;
        case NoteName::La:
            res = "6";
            break;
        case NoteName::Ti:
            res = "7";
            break;
        }

        // 根据变音记号添加符号
        switch (this->accidental)
        {
        case Accidental::SHARP:
            res += "#"; // 添加升号
            break;
        case Accidental::FLAT:
            res += "b"; // 添加降号
            break;
        case Accidental::NATURAL:
            res += " "; // 自然音添加空格
            break;
        }

        // 根据八度位置添加前缀
        switch (this->octDot)
        {
        case OctDot::UPPER:
            res = "`" + res; // 高八度前缀
            break;
        case OctDot::LOWER:
            res = "." + res; // 低八度前缀
            break;
        case OctDot::NONE:
            res = " " + res; // 中八度前缀
            break;
        }
        return res; // 返回格式化的音符字符串
    }

    // Note2int方法：将音符转换为整数表示（用于音高计算）
    int Note2int() const
    {
        int res = 0; // 结果值

        // 根据音名设置基础半音值
        switch (this->noteName)
        {
        case NoteName::Do:
            res = 0; // Do = 0
            break;
        case NoteName::Re:
            res = 2; // Re = 2
            break;
        case NoteName::Mi:
            res = 4; // Mi = 4
            break;
        case NoteName::Fa:
            res = 5; // Fa = 5
            break;
        case NoteName::Sol:
            res = 7; // Sol = 7
            break;
        case NoteName::La:
            res = 9; // La = 9
            break;
        case NoteName::Ti:
            res = 11; // Ti = 11
            break;
        }

        // 根据变音记号调整半音值
        switch (this->accidental)
        {
        case Accidental::SHARP:
            res++; // 升半音
            break;
        case Accidental::FLAT:
            res--; // 降半音
            break;
        case Accidental::NATURAL:
            break; // 无变化
        }

        // 根据八度位置调整值
        switch (this->octDot)
        {
        case OctDot::UPPER:
            res += 12; // 高八度加12
            break;
        case OctDot::LOWER:
            res -= 12; // 低八度减12
            break;
        case OctDot::NONE:
            break; // 中八度无变化
        }
        return res; // 返回音符的整数值
    }

    // int2Note静态方法：将整数转换回Note结构
    static Note int2Note(int number)
    {
        Note res = Note(); // 创建结果Note对象

        // 计算音名和变音记号（取模运算确保在0-11范围内）
        switch ((number % 12 + 12) % 12)
        {
        case 0:
            res.noteName = NoteName::Do;
            res.accidental = Accidental::NATURAL;
            break;
        case 1:
            res.noteName = NoteName::Do;
            res.accidental = Accidental::SHARP;
            break;
        case 2:
            res.noteName = NoteName::Re;
            res.accidental = Accidental::NATURAL;
            break;
        case 3:
            res.noteName = NoteName::Re;
            res.accidental = Accidental::SHARP;
            break;
        case 4:
            res.noteName = NoteName::Mi;
            res.accidental = Accidental::NATURAL;
            break;
        case 5:
            res.noteName = NoteName::Fa;
            res.accidental = Accidental::NATURAL;
            break;
        case 6:
            res.noteName = NoteName::Fa;
            res.accidental = Accidental::SHARP;
            break;
        case 7:
            res.noteName = NoteName::Sol;
            res.accidental = Accidental::NATURAL;
            break;
        case 8:
            res.noteName = NoteName::Sol;
            res.accidental = Accidental::SHARP;
            break;
        case 9:
            res.noteName = NoteName::La;
            res.accidental = Accidental::NATURAL;
            break;
        case 10:
            res.noteName = NoteName::La;
            res.accidental = Accidental::SHARP;
            break;
        case 11:
            res.noteName = NoteName::Ti;
            res.accidental = Accidental::NATURAL;
            break;
        }

        // 根据数值除以12的结果确定八度位置
        switch (static_cast<int>(floor(number / 12.0)))
        {
        case -1:
            res.octDot = OctDot::LOWER; // 低八度
            break;
        case 0:
            res.octDot = OctDot::NONE; // 中八度
            break;
        case 1:
            res.octDot = OctDot::UPPER; // 高八度
            break;
        }
        return res; // 返回转换后的Note对象
    }
};
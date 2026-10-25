#pragma once

#include <QList>
#include <QMetaType>

#include "core/Note.h"

namespace core {

/// 一份乐谱：音符集合 + 调号。
/// 全流程唯一的数据源，取代原先 ImgProcessor / HighLight / Converter 各存一份的局面。
class Score
{
public:
    const QList<Note>& notes() const { return m_notes; }
    int size() const { return static_cast<int>(m_notes.size()); }
    bool isEmpty() const { return m_notes.isEmpty(); }
    const Note& at(int index) const { return m_notes.at(index); }

    void setNotes(const QList<Note>& notes) { m_notes = notes; }
    void append(const Note& note) { m_notes.append(note); }
    void replace(int index, const Note& note) { m_notes.replace(index, note); }
    void removeAt(int index) { m_notes.removeAt(index); }

    Note tonic() const { return m_tonic; }
    void setTonic(const Note& tonic) { m_tonic = tonic; }

    /// 以 tonic 为 1(Do) 重算每个音的绝对音高，rect 保持不变
    void transposeTo(const Note& tonic);

private:
    QList<Note> m_notes;
    Note m_tonic;
};

} // namespace core

Q_DECLARE_METATYPE(core::Score)

#include "core/Score.h"

namespace core {

void Score::transposeTo(const Note& tonic)
{
    const int offset = tonic.toSemitone();
    m_tonic = tonic;

    for (Note& note : m_notes)
    {
        Note shifted = Note::fromSemitone(offset + note.toSemitone());
        shifted.rect = note.rect;
        note = shifted;
    }
}

} // namespace core

#include "ChordEngine.h"

#include <algorithm>
#include <iterator>

namespace chordcraft
{
namespace
{
constexpr std::array naturalMinorOffsets { 0, 2, 3, 5, 7, 8, 10 };
constexpr std::array degreesByPitchClass { 1, -1, 2, -1, 3, 4, -1, 5, -1, 6, -1, 7 };

struct Voicing
{
    // En el modo Abierto, la tercera, la séptima y las extensiones suben una octava de forma predeterminada.
    template <std::size_t Size>
    constexpr Voicing (const int (&pitches)[Size], unsigned lifted = 0b1111010) noexcept
        : intervalCount (Size), liftedVoices (lifted)
    {
        static_assert (Size <= maxVoices);
        for (std::size_t i = 0; i < Size; ++i)
            intervals[i] = pitches[i];
    }

    // Guarda los intervalos por valor. MSVC puede poner a cero los arreglos subyacentes
    // de las listas de inicialización anidadas en tablas constexpr.
    static constexpr std::size_t maxVoices = 7;
    std::array<int, maxVoices> intervals {};
    std::size_t intervalCount;
    unsigned liftedVoices;
};

constexpr Voicing major { { 0, 4, 7 } };
constexpr Voicing minor { { 0, 3, 7 } };
constexpr Voicing diminished { { 0, 3, 6 } };
constexpr Voicing major7 { { 0, 4, 7, 11 } };
constexpr Voicing minor7 { { 0, 3, 7, 10 } };
constexpr Voicing dominant7 { { 0, 4, 7, 10 } };
constexpr Voicing halfDiminished { { 0, 3, 6, 10 } };
constexpr Voicing major9 { { 0, 4, 7, 11, 14 } };
constexpr Voicing minor9 { { 0, 3, 7, 10, 14 } };
constexpr Voicing dominant9 { { 0, 4, 7, 10, 14 } };
constexpr Voicing minor11 { { 0, 3, 7, 10, 14, 17 } };
constexpr Voicing major9Sharp11 { { 0, 4, 7, 11, 14, 18 } };
constexpr Voicing halfDiminished11 { { 0, 3, 6, 10, 17 } };
constexpr Voicing major13 { { 0, 4, 7, 11, 14, 21 } };
constexpr Voicing dominant13 { { 0, 4, 7, 10, 14, 21 } };
constexpr Voicing minor13 { { 0, 3, 7, 10, 14, 17, 21 } };
constexpr Voicing minor9Add11Add6 { { 0, 3, 7, 9, 10, 14, 17 }, 0b1110010 };
constexpr Voicing major6Add9 { { 0, 4, 7, 9, 14 }, 0b10010 };
constexpr Voicing flatMajor7OverBass { { 0, 10, 14, 17, 21 }, 0b10100 }; // Fmaj7/G en do mayor.
constexpr Voicing dominantFlat9 { { 0, 4, 7, 10, 13 } };
constexpr Voicing dominantFlat13 { { 0, 4, 7, 10, 20 } };

// Usa la novena diatónica sobre el grado iii en el modo mayor y el grado v en el menor natural.
constexpr Voicing minorFlat9 { { 0, 3, 7, 10, 13 } };
constexpr Voicing minor11Without9 { { 0, 3, 7, 10, 17 } };
constexpr Voicing minorFlat9Add11 { { 0, 3, 7, 10, 13, 17 } };

constexpr Voicing majorVoicings[7][5] {
    { major,      major7,     major9,         major9,             major13 },
    { minor,      minor7,     minor9,         minor11,            minor9Add11Add6 },
    { minor,      minor7,     minor7,         minor11Without9,    minorFlat9Add11 },
    { major,      major7,     major9,         major6Add9,         major9Sharp11 },
    { major,      dominant7,  dominant9,      flatMajor7OverBass, dominant13 },
    { minor,      minor7,     minor9,         minor11,            dominantFlat13 },
    { diminished, diminished, halfDiminished, halfDiminished,    halfDiminished }
};

constexpr Voicing minorVoicings[7][5] {
    { minor,      minor7,         minor9,         minor9,            minor11 },
    { diminished, halfDiminished, halfDiminished, halfDiminished11,  halfDiminished11 },
    { major,      major7,         major9,         major6Add9,        major13 },
    { minor,      minor7,         minor9,         minor11,           minor13 },
    { minor,      minor7,         minorFlat9,     dominantFlat9,     dominantFlat13 },
    { major,      major7,         major9,         major9Sharp11,     major13 },
    { major,      dominant7,      dominant9,      dominant13,        dominant13 }
};
}

int degreeForMidiNote (int midiNote) noexcept
{
    if (midiNote < 0 || midiNote > 127)
        return -1;

    return degreesByPitchClass[static_cast<std::size_t> (midiNote % 12)];
}

Chord makeChord (int inputNote, int base, ScaleMode mode, int complexity, Amplitude amplitude) noexcept
{
    Chord chord;
    const auto degree = degreeForMidiNote (inputNote);
    if (degree < 0 || base < 0 || base >= 12 || complexity < 1 || complexity > 5)
        return chord;

    const auto isMajor = mode == ScaleMode::major;
    const auto& scale = isMajor ? whiteKeyOffsets : naturalMinorOffsets;
    const auto& voicings = isMajor ? majorVoicings : minorVoicings;
    const auto root = (inputNote / 12) * 12 + base + scale[static_cast<std::size_t> (degree - 1)];
    const auto& voicing = voicings[degree - 1][complexity - 1];
    const auto spreadVoices = amplitude == Amplitude::open || amplitude == Amplitude::wide;

    for (std::size_t voice = 0; voice < voicing.intervalCount; ++voice)
    {
        const auto interval = voicing.intervals[voice];
        auto note = root + interval;
        if (amplitude == Amplitude::cluster)
            note = root + interval % 12;
        else if (spreadVoices && (voicing.liftedVoices & (1u << voice)) != 0)
            note += 12;
        else if (amplitude == Amplitude::wide && voice == 0 && root >= 12)
            note -= 12;

        if (note <= 127)
            chord.notes[chord.size++] = note;
    }

    const auto first = chord.notes.begin();
    const auto last = first + static_cast<std::ptrdiff_t> (chord.size);
    std::sort (first, last);
    chord.size = static_cast<std::size_t> (std::distance (first, std::unique (first, last)));
    return chord;
}
}

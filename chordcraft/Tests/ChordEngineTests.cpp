#include "../Source/ChordEngine.h"

#include <algorithm>
#include <initializer_list>
#include <iostream>

namespace
{
bool checkChord (const char* description, const chordcraft::Chord& actual, std::initializer_list<int> expected)
{
    if (std::equal (actual.begin(), actual.end(), expected.begin(), expected.end()))
        return true;

    std::cerr << description << " expected";
    for (const auto note : expected)
        std::cerr << ' ' << note;

    std::cerr << ", got";
    for (const auto note : actual)
        std::cerr << ' ' << note;

    std::cerr << '\n';
    return false;
}
}

int main()
{
    using chordcraft::Amplitude;
    using chordcraft::makeChord;
    using chordcraft::ScaleMode;

    bool passed = checkChord ("C major, closed, level 1",
                              makeChord (60, 0, ScaleMode::major, 1, Amplitude::closed), { 60, 64, 67 });
    passed &= checkChord ("C major, closed, level 5",
                          makeChord (60, 0, ScaleMode::major, 5, Amplitude::closed), { 60, 64, 67, 71, 74, 81 });
    passed &= checkChord ("C major, cluster, level 5",
                          makeChord (60, 0, ScaleMode::major, 5, Amplitude::cluster), { 60, 62, 64, 67, 69, 71 });
    passed &= checkChord ("C major, open, level 1",
                          makeChord (60, 0, ScaleMode::major, 1, Amplitude::open), { 60, 67, 76 });
    passed &= checkChord ("C major, wide, level 1",
                          makeChord (60, 0, ScaleMode::major, 1, Amplitude::wide), { 48, 67, 76 });
    passed &= checkChord ("D minor, closed, level 1, transposed from C",
                          makeChord (60, 2, ScaleMode::naturalMinor, 1, Amplitude::closed), { 62, 65, 69 });
    passed &= checkChord ("Second degree in C major, open, level 5",
                          makeChord (62, 0, ScaleMode::major, 5, Amplitude::open), { 62, 69, 71, 77, 84, 88, 91 });

    return passed ? 0 : 1;
}

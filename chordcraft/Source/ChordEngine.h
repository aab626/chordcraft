#pragma once

#include <array>
#include <cstddef>

namespace chordcraft
{
inline constexpr std::array noteNames { "Do", "Do#", "Re", "Re#", "Mi", "Fa",
                                        "Fa#", "Sol", "Sol#", "La", "La#", "Si" };
inline constexpr std::array modeNames { "Mayor", "Menor" };
inline constexpr std::array amplitudeNames { "Cluster", "Cerrado", "Abierto", "Amplio" };
inline constexpr std::array whiteKeyOffsets { 0, 2, 4, 5, 7, 9, 11 };

enum class ScaleMode { major, naturalMinor };
enum class Amplitude { cluster, closed, open, wide };

struct Chord
{
    std::array<int, 128> notes {};
    std::size_t size = 0;

    auto begin() const noexcept { return notes.begin(); }
    auto end() const noexcept { return notes.begin() + static_cast<std::ptrdiff_t> (size); }
};

// White keys map to 1..7. Other notes return -1.
int degreeForMidiNote (int midiNote) noexcept;

Chord makeChord (int inputNote, int base, ScaleMode mode, int complexity, Amplitude amplitude) noexcept;
} // namespace chordcraft

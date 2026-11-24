# ChordCraft ⛏

Plugin VST3 para generar acordes a partir de una nota de entrada. Desarrollado como plugin JUCE como proyecto del curso Seminario de Diseño e Innovación Tecnológica EL4030 de la FCFM, Universidad de Chile.

<p align="center">
  <img src="docs/assets/gui.png" alt="Interfaz de ChordCraft">
</p>

## Estructura del repositorio

```text
├── chordcraft/
│   ├── .gitattributes
│   ├── .gitignore
│   ├── ChordCraft.jucer           # Proyecto Projucer
│   ├── Source/
│   │   ├── ChordEngine.cpp        # Cálculo de las notas del acorde
│   │   ├── ChordEngine.h
│   │   ├── PluginEditor.cpp       # Interfaz y controles
│   │   ├── PluginEditor.h
│   │   ├── PluginProcessor.cpp    # Procesamiento MIDI y estado del plugin
│   │   └── PluginProcessor.h
│   └── Tests/
│       └── ChordEngineTests.cpp   # Pruebas del motor de acordes
├── docs/
│   ├── build.md
│   └── install.md
└── CMakeLists.txt                 # Configuración de CMake
```

## Documentación

- [Guía de uso]()
- [Guía de desarrollo](docs/desarrollo.md)

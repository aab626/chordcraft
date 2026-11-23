# Compilar ChordCraft

El proyecto genera **VST3** y **Standalone** en Windows, Linux y macOS. Compila en el sistema para el que quieras generar el binario. [Requisitos de instalación](install.md).

## Generar los archivos de compilación

1. Abre Projucer desde la carpeta de JUCE `9.0.6`. En **File → Global Paths…** (**Projucer → Global Paths…** en macOS), configura **JUCE Modules** con la ruta a la carpeta `modules` extraída. En Windows puede ser `C:\dev\JUCE\modules`.
2. Abre `NewProject/NewProject.jucer` desde el clon y guárdalo con **File → Save Project**. Los tres exportadores ya están configurados en ese archivo.
3. Sigue los pasos de tu plataforma. Las rutas de abajo parten de la raíz del repo. Vuelve a guardar el `.jucer` cuando cambie la configuración del proyecto. Projucer vuelve a generar `NewProject/Builds/` y `NewProject/JuceLibraryCode/`, que están ignorados por Git.

Usa `Release` para probar el plugin en un DAW y `Debug` para depurar. Los ejemplos usan `Release`.

## Windows x64 desde un clon limpio

1. Instala Visual Studio **2022** con C++, MSVC v143 x64/x86 y el SDK de Windows. Extrae el ZIP de JUCE para Windows. Para la prueba del VST3, instala REAPER x64. [Instalación del entorno](install.md#windows).
2. Ejecuta `Projucer.exe`, configura **JUCE Modules** y guarda el `.jucer` como se indica arriba. Abre `NewProject/Builds/VisualStudio2022/ChordCraft.sln`.
3. En Visual Studio selecciona **Release** y **x64**, y ejecuta **Build → Build Solution** (`Ctrl+Shift+B`). También puedes compilar desde **x64 Native Tools Command Prompt for VS 2022**, en la raíz del repo.

   ```bat
   msbuild "NewProject\Builds\VisualStudio2022\ChordCraft.sln" /m /p:Configuration=Release /p:Platform=x64
   ```

4. Los resultados quedan en `NewProject/Builds/VisualStudio2022/x64/Release/`.
   - `VST3/ChordCraft.vst3/` es la carpeta completa del plugin. Cópiala a `C:\Program Files\Common Files\VST3\` para el escaneo estándar de REAPER. Windows puede pedir permisos para escribir ahí. También puedes añadir `VST3/` de la compilación a las rutas de escaneo de REAPER.
   - `Standalone Plugin/ChordCraft.exe` sirve para abrir la interfaz sin un DAW.

Si Visual Studio indica que falta `v143` o el SDK, agrégalos desde **Visual Studio Installer → Modificar**. La solución también compila el generador de manifiesto VST3 para Win32, de ahí el requisito de herramientas x64/x86.

## Linux

Después de instalar GCC, Make y las bibliotecas de desarrollo, guarda el `.jucer` en Projucer y ejecuta este comando desde la raíz del repo. [Dependencias de Linux](install.md#linux).

```bash
make -C NewProject/Builds/LinuxMakefile CONFIG=Release -j2
```

En `NewProject/Builds/LinuxMakefile/build/` quedan `ChordCraft.vst3/` y el ejecutable `ChordCraft`. El Makefile también copia el VST3 a `~/.vst3/` por defecto. Cierra el DAW antes de recompilar o sustituir un plugin que tenga cargado.

Si `pkg-config` viene de Homebrew y Make no encuentra `alsa.pc` pese a tener instalado `libasound2-dev`, agrega `PKG_CONFIG=/usr/bin/pkg-config` al comando de compilación en Ubuntu.

## macOS

Después de configurar Xcode, guarda el `.jucer` en Projucer y abre `NewProject/Builds/MacOSX/ChordCraft.xcodeproj`. [Instalación de Xcode](install.md#macos).

Puedes compilar todos los targets con este comando desde la raíz del repo.

```bash
xcodebuild -project NewProject/Builds/MacOSX/ChordCraft.xcodeproj \
  -target "ChordCraft - All" -configuration Release build
```

En `NewProject/Builds/MacOSX/build/Release/` quedan `ChordCraft.vst3` y `ChordCraft.app`. Copia el paquete completo `ChordCraft.vst3` a `~/Library/Audio/Plug-Ins/VST3/` y crea la carpeta si hace falta. El plugin debe incluir la arquitectura del DAW (`arm64` para Apple Silicon nativo o `x86_64` para Intel).

## Probar en REAPER

1. Abre **Options → Preferences → Plug-ins → VST**, comprueba que esté la ruta donde dejaste `ChordCraft.vst3` y vuelve a escanear.
2. En una pista con un ítem MIDI, inserta `VST3: ChordCraft` primero y un instrumento como `ReaSynth` después. ChordCraft genera notas MIDI y el instrumento las convierte en audio.
3. Con base **Do**, modo **Mayor**, amplitud **Cerrado** y complejidad **1**, reproduce la nota MIDI 60 (Do). Debería sonar un acorde de Do mayor. Cambia la complejidad para comprobar que cambia el acorde.

Si ChordCraft aparece en la lista pero no se oye el acorde, revisa el menú **MIDI output** del FX. Selecciona **Replaces MIDI bus (default)** para enviar el acorde al instrumento en la misma cadena.

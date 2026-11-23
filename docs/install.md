# Instalación del entorno de desarrollo

## Instalación del DAW

Para probar el VST3 usaremos REAPER. Si usas otro DAW, comprueba que admita la salida MIDI de efectos VST3. [Descargar REAPER](https://www.reaper.fm/download.php).

## JUCE y Projucer

Descarga el ZIP de JUCE `9.0.6` para tu sistema (`windows`, `linux` u `osx`) desde **Assets** y extrae la carpeta completa. El ZIP incluye Projucer, la carpeta `modules` y el SDK de VST3. [Descargar JUCE 9.0.6](https://github.com/juce-framework/JUCE/releases/tag/9.0.6).

Los módulos de JUCE se ofrecen bajo AGPLv3 o licencia comercial. Según el uso y la distribución del plugin, puede que necesites una licencia comercial. Revisa `LICENSE.md` de tu copia de JUCE.

## Requisitos de desarrollo

### Windows

Instala Visual Studio **2022** Community con la carga de trabajo **Desarrollo para el escritorio con C++** (_Desktop development with C++_). Verifica que incluya **MSVC v143 para x64/x86** y un **SDK de Windows 10 u 11**. [Descargar el instalador de Visual Studio 2022](https://aka.ms/vs/17/release/vs_community.exe).
Una vez instalados, en la terminal **x64 Native Tools Command Prompt for VS 2022** ejecuta estos comandos:

```bat
cl
where rc
```

Se debería observar la versión del compilador de Microsoft y la ruta a `rc.exe` en el SDK.

### macOS

Para [compilar con CMake](build.md#macos-con-cmake-sin-xcode-completo) no necesitas Xcode completo. Instala las Command Line Tools de Apple y CMake; el segundo comando requiere Homebrew. Si las Command Line Tools ya están instaladas, omite el primero.

```bash
xcode-select --install
brew install cmake
```

Comprueba que CMake y el compilador del SDK estén disponibles.

```bash
cmake --version
xcrun --sdk macosx --find clang++
```

Para usar el proyecto de Xcode generado por Projucer, instala Xcode completo y ábrelo una vez para aceptar la licencia y completar la configuración inicial. [Descargar Xcode](https://developer.apple.com/xcode/).
En Terminal, ejecuta estos comandos:

```bash
xcodebuild -version
xcrun --sdk macosx --find clang++
```

Se debería observar la versión de `Xcode` y la ruta al compilador del SDK.

### Linux

Tomando como referencia Ubuntu 24.04, instala GCC, `make` y las bibliotecas de desarrollo de ALSA, fuentes y X11. Para otras distribuciones, hay que buscar paquetes equivalentes.

```bash
sudo apt update
sudo apt install build-essential pkg-config libasound2-dev \
  libfontconfig1-dev libfreetype-dev libx11-dev libxcomposite-dev \
  libxcursor-dev libxext-dev libxi-dev libxinerama-dev libxrandr-dev libxrender-dev
```

Luego, comprueba que todo esté instalado con:

```bash
g++ --version
make --version
pkg-config --modversion alsa fontconfig freetype2 x11 xcomposite \
  xcursor xext xi xinerama xrandr xrender
```

Continúa con la [guía de compilación y prueba](build.md).

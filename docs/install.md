# Instalación del entorno de desarrollo

## Instalación del DAW

Usaremos REAPER para el desarrollo, [descargar desde acá](https://www.reaper.fm/download.php).
Sin embargo, cualquier DAW compatible con plugins JUCE debería ser compatible.

## JUCE y Projucer

Este proyecto usa JUCE `9.0.6`, descargar desde [GitHub](https://github.com/juce-framework/JUCE/releases/tag/9.0.6) el zip correspondiente a tu sistema.

## Requisitos de desarrollo

### Windows

Instala Visual Studio 2022 Community con el workload _Desktop development with C++_. [Descargar Visual Studio 2022](https://visualstudio.microsoft.com/downloads)

### macOS

Instala Xcode con y ábrelo una vez para completar la configuración inicial. [Descargar Xcode](https://developer.apple.com/xcode/)

### Linux

Tomando como referencia Ubuntu 24.04, instala GCC, `make` y las bibliotecas de desarrollo de ALSA, fuentes y X11. Para otras distribuciones, hay que buscar paquetes equivalentes.

```bash
sudo apt update
sudo apt install build-essential pkg-config libasound2-dev \
  libfontconfig1-dev libfreetype-dev libx11-dev libxcomposite-dev \
  libxcursor-dev libxext-dev libxi-dev libxinerama-dev libxrandr-dev libxrender-dev
```

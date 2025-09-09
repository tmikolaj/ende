# 🌀 Ende - A 3D Procedural Generator

[![Last Commit](https://img.shields.io/github/last-commit/tmikolaj/ende?style=flat-square)](https://github.com/tmikolaj/ende/commits/main)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](./LICENSE)
[![Stars](https://img.shields.io/github/stars/tmikolaj/ende?style=social)](https://github.com/tmikolaj/ende)

---

## 🌍 Overview

**Ende** is a real-time system for generating 3D geometry—currently focused on **terrain** and **rocks**—using procedural algorithms. It supports various noise functions and real-time editing with full customization via an intuitive UI. Perfect for **game development**, **simulation**, and **generative art** projects.
Below is a sample **terrain** made using ende.

![Demo](assets/readme/gifs/demo.gif)

## ✨ Features

- 🌄 **Procedural Terrain** generation
- 🧱 **Procedural Rocks** with noise-based variation
- 🎛️ **Customizable generation** via UI
- ⚙️ Support for:
    - Sine/Cosine patterning
    - Fractal Noise
    - Value Noise
    - Perlin Noise
- ⚡ **Real-time Editing** with immediate feedback
- 🚀 Export to .obj

## 🛠️ Built With

- C++
  - Raylib (graphics)
  - ImGui (ui)
  - glm (math)

## Demo & Screenshots

### Live editing
Adjust terrain parameters in real time — every change updates the 3D geometry instantly
![LiveUITweaking](assets/readme/gifs/uidemo.gif)

### Mesh painting
Paint directly onto the mesh: choose a solid color or a custom texture and stamp it onto the surface. Supports an "angle jitter" mode to vary stamp rotation for more organic results   
*Known quirk:* angle jitter can behave oddly (a single click may produce multiple jittered stamps)
![LivePainting](assets/readme/gifs/paintdemo.gif)

### Thermal Erosion
Simulate natural processes like weathering and sediment transport to smooth and reshape landscapes
![LiveThermalErosion](assets/readme/gifs/demothermalerosion.gif)

### Tooltips
Built-in contextual tooltips explain controls and parameters as you hover, making the UI approachable even for beginners
![LiveTooltips](assets/readme/gifs/demotooltips.gif)

## 🚀 Installation Quick Start

1. Download release
2. Unzip
3. Go to endeapp directory
4. Double-click the binary/executable
 ```bash
   tar xzf ende-v0.1-alpha-linux-x86_64.tar.gz
   cd linux/endeapp
   ./ende
   ```

> ⚠️ **Important:** Do not move the binary/executable after unzipping
> as it will break the path!

## 📅 Roadmap

- [ ] Add saving/loading
- [ ] Fix known bugs
- [ ] Improve installation
- [ ] Add undo/redo
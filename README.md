# OFP Vulkan Renderer

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/Platform-Windows-blue.svg)]()
[![Vulkan](https://img.shields.io/badge/Vulkan-1.3-green.svg)]()

> Modern Vulkan-based renderer for Operation Flashpoint, replacing the unstable D3D8 wrapper.

## 🚀 Quick Start (No Local Installation Required!)

### Option 1: GitHub Codespaces (Recommended)

1. Open this repository on GitHub
2. Click the green **"Code"** button
3. Select **"Create codespace"**
4. Wait for the environment to initialize (~5 minutes)
5. The complete development environment will be ready!

### Option 2: Local Development (Minimal Install)

Required tools (all free):
- [VS Code](https://code.visualstudio.com/) (~100MB)
- [GitHub Desktop](https://desktop.github.com/) (~150MB)
- [7-Zip](https://www.7-zip.org/) (~5MB)

**Total local storage: < 300MB** ✅

## 📦 Installation

### Download Pre-built DLL

1. Go to [Releases](https://github.com/jsshwqz/ofp-renderer/releases)
2. Download the latest `ofp_renderer.dll`
3. Copy to your OFP game directory
4. Rename or configure to load the DLL

### Build from Source (Cloud Compilation)

1. Fork this repository
2. Push your changes
3. GitHub Actions automatically builds
4. Download DLL from Actions artifacts

## 🔧 Configuration

Edit `ofp_renderer.ini`:

```ini
[Renderer]
EnableValidation=true
EnableVSync=false
EnableAnisotropy=true
AnisotropyLevel=16

[Effects]
EnablePostProcessing=true
EnableHardLight=false
EnableDesaturate=false
EnableGlare=false
```

## 🎮 Features

### Rendering Improvements
- ✅ Vulkan API for modern GPUs
- ✅ Anisotropic filtering (up to 16x)
- ✅ FOV control
- ✅ Performance optimization

### Post-Processing Effects
- ✅ Hard Light - Enhance contrast
- ✅ Desaturation - Film color grade
- ✅ Glare/Bloom - Realistic glow

### Stability Fixes
- ✅ No more screenshot black screen
- ✅ No more random crashes
- ✅ Better memory management

## 🏗️ Architecture

```
OFP Game (D3D8)
     │
     ▼
┌─────────────────────────────────────┐
│    D3D8 → Vulkan Bridge Layer       │
│  Translates D3D8 calls to Vulkan    │
└─────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────┐
│         Vulkan Renderer             │
│  - Instance/Device creation         │
│  - Swap chain management            │
│  - Graphics pipeline                │
└─────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────┐
│       Post-Processing               │
│  - Hard Light effect                │
│  - Desaturation effect              │
│  - Glare/Bloom effect               │
└─────────────────────────────────────┘
     │
     ▼
         Screen
```

## 💻 Development

### Cloud Development (Recommended)

```bash
# 1. Create codespace
# (Use GitHub web interface)

# 2. In codespace terminal:
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

# 3. Download artifacts from GitHub Actions
```

### Local Development

```bash
# 1. Clone repository
git clone https://github.com/jsshwqz/ofp-renderer.git
cd ofp-renderer

# 2. Open in VS Code
code .

# 3. Build with CMake
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### GitHub Actions CI/CD

The project uses GitHub Actions for:
- Automatic compilation (Windows + Linux)
- Testing
- Release creation

Workflow: `.github/workflows/build.yml`

## 📁 Project Structure

```
ofp-renderer/
├── .github/
│   └── workflows/
│       └── build.yml          # CI/CD pipeline
├── .devcontainer/
│   └── devcontainer.json      # GitHub Codespaces config
├── cmake/
│   ├── FindVulkan.cmake       # Vulkan finder
│   └── FindDirectX9.cmake     # DirectX finder
├── config/
│   └── ofp_renderer.ini.in    # Default config
├── include/
│   ├── config.h               # Configuration
│   ├── vulkan_renderer.h      # Core renderer
│   ├── d3d8_bridge.h          # D3D8 compatibility
│   └── post_processing.h      # Post-effects
├── src/
│   └── (implementation files)
├── shaders/
│   └── (GLSL/HLSL shaders)
├── CMakeLists.txt
└── README.md
```

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## 📝 License

MIT License - see [LICENSE](LICENSE) for details.

## 🙏 Acknowledgments

- Vulkan SDK from LunarG
- DirectX SDK from Microsoft
- Operation Flashpoint community

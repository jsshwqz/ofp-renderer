# OFP Renderer Development Scripts

This directory contains helpful scripts for development.

## Available Scripts

### setup.bat
Initializes the development environment and checks prerequisites.

```batch
scripts\setup.bat
```

### build.bat
Builds the project using Visual Studio.

```batch
scripts\build.bat
```

### clean.bat
Cleans build artifacts and temporary files.

```batch
scripts\clean.bat
```

### rebuild.bat
Cleans and rebuilds the project.

```batch
scripts\rebuild.bat
```

## Cloud Development

For cloud compilation, use GitHub Actions or GitHub Codespaces:

### GitHub Actions
1. Push changes to GitHub
2. Go to Actions tab
3. Download artifacts from workflow run

### GitHub Codespaces
1. Open repository on GitHub
2. Click "Code" → "Create codespace"
3. Run build commands in terminal

## Quick Reference

| Task | Command |
|------|---------|
| Setup | `scripts\setup.bat` |
| Build | `scripts\build.bat` |
| Clean | `scripts\clean.bat` |
| Rebuild | `scripts\rebuild.bat` |

## Requirements

- Windows 7+
- Visual Studio 2019+ or Build Tools
- CMake 3.16+
- Vulkan SDK (optional for local build)
- DirectX SDK (optional for local build)

For cloud development, no local installation required!

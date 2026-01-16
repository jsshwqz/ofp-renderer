# Contributing to OFP Vulkan Renderer

Thank you for considering contributing to this project! This document outlines the process for contributing.

## How to Contribute

### 1. Fork the Repository

1. Click the **Fork** button on GitHub
2. Clone your fork:
```bash
git clone https://github.com/YOUR_USERNAME/ofp-renderer.git
cd ofp-renderer
```

### 2. Create a Feature Branch

```bash
git checkout -b feature/amazing-new-feature
```

### 3. Make Your Changes

- Follow the existing code style
- Add comments for complex logic
- Test your changes

### 4. Commit Your Changes

```bash
git add .
git commit -m "Add amazing new feature"
```

### 5. Push to GitHub

```bash
git push origin feature/amazing-new-feature
```

### 6. Create a Pull Request

1. Go to your repository on GitHub
2. Click **Compare & pull request**
3. Fill in the PR template
4. Submit

## Development Setup

### Option 1: GitHub Codespaces (Recommended)

1. Open the repository on GitHub
2. Click **Code** → **Create codespace**
3. Wait for environment to initialize

### Option 2: Local Development

Required tools:
- VS Code
- GitHub Desktop
- CMake 3.16+

## Code Style

- Use C++17 standard
- 4 spaces for indentation
- Follow Windows API naming conventions
- Functions: PascalCase
- Variables: camelCase
- Classes: PascalCase
- Constants: UPPER_SNAKE_CASE

## Testing

Before submitting:
1. Ensure code compiles
2. Test with OFP game
3. Check for memory leaks

## Reporting Issues

When reporting issues, include:
- Operating system version
- GPU model and driver version
- Detailed error description
- Steps to reproduce

## License

Contributions are licensed under MIT License.

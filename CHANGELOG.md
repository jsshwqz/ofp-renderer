# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-01-16

### Added
- Initial release of OFP Vulkan Renderer
- Vulkan-based rendering core replacing D3D8
- D3D8 to Vulkan compatibility layer
- Post-processing effects system (Hard Light, Desaturation, Glare)
- Configuration management system
- GitHub Actions CI/CD pipeline
- GitHub Codespaces development environment
- CMake build system
- MIT License

### Features
- Vulkan instance and device creation
- Swap chain management
- Render pass and pipeline creation
- Command buffer management
- Synchronization primitives

### Post-Processing
- Hard Light effect with per-channel control
- Desaturation effect
- Glare/Bloom effect

### Configuration
- INI-based configuration file
- Renderer settings (VSync, Anisotropy)
- Effect settings
- Screenshot settings

### Development
- CMake-based build system
- GitHub Actions for Windows and Linux
- GitHub Codespaces support
- Comprehensive documentation

## [Unreleased]

### Planned
- Complete D3D8 API translation
- Additional post-processing effects
- Performance optimizations
- GUI configuration tool
- Screenshot functionality

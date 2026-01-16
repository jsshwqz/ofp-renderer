/**
 * @file config.h
 * @brief Configuration management for OFP Vulkan Renderer
 * 
 * This file handles loading and saving of configuration settings
 * for the renderer, including graphics options, effects, and performance.
 */

#ifndef OFP_RENDERER_CONFIG_H
#define OFP_RENDERER_CONFIG_H

#include <Windows.h>
#include <string>

namespace Config {

/**
 * @struct RendererSettings
 * @brief Graphics rendering configuration
 */
struct RendererSettings {
    bool enableValidation = true;           // Enable Vulkan validation layers
    bool enableVSync = false;               // Enable vertical synchronization
    bool enableAnisotropy = true;           // Enable anisotropic filtering
    UINT anisotropyLevel = 16;              // Anisotropy level (1-16)
    UINT width = 1920;                      // Window width
    UINT height = 1080;                     // Window height
    bool fullscreen = false;                // Fullscreen mode
};

/**
 * @struct EffectSettings
 * @brief Post-processing effects configuration
 */
struct EffectSettings {
    bool enablePostProcessing = true;       // Enable post-processing
    bool enableHardLight = false;           // Enable hard light effect
    bool enableDesaturate = false;          // Enable desaturation effect
    bool enableGlare = false;               // Enable glare/bloom effect
    
    float hardLightStrength = 0.4f;         // Hard light effect strength
    float desaturationStrength = 0.2f;      // Desaturation strength
    float glareStrength = 0.3f;             // Glare effect strength
    int glareSize = 3;                      // Glare size (1-8)
    bool glareDarkenSky = false;            // Darken sky with glare
};

/**
 * @struct PerformanceSettings
 * @brief Performance optimization configuration
 */
struct PerformanceSettings {
    bool enableAutoFallback = true;         // Auto-fallback on low performance
    bool enableLODBias = false;             // Enable LOD bias adjustment
    float LODBias0 = 0.0f;                  // Texture LOD bias
    float LODBias1 = 0.0f;                  // Multi-texture LOD bias
};

/**
 * @struct ScreenshotSettings
 * @brief Screenshot configuration
 */
struct ScreenshotSettings {
    bool enableScreenshots = true;          // Enable screenshots
    bool autoSave = true;                   // Auto-save screenshots
    std::wstring savePath = L".\\Screenshots";  // Screenshot save path
    std::wstring format = L"png";           // Screenshot format (png/bmp)
};

/**
 * @class ConfigManager
 * @brief Manages all configuration settings
 */
class ConfigManager {
public:
    static ConfigManager& GetInstance();
    
    bool Load(const std::wstring& filename = L"ofp_renderer.ini");
    bool Save(const std::wstring& filename = L"ofp_renderer.ini");
    void ResetToDefaults();
    
    // Getters
    const RendererSettings& GetRenderer() const { return m_Renderer; }
    const EffectSettings& GetEffects() const { return m_Effects; }
    const PerformanceSettings& GetPerformance() const { return m_Performance; }
    const ScreenshotSettings& GetScreenshot() const { return m_Screenshot; }
    
    // Setters
    RendererSettings& GetRenderer() { return m_Renderer; }
    EffectSettings& GetEffects() { return m_Effects; }
    PerformanceSettings& GetPerformance() { return m_Performance; }
    ScreenshotSettings& GetScreenshot() { return m_Screenshot; }
    
private:
    ConfigManager() = default;
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    RendererSettings m_Renderer;
    EffectSettings m_Effects;
    PerformanceSettings m_Performance;
    ScreenshotSettings m_Screenshot;
};

} // namespace Config

#endif // OFP_RENDERER_CONFIG_H

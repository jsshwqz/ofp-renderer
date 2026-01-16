# OFP Renderer API Documentation

## Core Classes

### Vulkan::Renderer

Main Vulkan rendering engine class.

```cpp
namespace Vulkan {

class Renderer {
public:
    static Renderer& GetInstance();
    
    bool Initialize(HWND hwnd, UINT width, UINT height);
    void Shutdown();
    
    bool BeginFrame();
    void EndFrame();
    void RenderScene();
    void RenderUI();
    void Resize(UINT width, UINT height);
    
    // Getters
    VkInstance GetInstance() const;
    VkPhysicalDevice GetPhysicalDevice() const;
    VkDevice GetDevice() const;
    VkQueue GetGraphicsQueue() const;
    VkCommandBuffer GetCommandBuffer() const;
    VkRenderPass GetRenderPass() const;
    VkFramebuffer GetFramebuffer() const;
    
    UINT GetWidth() const;
    UINT GetHeight() const;
    bool IsInitialized() const;
};

} // namespace Vulkan
```

### Config::ConfigManager

Configuration management class.

```cpp
namespace Config {

class ConfigManager {
public:
    static ConfigManager& GetInstance();
    
    bool Load(const std::wstring& filename = L"ofp_renderer.ini");
    bool Save(const std::wstring& filename = L"ofp_renderer.ini");
    void ResetToDefaults();
    
    // Getters
    const RendererSettings& GetRenderer() const;
    const EffectSettings& GetEffects() const;
    const PerformanceSettings& GetPerformance() const;
    const ScreenshotSettings& GetScreenshot() const;
    
    // Setters
    RendererSettings& GetRenderer();
    EffectSettings& GetEffects();
    PerformanceSettings& GetPerformance();
    ScreenshotSettings& GetScreenshot();
};

} // namespace Config
```

### Bridge::D3D8Bridge

D3D8 to Vulkan compatibility layer.

```cpp
namespace Bridge {

class D3D8Bridge {
public:
    static D3D8Bridge& GetInstance();
    
    bool Initialize();
    void Shutdown();
    
    bool CreateDeviceObjects(IDirect3DDevice9* device, UINT width, UINT height, D3DFORMAT format);
    void DestroyDeviceObjects();
    
    // D3D8 API translation
    void SetVertexBuffer(VkBuffer buffer, UINT stride);
    void SetIndexBuffer(VkBuffer buffer, VkIndexType indexType);
    void SetRenderTargets(VkImage rt, VkImage ds);
    void SetViewport(const VkViewport& viewport);
    void SetScissor(const VkRect2D& scissor);
    
    void Draw(UINT vertexCount, UINT startVertex);
    void DrawIndexed(UINT indexCount, UINT startIndex, INT baseVertex);
    
    void Clear(DWORD count, const D3DRECT* rects, DWORD flags, D3DCOLOR color, float z, DWORD stencil);
    void BeginScene();
    void EndScene();
    void Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
    
    State& GetState();
};

} // namespace Bridge
```

### PostProcessing::PostProcessor

Post-processing effects manager.

```cpp
namespace PostProcessing {

class PostProcessor {
public:
    static PostProcessor& GetInstance();
    
    bool Initialize(VkDevice device, VkPhysicalDevice physicalDevice, UINT width, UINT height);
    void Shutdown();
    
    void Resize(UINT width, UINT height);
    void BeginPostProcessing();
    void EndPostProcessing();
    
    void ApplyHardLight(float strengthR, float strengthG, float strengthB);
    void ApplyDesaturation(float strength);
    void ApplyGlare(float strength, int size, bool darkenSky);
    
    VkImageView GetOutputView() const;
};

} // namespace PostProcessing
```

## Configuration File

### ofp_renderer.ini

```ini
[Renderer]
EnableValidation=true
EnableVSync=false
EnableAnisotropy=true
AnisotropyLevel=16
Width=1920
Height=1080
Fullscreen=false

[Effects]
EnablePostProcessing=true
EnableHardLight=false
EnableDesaturate=false
EnableGlare=false
HardLightStrength=0.4
DesaturationStrength=0.2
GlareStrength=0.3
GlareSize=3
GlareDarkenSky=false

[Performance]
EnableAutoFallback=true
EnableLODBias=false
LODBias0=0.0
LODBias1=0.0

[Screenshot]
EnableScreenshots=true
AutoSave=true
SavePath=.\Screenshots
Format=png
```

## Usage Example

```cpp
#include "config.h"
#include "vulkan_renderer.h"
#include "d3d8_bridge.h"
#include "post_processing.h"

int main() {
    // Load configuration
    Config::ConfigManager::GetInstance().Load();
    
    // Initialize renderer
    Vulkan::Renderer& renderer = Vulkan::Renderer::GetInstance();
    renderer.Initialize(hwnd, 1920, 1080);
    
    // Main loop
    while (running) {
        if (renderer.BeginFrame()) {
            renderer.RenderScene();
            renderer.RenderUI();
            renderer.EndFrame();
        }
    }
    
    renderer.Shutdown();
    return 0;
}
```

## Error Handling

All methods return `bool` indicating success (`true`) or failure (`false`).

Use debug output for troubleshooting:
```cpp
OutputDebugStringA("[OFP Renderer] Error message");
```

## Thread Safety

- `Renderer::GetInstance()` - Thread-safe singleton
- `ConfigManager::GetInstance()` - Thread-safe singleton
- Other classes should be accessed from a single thread

/**
 * @file d3d8_bridge.h
 * @brief D3D8 to Vulkan compatibility layer
 * 
 * This bridge translates Direct3D 8 API calls to Vulkan,
 * allowing OFP (which uses D3D8) to run with Vulkan rendering.
 */

#ifndef OFP_RENDERER_D3D8_BRIDGE_H
#define OFP_RENDERER_D3D8_BRIDGE_H

#include <d3d8.h>
#include <d3d9.h>
#include <vulkan/vulkan.h>

namespace Bridge {

/**
 * @struct State
 * @brief Current rendering state
 */
struct State {
    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkBuffer indexBuffer = VK_NULL_HANDLE;
    VkImage renderTarget = VK_NULL_HANDLE;
    VkImage depthStencil = VK_NULL_HANDLE;
    VkImageView renderTargetView = VK_NULL_HANDLE;
    VkImageView depthStencilView = VK_NULL_HANDLE;
    
    VkSampler anisotropySampler = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline graphicsPipeline = VK_NULL_HANDLE;
    
    VkCommandBuffer currentCommandBuffer = VK_NULL_HANDLE;
    VkFence commandBufferFence = VK_NULL_HANDLE;
    
    D3DMATRIX worldMatrix;
    D3DMATRIX viewMatrix;
    D3DMATRIX projectionMatrix;
    
    VkViewport viewport;
    VkRect2D scissor;
    
    D3DFORMAT backBufferFormat = D3DFMT_UNKNOWN;
    UINT width = 0;
    UINT height = 0;
    BOOL fullscreen = FALSE;
};

/**
 * @class D3D8Bridge
 * @brief Translates D3D8 calls to Vulkan
 * 
 * This class provides compatibility between the legacy D3D8 API
 * used by OFP and the modern Vulkan API used for rendering.
 */
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
    
    // Drawing
    void Draw(UINT vertexCount, UINT startVertex);
    void DrawIndexed(UINT indexCount, UINT startIndex, INT baseVertex);
    void DrawIndexedPrimitiveUP(
        D3DPRIMITIVETYPE primitiveType,
        UINT minVertexIndex,
        UINT numVertices,
        UINT primitiveCount,
        CONST void* pIndexData,
        D3DFORMAT indexDataFormat,
        CONST void* pVertexData,
        UINT vertexStride
    );
    
    // Frame management
    void Clear(DWORD count, const D3DRECT* rects, DWORD flags, D3DCOLOR color, float z, DWORD stencil);
    void BeginScene();
    void EndScene();
    void Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
    
    // State access
    State& GetState() { return m_State; }
    const State& GetState() const { return m_State; }
    
private:
    D3D8Bridge() = default;
    ~D3D8Bridge() { Shutdown(); }
    
    bool CreateVertexShader();
    bool CreatePixelShader();
    bool CreateSampler();
    bool CreateUniformBuffer();
    void UpdateUniformBuffer();
    
    State m_State;
    VkBuffer m_UniformBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_UniformBufferMemory = VK_NULL_HANDLE;
    
    VkShaderModule m_VertexShaderModule = VK_NULL_HANDLE;
    VkShaderModule m_PixelShaderModule = VK_NULL_HANDLE;
    
    bool m_Initialized = false;
    bool m_InScene = false;
};

} // namespace Bridge

#endif // OFP_RENDERER_D3D8_BRIDGE_H

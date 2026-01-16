#pragma once
#include <vulkan/vulkan.h>
#include <d3d9.h>

struct D3D8ToVulkanState
{
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

class D3D8ToVulkanBridge
{
public:
    static D3D8ToVulkanBridge& GetInstance()
    {
        static D3D8ToVulkanBridge instance;
        return instance;
    }

    bool Initialize();
    void Shutdown();

    bool CreateDeviceObjects(IDirect3DDevice9* pDevice, UINT width, UINT height, D3DFORMAT format);
    void DestroyDeviceObjects();

    void SetVertexBuffer(VkBuffer buffer, UINT stride);
    void SetIndexBuffer(VkBuffer buffer, VkIndexType indexType);
    void SetRenderTargets(VkImage rt, VkImage ds);
    void SetViewport(const VkViewport& viewport);
    void SetScissor(const VkRect2D& scissor);

    void Draw(UINT vertexCount, UINT startVertex);
    void DrawIndexed(UINT indexCount, UINT startIndex, INT baseVertex);
    void DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE primitiveType, UINT minVertexIndex, UINT numVertices, UINT primitiveCount, CONST void* pIndexData, D3DFORMAT indexDataFormat, CONST void* pVertexData, UINT vertexStride);

    void Clear(DWORD count, const D3DRECT* rects, DWORD flags, D3DCOLOR color, float z, DWORD stencil);

    void BeginScene();
    void EndScene();

    void Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);

    D3D8ToVulkanState& GetState() { return m_State; }

private:
    D3D8ToVulkanBridge() = default;
    ~D3D8ToVulkanBridge() { Shutdown(); }

    bool CreateVertexShader();
    bool CreatePixelShader();
    bool CreateSampler();
    bool CreateUniformBuffer();

    void UpdateUniformBuffer();

    D3D8ToVulkanState m_State;
    VkBuffer m_UniformBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_UniformBufferMemory = VK_NULL_HANDLE;

    VkShaderModule m_VertexShaderModule = VK_NULL_HANDLE;
    VkShaderModule m_PixelShaderModule = VK_NULL_HANDLE;

    bool m_bInitialized = false;
    bool m_bInScene = false;
};

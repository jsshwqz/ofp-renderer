/**
 * @file vulkan_renderer.h
 * @brief Main Vulkan renderer class for OFP
 * 
 * This is the core renderer that replaces the original D3D8 rendering
 * with modern Vulkan API for improved performance and stability.
 */

#ifndef OFP_RENDERER_VULKAN_RENDERER_H
#define OFP_RENDERER_VULKAN_RENDERER_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "vulkan/vulkan.h"
#include <vector>
#include <memory>
#include <cstdint>

namespace Vulkan {

/**
 * @class Renderer
 * @brief Main Vulkan rendering engine
 * 
 * This class handles:
 * - Vulkan instance and device creation
 * - Swap chain management
 * - Render pass and pipeline creation
 * - Frame rendering
 * - Present to screen
 */
class Renderer {
public:
    static Renderer& GetInstance();
    
    /**
     * @brief Initialize the Vulkan renderer
     * @param hwnd Window handle
     * @param width Window width
     * @param height Window height
     * @return true if successful
     */
    bool Initialize(HWND hwnd, uint32_t width, uint32_t height);
    
    /**
     * @brief Shutdown the renderer and release resources
     */
    void Shutdown();
    
    /**
     * @brief Begin a new frame
     * @return true if successful
     */
    bool BeginFrame();
    
    /**
     * @brief End the current frame and present
     */
    void EndFrame();
    
    /**
     * @brief Render the scene
     */
    void RenderScene();
    
    /**
     * @brief Render UI overlays
     */
    void RenderUI();
    
    /**
     * @brief Handle window resize
     */
    void Resize(uint32_t width, uint32_t height);
    
    // Getters
    VkInstance GetVkInstance() const { return m_Instance; }
    VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
    VkDevice GetDevice() const { return m_Device; }
    VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
    VkCommandBuffer GetCommandBuffer() const { return m_CommandBuffer; }
    VkRenderPass GetRenderPass() const { return m_RenderPass; }
    VkFramebuffer GetFramebuffer() const { return m_Framebuffers[0]; }
    
    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }
    bool IsInitialized() const { return m_Initialized; }
    
private:
    Renderer() = default;
    ~Renderer() { Shutdown(); }
    
    bool CreateInstance();
    bool CreateSurface(HWND hwnd);
    bool PickPhysicalDevice();
    bool CreateLogicalDevice();
    bool CreateSwapChain(uint32_t width, uint32_t height);
    bool CreateImageViews();
    bool CreateRenderPass();
    bool CreateGraphicsPipeline();
    bool CreateFramebuffers();
    bool CreateCommandPool();
    bool CreateCommandBuffer();
    bool CreateSynchronization();
    
    void CleanupSwapChain();
    void RecreateSwapChain(uint32_t width, uint32_t height);
    
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device = VK_NULL_HANDLE;
    VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
    
    std::vector<VkImage> m_SwapChainImages;
    std::vector<VkImageView> m_SwapChainImageViews;
    std::vector<VkFramebuffer> m_Framebuffers;
    
    VkRenderPass m_RenderPass = VK_NULL_HANDLE;
    VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
    
    VkCommandPool m_CommandPool = VK_NULL_HANDLE;
    VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
    
    VkSemaphore m_ImageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore m_RenderFinishedSemaphore = VK_NULL_HANDLE;
    VkFence m_InFlightFence = VK_NULL_HANDLE;
    
    VkExtent2D m_SwapChainExtent;
    VkPresentModeKHR m_PresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    VkSurfaceFormatKHR m_SurfaceFormat;
    
    uint32_t m_Width = 0;
    uint32_t m_Height = 0;
    uint32_t m_CurrentFrame = 0;
    
    bool m_Initialized = false;
    bool m_VSyncEnabled = false;
};

} // namespace Vulkan

#endif // OFP_RENDERER_VULKAN_RENDERER_H

/**
 * @file post_processing.h
 * @brief Post-processing effects for Vulkan renderer
 * 
 * This file defines the post-processing effects that can be applied
 * to the rendered scene, including hard light, desaturation, and glare.
 */

#ifndef OFP_RENDERER_POST_PROCESSING_H
#define OFP_RENDERER_POST_PROCESSING_H

#include <vulkan/vulkan.h>

namespace PostProcessing {

/**
 * @struct EffectConfig
 * @brief Configuration for a single post-processing effect
 */
struct EffectConfig {
    bool enabled = false;
    float strength = 0.5f;
    float param0 = 0.0f;
    float param1 = 0.0f;
    float param2 = 0.0f;
};

/**
 * @class PostProcessor
 * @brief Manages post-processing effects
 * 
 * This class handles:
 * - Creating render targets for off-screen processing
 * - Managing post-processing shaders
 * - Applying effects in the correct order
 * - Blending the result with the final image
 */
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
    
    VkImageView GetOutputView() const { return m_OutputImageView; }
    
private:
    PostProcessor() = default;
    ~PostProcessor() { Shutdown(); }

    void CleanupRenderTargets();
    UINT FindMemoryType(VkPhysicalDeviceMemoryProperties properties, UINT typeFilter, VkMemoryPropertyFlags propertiesFlags);
    bool CreateRenderTargets(UINT width, UINT height);
    bool CreateShaders();
    bool CreateSamplers();
    
    void RenderQuad();
    
    VkDevice m_Device = VK_NULL_HANDLE;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    
    VkImage m_IntermediateImage = VK_NULL_HANDLE;
    VkImageView m_IntermediateImageView = VK_NULL_HANDLE;
    VkDeviceMemory m_IntermediateImageMemory = VK_NULL_HANDLE;
    
    VkImage m_OutputImage = VK_NULL_HANDLE;
    VkImageView m_OutputImageView = VK_NULL_HANDLE;
    VkDeviceMemory m_OutputImageMemory = VK_NULL_HANDLE;
    
    VkSampler m_Sampler = VK_NULL_HANDLE;
    
    VkShaderModule m_HardLightShader = VK_NULL_HANDLE;
    VkShaderModule m_DesaturateShader = VK_NULL_HANDLE;
    VkShaderModule m_GlareShader = VK_NULL_HANDLE;
    VkShaderModule m_CopyShader = VK_NULL_HANDLE;
    
    VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_HardLightPipeline = VK_NULL_HANDLE;
    VkPipeline m_DesaturatePipeline = VK_NULL_HANDLE;
    VkPipeline m_GlarePipeline = VK_NULL_HANDLE;
    VkPipeline m_CopyPipeline = VK_NULL_HANDLE;
    
    UINT m_Width = 0;
    UINT m_Height = 0;
    
    EffectConfig m_HardLight;
    EffectConfig m_Desaturate;
    EffectConfig m_Glare;
    
    bool m_Initialized = false;
};

} // namespace PostProcessing

#endif // OFP_RENDERER_POST_PROCESSING_H

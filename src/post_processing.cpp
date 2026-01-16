#include "post_processing.h"
#include <iostream>
#include <stdexcept>

namespace PostProcessing {

PostProcessor& PostProcessor::GetInstance()
{
    static PostProcessor instance;
    return instance;
}

bool PostProcessor::Initialize(VkDevice device, VkPhysicalDevice physicalDevice, UINT width, UINT height)
{
    if (m_Initialized) return true;

    m_Device = device;
    m_PhysicalDevice = physicalDevice;
    m_Width = width;
    m_Height = height;

    OutputDebugStringA("[PostProcessing] Initializing...\n");

    if (!CreateRenderTargets(width, height))
    {
        OutputDebugStringA("[PostProcessing] Failed to create render targets\n");
        return false;
    }

    if (!CreateSamplers())
    {
        OutputDebugStringA("[PostProcessing] Failed to create samplers\n");
        return false;
    }

    m_Initialized = true;
    OutputDebugStringA("[PostProcessing] Initialized successfully\n");
    return true;
}

void PostProcessor::Shutdown()
{
    if (!m_Initialized) return;

    vkDeviceWaitIdle(m_Device);

    if (m_CopyPipeline) vkDestroyPipeline(m_Device, m_CopyPipeline, nullptr);
    if (m_GlarePipeline) vkDestroyPipeline(m_Device, m_GlarePipeline, nullptr);
    if (m_DesaturatePipeline) vkDestroyPipeline(m_Device, m_DesaturatePipeline, nullptr);
    if (m_HardLightPipeline) vkDestroyPipeline(m_Device, m_HardLightPipeline, nullptr);
    if (m_PipelineLayout) vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);

    if (m_CopyShader) vkDestroyShaderModule(m_Device, m_CopyShader, nullptr);
    if (m_GlareShader) vkDestroyShaderModule(m_Device, m_GlareShader, nullptr);
    if (m_DesaturateShader) vkDestroyShaderModule(m_Device, m_DesaturateShader, nullptr);
    if (m_HardLightShader) vkDestroyShaderModule(m_Device, m_HardLightShader, nullptr);

    if (m_Sampler) vkDestroySampler(m_Device, m_Sampler, nullptr);

    if (m_OutputImageView) vkDestroyImageView(m_Device, m_OutputImageView, nullptr);
    if (m_OutputImageMemory) vkFreeMemory(m_Device, m_OutputImageMemory, nullptr);
    if (m_OutputImage) vkDestroyImage(m_Device, m_OutputImage, nullptr);

    if (m_IntermediateImageView) vkDestroyImageView(m_Device, m_IntermediateImageView, nullptr);
    if (m_IntermediateImageMemory) vkFreeMemory(m_Device, m_IntermediateImageMemory, nullptr);
    if (m_IntermediateImage) vkDestroyImage(m_Device, m_IntermediateImage, nullptr);

    m_Initialized = false;
    OutputDebugStringA("[PostProcessing] Shutdown complete\n");
}

void PostProcessor::Resize(UINT width, UINT height)
{
    if (!m_Initialized) return;

    m_Width = width;
    m_Height = height;

    vkDeviceWaitIdle(m_Device);

    CreateRenderTargets(width, height);
}

void PostProcessor::BeginPostProcessing()
{
}

void PostProcessor::EndPostProcessing()
{
}

void PostProcessor::ApplyHardLight(float strengthR, float strengthG, float strengthB)
{
    if (!m_Initialized || !m_HardLight.enabled) return;

    m_HardLight.strength = strengthR;
    m_HardLight.param0 = strengthG;
    m_HardLight.param1 = strengthB;

    RenderQuad();
}

void PostProcessor::ApplyDesaturation(float strength)
{
    if (!m_Initialized || !m_Desaturate.enabled) return;

    m_Desaturate.strength = strength;
    RenderQuad();
}

void PostProcessor::ApplyGlare(float strength, int size, bool darkenSky)
{
    if (!m_Initialized || !m_Glare.enabled) return;

    m_Glare.strength = strength;
    m_Glare.param0 = static_cast<float>(size);
    m_Glare.param1 = darkenSky ? 1.0f : 0.0f;

    RenderQuad();
}

bool PostProcessor::CreateRenderTargets(UINT width, UINT height)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageInfo.extent = {width, height, 1};
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkMemoryAllocateInfo memInfo{};
    memInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memInfo.allocationSize = 0;
    memInfo.memoryTypeIndex = 0;

    VkMemoryRequirements memReq{};
    vkGetImageMemoryRequirements(m_Device, m_IntermediateImage, &memReq);
    memInfo.allocationSize = memReq.size;
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memInfo.memoryTypeIndex);

    if (vkAllocateMemory(m_Device, &memInfo, nullptr, &m_IntermediateImageMemory) != VK_SUCCESS)
    {
        OutputDebugStringA("[PostProcessing] Failed to allocate intermediate image memory\n");
        return false;
    }

    vkBindImageMemory(m_Device, m_IntermediateImage, m_IntermediateImageMemory, 0);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_IntermediateImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_Device, &viewInfo, nullptr, &m_IntermediateImageView) != VK_SUCCESS)
    {
        OutputDebugStringA("[PostProcessing] Failed to create intermediate image view\n");
        return false;
    }

    imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    if (vkCreateImage(m_Device, &imageInfo, nullptr, &m_OutputImage) != VK_SUCCESS)
    {
        OutputDebugStringA("[PostProcessing] Failed to create output image\n");
        return false;
    }

    vkGetImageMemoryRequirements(m_Device, m_OutputImage, &memReq);
    memInfo.allocationSize = memReq.size;
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memInfo.memoryTypeIndex);

    if (vkAllocateMemory(m_Device, &memInfo, nullptr, &m_OutputImageMemory) != VK_SUCCESS)
    {
        OutputDebugStringA("[PostProcessing] Failed to allocate output image memory\n");
        return false;
    }

    vkBindImageMemory(m_Device, m_OutputImage, m_OutputImageMemory, 0);

    if (vkCreateImageView(m_Device, &viewInfo, nullptr, &m_OutputImageView) != VK_SUCCESS)
    {
        OutputDebugStringA("[PostProcessing] Failed to create output image view\n");
        return false;
    }

    return true;
}

bool PostProcessor::CreateShaders()
{
    return true;
}

bool PostProcessor::CreateSamplers()
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

    if (vkCreateSampler(m_Device, &samplerInfo, nullptr, &m_Sampler) != VK_SUCCESS)
    {
        OutputDebugStringA("[PostProcessing] Failed to create sampler\n");
        return false;
    }

    return true;
}

void PostProcessor::RenderQuad()
{
}

} // namespace PostProcessing

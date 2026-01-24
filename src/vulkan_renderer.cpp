#include <windows.h>
#include "../include/vulkan_renderer.h"
#include <iostream>
#include <stdexcept>
#include <set>
#include <vector>

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

bool Vulkan::Renderer::Initialize(HWND hwnd, uint32_t width, uint32_t height)
{
    if (m_bInitialized) return true;

    m_Width = width;
    m_Height = height;

    OutputDebugStringA("[VulkanRenderer] Initializing...\n");

    if (!CreateInstance())
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create instance\n");
        return false;
    }

    if (!CreateDevice(hwnd))
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create device\n");
        return false;
    }

    if (!CreateSwapChain(width, height))
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create swap chain\n");
        return false;
    }

    if (!CreateRenderPass())
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create render pass\n");
        return false;
    }

    if (!CreateFramebuffers())
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create framebuffers\n");
        return false;
    }

    if (!CreateCommandPool())
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create command pool\n");
        return false;
    }

    if (!CreateCommandBuffer())
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create command buffer\n");
        return false;
    }

    if (!CreateSynchronizationObjects())
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create sync objects\n");
        return false;
    }

    if (!CreateShaders())
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create shaders\n");
        return false;
    }

    if (!CreatePipeline())
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create pipeline\n");
        return false;
    }

    m_bInitialized = true;
    OutputDebugStringA("[VulkanRenderer] Initialized successfully\n");
    return true;
}

void Vulkan::Renderer::Shutdown()
{
    if (!m_bInitialized) return;

    vkDeviceWaitIdle(m_VkDevice);

    if (m_VkInFlightFence) vkDestroyFence(m_VkDevice, m_VkInFlightFence, nullptr);
    if (m_VkRenderFinishedSemaphore) vkDestroySemaphore(m_VkDevice, m_VkRenderFinishedSemaphore, nullptr);
    if (m_VkImageAvailableSemaphore) vkDestroySemaphore(m_VkDevice, m_VkImageAvailableSemaphore, nullptr);

    for (auto framebuffer : m_Framebuffers) vkDestroyFramebuffer(m_VkDevice, framebuffer, nullptr);
    if (m_VkPipeline) vkDestroyPipeline(m_VkDevice, m_VkPipeline, nullptr);
    if (m_VkPipelineLayout) vkDestroyPipelineLayout(m_VkDevice, m_VkPipelineLayout, nullptr);
    if (m_VkRenderPass) vkDestroyRenderPass(m_VkDevice, m_VkRenderPass, nullptr);
    if (m_VkFragmentShader) vkDestroyShaderModule(m_VkDevice, m_VkFragmentShader, nullptr);
    if (m_VkVertexShader) vkDestroyShaderModule(m_VkDevice, m_VkVertexShader, nullptr);

    vkFreeCommandBuffers(m_VkDevice, m_VkCommandPool, 1, &m_VkCommandBuffer);
    vkDestroyCommandPool(m_VkDevice, m_VkCommandPool, nullptr);

    for (auto imageView : m_SwapChainImageViews) vkDestroyImageView(m_VkDevice, imageView, nullptr);
    if (m_VkSwapChain) vkDestroySwapchainKHR(m_VkDevice, m_VkSwapChain, nullptr);

    vkDestroyDevice(m_VkDevice, nullptr);
    vkDestroyInstance(m_VkInstance, nullptr);

    m_bInitialized = false;
    OutputDebugStringA("[VulkanRenderer] Shutdown complete\n");
}

bool Vulkan::Renderer::CreateInstance()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "OFP Vulkan Renderer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "OFPEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::vector<const char*> requiredExtensions;
    requiredExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    requiredExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

    createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    if (m_Config.enableValidation)
    {
        createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&createInfo, nullptr, &m_VkInstance) != VK_SUCCESS)
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create instance\n");
        return false;
    }

    return true;
}

bool Vulkan::Renderer::CreateDevice(HWND hwnd)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        OutputDebugStringA("[VulkanRenderer] No Vulkan devices found\n");
        return false;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            m_VkPhysicalDevice = device;
            break;
        }
    }

    if (m_VkPhysicalDevice == VK_NULL_HANDLE)
    {
        m_VkPhysicalDevice = devices[0];
    }

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(m_VkPhysicalDevice, &properties);

    char msg[256];
    sprintf_s(msg, "[VulkanRenderer] Using GPU: %s\n", properties.deviceName);
    OutputDebugStringA(msg);

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

    int graphicsFamily = -1;
    int presentFamily = -1;

    for (int i = 0; i < queueFamilyCount; i++)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(m_VkPhysicalDevice, i, m_VkSurface, &presentSupport);
        if (presentSupport)
        {
            presentFamily = i;
        }
    }

    if (graphicsFamily == -1)
    {
        OutputDebugStringA("[VulkanRenderer] No graphics queue found\n");
        return false;
    }

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> uniqueQueueFamilies = {graphicsFamily, presentFamily};

    float queuePriority = 1.0f;
    for (int queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = m_Config.enableAnisotropy ? VK_TRUE : VK_FALSE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (m_Config.enableValidation)
    {
        createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(m_VkPhysicalDevice, &createInfo, nullptr, &m_VkDevice) != VK_SUCCESS)
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create device\n");
        return false;
    }

    vkGetDeviceQueue(m_VkDevice, graphicsFamily, 0, &m_VkGraphicsQueue);

    return true;
}

bool Vulkan::Renderer::CreateSwapChain(UINT width, UINT height)
{
    VkSurfaceCapabilities capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_VkPhysicalDevice, m_VkSurface, &capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_VkPhysicalDevice, m_VkSurface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_VkPhysicalDevice, m_VkSurface, &formatCount, formats.data());

    VkSurfaceFormatKHR surfaceFormat = formats[0];
    for (const auto& format : formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            surfaceFormat = format;
            break;
        }
    }

    VkExtent2D extent = {};
    extent.width = width;
    extent.height = height;

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
    {
        imageCount = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_VkSurface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = m_bVSyncEnabled ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(m_VkDevice, &createInfo, nullptr, &m_VkSwapChain) != VK_SUCCESS)
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create swap chain\n");
        return false;
    }

    vkGetSwapchainImagesKHR(m_VkDevice, m_VkSwapChain, &imageCount, nullptr);
    m_SwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_VkDevice, m_VkSwapChain, &imageCount, m_SwapChainImages.data());

    m_SwapChainImageViews.resize(imageCount);
    for (size_t i = 0; i < m_SwapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_SwapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = surfaceFormat.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_VkDevice, &createInfo, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS)
        {
            OutputDebugStringA("[VulkanRenderer] Failed to create image view\n");
            return false;
        }
    }

    return true;
}

bool Vulkan::Renderer::CreateRenderPass()
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(m_VkDevice, &renderPassInfo, nullptr, &m_VkRenderPass) != VK_SUCCESS)
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create render pass\n");
        return false;
    }

    return true;
}

bool Vulkan::Renderer::CreateFramebuffers()
{
    m_Framebuffers.resize(m_SwapChainImageViews.size());

    for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
    {
        VkImageView attachments[] = { m_SwapChainImageViews[i] };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_VkRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_Width;
        framebufferInfo.height = m_Height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_VkDevice, &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
        {
            OutputDebugStringA("[VulkanRenderer] Failed to create framebuffer\n");
            return false;
        }
    }

    return true;
}

bool Vulkan::Renderer::CreateCommandPool()
{
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = 0;

    if (vkCreateCommandPool(m_VkDevice, &poolInfo, nullptr, &m_VkCommandPool) != VK_SUCCESS)
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create command pool\n");
        return false;
    }

    return true;
}

bool Vulkan::Renderer::CreateCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_VkCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(m_VkDevice, &allocInfo, &m_VkCommandBuffer) != VK_SUCCESS)
    {
        OutputDebugStringA("[VulkanRenderer] Failed to allocate command buffer\n");
        return false;
    }

    return true;
}

bool Vulkan::Renderer::CreateSynchronizationObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(m_VkDevice, &semaphoreInfo, nullptr, &m_VkImageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(m_VkDevice, &semaphoreInfo, nullptr, &m_VkRenderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(m_VkDevice, &fenceInfo, nullptr, &m_VkInFlightFence) != VK_SUCCESS)
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create sync objects\n");
        return false;
    }

    return true;
}

bool Vulkan::Renderer::CreateShaders()
{
    return true;
}

bool Vulkan::Renderer::CreatePipeline()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(float) * 5;
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attributeDescriptions[2] = {};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = 0;

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].offset = sizeof(float) * 3;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = 2;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_Width;
    viewport.height = (float)m_Height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = {m_Width, m_Height};

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    if (vkCreatePipelineLayout(m_VkDevice, &pipelineLayoutInfo, nullptr, &m_VkPipelineLayout) != VK_SUCCESS)
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create pipeline layout\n");
        return false;
    }

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.layout = m_VkPipelineLayout;
    pipelineInfo.renderPass = m_VkRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.stageCount = 2;

    VkPipelineShaderStageCreateInfo stages[2] = {};
    stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].pName = "main";

    stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].pName = "main";

    pipelineInfo.pStages = stages;

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;

    if (vkCreateGraphicsPipelines(m_VkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_VkPipeline) != VK_SUCCESS)
    {
        OutputDebugStringA("[VulkanRenderer] Failed to create graphics pipeline\n");
        return false;
    }

    return true;
}

bool Vulkan::Renderer::BeginFrame()
{
    vkWaitForFences(m_VkDevice, 1, &m_VkInFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_VkDevice, 1, &m_VkInFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(m_VkDevice, m_VkSwapChain, UINT64_MAX, m_VkImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(m_VkCommandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(m_VkCommandBuffer, &beginInfo);

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_VkRenderPass;
    renderPassInfo.framebuffer = m_Framebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {m_Width, m_Height};

    VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(m_VkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(m_VkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VkPipeline);

    return true;
}

void Vulkan::Renderer::EndFrame()
{
    vkCmdEndRenderPass(m_VkCommandBuffer);

    if (vkEndCommandBuffer(m_VkCommandBuffer) != VK_SUCCESS)
    {
        OutputDebugStringA("[VulkanRenderer] Failed to record command buffer\n");
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &m_VkImageAvailableSemaphore;
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_VkCommandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &m_VkRenderFinishedSemaphore;

    vkQueueSubmit(m_VkGraphicsQueue, 1, &submitInfo, m_VkInFlightFence);

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_VkRenderFinishedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_VkSwapChain;

    vkQueuePresentKHR(m_VkGraphicsQueue, &presentInfo);
}

void Vulkan::Renderer::RenderScene()
{
}

void Vulkan::Renderer::RenderUI()
{
}

void Vulkan::Renderer::UpdatePipeline()
{
}

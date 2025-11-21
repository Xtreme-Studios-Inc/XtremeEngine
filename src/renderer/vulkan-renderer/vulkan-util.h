#pragma once

#include <codeframe>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <array>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>

//--DEVICE EXTENSIONS
const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

//--Indicies (Locations) of Queue Families (if they exist at all)
struct QueueFamilyIndices {
    int graphicsFamily = -1;     // Location of graphics que family
    int presentationFamily = -1; // Location of presentation queue family

    // Check if queue families are valid
    bool isValid() { return graphicsFamily >= 0 && presentationFamily >= 0; }
};

struct SwapChainDetails {
    VkSurfaceCapabilitiesKHR surfaceCapabilities{};    // Surface Properties
    std::vector<VkSurfaceFormatKHR> formats{};         // Surface image formats
    std::vector<VkPresentModeKHR> presentationModes{}; // How images are showen too screen
};

struct SwapChainImage {
    VkImage image{};
    VkImageView imageView{};
};

struct GraphicSettings {
    int MSAA{};
};

std::vector<char> readShader(const fs::path &shaderPath);

// Mesh Functions
// Vertex Data Representation
struct Vertex {
    glm::vec3 pos{};
    glm::vec3 col{};
    glm::vec2 tex{}; // Texture Coordiantes (u , v)
    glm::vec3 norm{};
};
uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t allowedTypes, VkMemoryPropertyFlags properties);
void createBuffer(VkPhysicalDevice physicalDevice,
                  VkDevice device,
                  VkDeviceSize bufferSize,
                  VkBufferUsageFlags bufferUsage,
                  VkMemoryPropertyFlags bufferProperties,
                  VkBuffer *buffer,
                  VkDeviceMemory *bufferMemory);

VkCommandBuffer beginCommandBuffer(VkDevice device, VkCommandPool commandPool);

void endAndSubmitCommandBuffer(VkDevice device,
                               VkCommandPool commandPool,
                               VkQueue queue,
                               VkCommandBuffer commandBuffer);

void copyBuffer(VkDevice device,
                VkQueue transferQueue,
                VkCommandPool transferCommandPool,
                VkBuffer srcBuffer,
                VkBuffer dstBuffer,
                VkDeviceSize bufferSize);

void copyImageBuffer(VkDevice device,
                     VkQueue transferQueue,
                     VkCommandPool transferCommandPool,
                     VkBuffer srcBuffer,
                     VkImage image,
                     uint32_t width,
                     uint32_t height);

void transitionImageLayout(VkDevice device,
                           VkQueue queue,
                           VkCommandPool commandPool,
                           VkImage image,
                           VkImageLayout oldLayout,
                           VkImageLayout newLayout,
                           uint32_t mipLevelCount);

// Image and ImageView
VkImage createImage(VkPhysicalDevice physicalDevice,
                    VkDevice device,
                    uint32_t width,
                    uint32_t height,
                    /*Optional*/ uint32_t mipLevelCount,
                    /*Optional*/ VkSampleCountFlagBits numSamples,
                    VkFormat format,
                    VkImageTiling tiling,
                    VkImageUsageFlags useFlags,
                    VkMemoryPropertyFlags propFlags,
                    VkDeviceMemory *imageMemory);
//	-- IMAGE VIEW --
VkImageView createImageView(
    VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevelCount);

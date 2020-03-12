#pragma once
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>
#include <cassert>
#include <fstream>
#include <experimental/filesystem>

namespace vk {
inline bool vk_check(VkResult res) {
  assert(("Vulkan command did not execute correctly", res == VK_SUCCESS));
}

inline VkShaderModule loadShader(const char* fileName, VkDevice device) {
  std::ifstream is(fileName, std::ios::binary | std::ios::in | std::ios::ate);

  if (is.is_open()) {
    size_t size = is.tellg();
    is.seekg(0, std::ios::beg);
    char* shaderCode = new char[size];
    is.read(shaderCode, size);
    is.close();

    assert(size > 0);

    VkShaderModule shaderModule;
    VkShaderModuleCreateInfo moduleCreateInfo{};
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.codeSize = size;
    moduleCreateInfo.pCode = (uint32_t*)shaderCode;

    vk_check(vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderModule));

    delete[] shaderCode;

    return shaderModule;
  } else {
    spdlog::error("Error: Could not open shader file {0}. Please make sure you are running Griddy from the correct working directory", std::experimental::filesystem::current_path().string() + "/" + std::string(fileName));
    return VK_NULL_HANDLE;
  }
}

inline VkBool32 getSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat* depthFormat) {
  // Since all depth formats may be optional, we need to find a suitable depth format to use
  // Start with the highest precision packed format
  std::vector<VkFormat> depthFormats = {
      VK_FORMAT_D32_SFLOAT_S8_UINT,
      VK_FORMAT_D32_SFLOAT,
      VK_FORMAT_D24_UNORM_S8_UINT,
      VK_FORMAT_D16_UNORM_S8_UINT,
      VK_FORMAT_D16_UNORM};

  for (auto& format : depthFormats) {
    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);
    // Format must support depth stencil attachment for optimal tiling
    if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
      *depthFormat = format;
      return true;
    }
  }

  return false;
}

}  // namespace vk
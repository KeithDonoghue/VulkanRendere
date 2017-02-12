#ifndef API_USAGE_HEADER_HPP 
#define API_USAGE_HEADER_HPP 1

#include "EngineLogging.h"


#include <stdio.h>
#include <unordered_map>
#include <Windows.h>

#define VK_USE_PLATFORM_WIN32_KHR 1
#include <vulkan/vulkan.h>




#define AddToErrorMap(entrypoint)                               \
			    {                                                                          \
            theMap[entrypoint] = #entrypoint; \
			    }

std::unordered_map<VkResult, std::string> InitMap();
std::string VulkanResultString(VkResult);



#endif // API_USAGE_HEADER_HPP 

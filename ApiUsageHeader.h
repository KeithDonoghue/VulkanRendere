#ifndef API_USAGE_HEADER_HPP 
#define API_USAGE_HEADER_HPP 1

#include <stdio.h>
#include <unordered_map>
#include <Windows.h>

#include <vulkan/vulkan.h>


/*
#define AddToErrorMap(map, ErrorCode)  \																
	{									\
map.insert(std::make_pair(ErrorCode, #ErrorCode)); \	
}\
*/



#define AddToErrorMap(entrypoint)                               \
			    {                                                                          \
            theMap[entrypoint] = #entrypoint; \
			    }

std::unordered_map<VkResult, std::string> InitMap();
std::string VulkanResultString(VkResult);



#endif // API_USAGE_HEADER_HPP 

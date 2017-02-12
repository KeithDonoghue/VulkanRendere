#include "ShaderModule.h"

#include "VulkanDevice.h"

#include <fstream>
#include <string>
#include <streambuf>

ShaderModule::ShaderModule(VulkanDevice& theDevice, std::string filename) :
mDevice(theDevice)
{
	std::ifstream inFile(filename, std::ifstream::binary);
	inFile.seekg(0, std::ios_base::end);
	size_t size = inFile.tellg();
	inFile.seekg(0, std::ios_base::beg);

	std::vector<char> code(size);
	inFile.read(code.data(), size);

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0; // reserved for future use.
	createInfo.codeSize = size;
	createInfo.pCode = (uint32_t*)code.data();

	VkResult result = vkCreateShaderModule(mDevice.GetVkDevice(), &createInfo, nullptr, &m_TheVulkanShaderModule);
	if (result != VK_SUCCESS)
	{
		EngineLog("failed to Create shader module.")
	}
}




ShaderModule::~ShaderModule()
{
	vkDestroyShaderModule(getDevice().getVkDevice(), m_TheVulkanShaderModule, nullptr);
}




ShaderModule * ShaderModule::CreateFragmentShader(VulkanDevice& theDevice)
{
	return new ShaderModule(theDevice, "Resources/frag.spv");
}


ShaderModule * ShaderModule::CreateVertexShader(VulkanDevice& theDevice)
{
	return new ShaderModule(theDevice, "Resources/vert.spv");
}




ShaderModule * ShaderModule::CreateFragmentShader2(VulkanDevice& theDevice)
{
	return new ShaderModule(theDevice, "Resources/red.spv");
}

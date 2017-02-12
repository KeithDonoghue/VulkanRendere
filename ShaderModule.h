#ifndef SHADER_MODULE_HDR
#define SHADER_MODULE_HDR 1

#include "ApiUsageHeader.h"

#include <string>


class VulkanDevice;

class ShaderModule{
public:
	ShaderModule(VulkanDevice&, std::string);
	~ShaderModule();
	VulkanDevice& getDevice(){ return mDevice; }
	VkShaderModule getVkShaderModule(){ return m_TheVulkanShaderModule; } 

	static ShaderModule * CreateVertexShader(VulkanDevice&);
	static ShaderModule * CreateFragmentShader(VulkanDevice&);
	static ShaderModule * CreateFragmentShader2(VulkanDevice&);

private:
	VulkanDevice& mDevice;
	VkShaderModule m_TheVulkanShaderModule;
};

#endif // SHADER_MODULE_HDR

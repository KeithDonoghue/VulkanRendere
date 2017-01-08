#include "vulkan/vulkan.h"
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

private:
	VulkanDevice& mDevice;
	VkShaderModule m_TheVulkanShaderModule;
};
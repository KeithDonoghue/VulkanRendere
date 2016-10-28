#include "vulkan/vulkan.h"

class Swapchain{
public:
	Swapchain(VkDevice, VkSurfaceKHR);
	Swapchain(VkDevice, VkSurfaceKHR, VkSwapchainCreateInfoKHR);
	~Swapchain();

private:
	void init();

private:
	VkSwapchainCreateInfoKHR mCreateInfo;
	VkDevice mDevice;
	VkSurfaceKHR mSurface;

	VkSwapchainKHR theVulkanSwapchain;
};
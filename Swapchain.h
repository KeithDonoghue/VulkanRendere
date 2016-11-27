#include "vulkan/vulkan.h"

#include "VulkanDevice.h"
#include "WindowsWindow.h"

class Swapchain{
public:
	Swapchain(VulkanDevice*, EngineWindow*);
	Swapchain(VulkanDevice*, EngineWindow*, VkSwapchainCreateInfoKHR);
	~Swapchain();
	void GetImages();
	void Update();

private:
	void init();

private:
	VkSwapchainCreateInfoKHR mCreateInfo;
	VulkanDevice * mDevice;
	EngineWindow * mWindow;

	VkSwapchainKHR theVulkanSwapchain;

	VkSemaphore mWaitForAquireSemaphore;
	VkSemaphore mWaitForPresentSemaphore;
};
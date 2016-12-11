#include "vulkan/vulkan.h"

#include "VulkanDevice.h"
#include "WindowsWindow.h"

#include <deque>

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


	uint32_t mNumSemaphores;
	std::deque<VkSemaphore> mSemaphoreQueue;

	uint32_t mMinImageCount;
	uint32_t mSwapchainImageCount;

	uint32_t mImagesHeld;
};
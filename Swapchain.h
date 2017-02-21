#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP 1

#include "ApiUsageHeader.h"
#include "VulkanDevice.h"
#include "WindowsWindow.h"

#include <deque>

class Swapchain{
public:
	Swapchain(VulkanDevice*, EngineWindow&);
	Swapchain(VulkanDevice*, EngineWindow&, VkSwapchainCreateInfoKHR);
	~Swapchain();
	void Resize(uint32_t, uint32_t){};
	uint32_t getNumImages(){ return mSwapchainImageCount; }
	void GetImages();
	void Update();

private:
	void init();

private:
	VkSwapchainCreateInfoKHR mCreateInfo;
	VulkanDevice * mDevice;
	EngineWindow & mWindow;

	VkSwapchainKHR theVulkanSwapchain;


	uint32_t mNumSemaphores;
	std::deque<VkSemaphore> mSemaphoreQueue;

	uint32_t mMinImageCount;
	uint32_t mSwapchainImageCount;

	uint32_t mImagesHeld;
};
#endif // SWAPCHAIN_HPP

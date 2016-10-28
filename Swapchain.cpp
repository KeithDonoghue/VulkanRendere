#include "Swapchain.h"

#define ENGINE_LOGGING_ENABLED 1 
#include "EngineLogging.h"


#include "Windows.h"

Swapchain::Swapchain(VkDevice theDevice, VkSurfaceKHR theSurface):
	mDevice(theDevice),
	mSurface(theSurface)
{ 

	VkExtent2D WindowSize;
	memset(&WindowSize, 0, sizeof(VkExtent2D));
	WindowSize.width = 400;
	WindowSize.height = 400;


	memset(&mCreateInfo, 0, sizeof(mCreateInfo));
	mCreateInfo.sType				= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	mCreateInfo.pNext				= nullptr;
	//mCreateInfo.flags				= 0; reserved for future use.
	mCreateInfo.surface				= theSurface;
	mCreateInfo.minImageCount		= 2;
	mCreateInfo.imageFormat			= VK_FORMAT_B8G8R8A8_UNORM;
	mCreateInfo.imageColorSpace		= VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	mCreateInfo.imageExtent			= WindowSize;
	mCreateInfo.imageArrayLayers	= 1;
	mCreateInfo.imageUsage			= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	mCreateInfo.imageSharingMode	= VK_SHARING_MODE_EXCLUSIVE;
	//mCreateInfo.queueFamilyIndexCount 
	//mCreateInfo.pQueueFamilyIndices
	mCreateInfo.preTransform		= VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	mCreateInfo.compositeAlpha		= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	mCreateInfo.presentMode			= VK_PRESENT_MODE_FIFO_KHR;

	mCreateInfo.clipped				= VK_TRUE;
	mCreateInfo.oldSwapchain		= VK_NULL_HANDLE;

	init();
}




Swapchain::Swapchain(VkDevice theDevice, VkSurfaceKHR theSurface, VkSwapchainCreateInfoKHR theCreateInfo):
	mDevice(theDevice),
	mSurface(theSurface),
	mCreateInfo(theCreateInfo)
{
	init();
}




Swapchain::~Swapchain()
{
	vkDestroySwapchainKHR(mDevice, theVulkanSwapchain, nullptr);
}



void Swapchain::init()
{

	VkResult result = vkCreateSwapchainKHR(mDevice, &mCreateInfo, nullptr, &theVulkanSwapchain);

	if (result != VK_SUCCESS)
	{
		EngineLog("Hello");
	}
	else
	{
		EngineLog("success!");
	}



}
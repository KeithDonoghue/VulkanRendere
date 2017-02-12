#define VULKAN_FRAMERATE_LOGGER 0

#if VULKAN_FRAMERATE_LOGGER  
#define ENGINE_LOGGING_ENABLED 1
#include "EngineLogging.h"
#endif

#include "Swapchain.h"

#include "Windows.h"


#include <chrono>

Swapchain::Swapchain(VulkanDevice * theDevice, EngineWindow & theWindow):
	mDevice(theDevice),
	mWindow(theWindow),
	mNumSemaphores(0),
	mMinImageCount(0),
	mImagesHeld(0)
{ 
	VkSurfaceCapabilitiesKHR surfaceCaps = mDevice->GetSurfaceCapabilities();
	mMinImageCount = surfaceCaps.minImageCount;

	memset(&mCreateInfo, 0, sizeof(mCreateInfo));
	mCreateInfo.sType				= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	mCreateInfo.pNext				= nullptr;
	//mCreateInfo.flags				= 0; reserved for future use.
	mCreateInfo.surface				= NULL; // assigned in init.
	mCreateInfo.minImageCount		= 8;
	mCreateInfo.imageFormat			= VK_FORMAT_B8G8R8A8_UNORM;
	mCreateInfo.imageColorSpace		= VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	mCreateInfo.imageExtent			= theWindow.GetExtent();
	mCreateInfo.imageArrayLayers	= 1;
	mCreateInfo.imageUsage			= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	mCreateInfo.imageSharingMode	= VK_SHARING_MODE_EXCLUSIVE;
	//mCreateInfo.queueFamilyIndexCount 
	//mCreateInfo.pQueueFamilyIndices
	mCreateInfo.preTransform		= VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	mCreateInfo.compositeAlpha		= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	mCreateInfo.presentMode			= VK_PRESENT_MODE_FIFO_RELAXED_KHR;

	mCreateInfo.clipped				= VK_TRUE;
	mCreateInfo.oldSwapchain		= VK_NULL_HANDLE;

	init();
}




Swapchain::Swapchain(VulkanDevice * theDevice, EngineWindow & theWindow, VkSwapchainCreateInfoKHR theCreateInfo):
	mDevice(theDevice),
	mWindow(theWindow),
	mCreateInfo(theCreateInfo)
{
	init();
}




Swapchain::~Swapchain()
{

	for (VkSemaphore it : mSemaphoreQueue)
	{
		vkDestroySemaphore(mDevice->getVkDevice(), it, nullptr);
	}

	vkDestroySwapchainKHR(mDevice->getVkDevice(), theVulkanSwapchain, nullptr);
}



void Swapchain::init()
{

	mCreateInfo.surface = mWindow.GetSurface();

	VkResult result = vkCreateSwapchainKHR(mDevice->getVkDevice(), &mCreateInfo, nullptr, &theVulkanSwapchain);

	if (result != VK_SUCCESS)
	{
		EngineLog("Hello");
	}
	else
	{
		EngineLog("success!");
	}

	GetImages();

}





void Swapchain::GetImages()
{
	VkResult result = vkGetSwapchainImagesKHR(mDevice->GetVkDevice(), theVulkanSwapchain, &mSwapchainImageCount, nullptr);

	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to get Images");
	}

	VkImage *  SwpachainImages = (VkImage*) malloc(mSwapchainImageCount* sizeof(VkImage));
	result = vkGetSwapchainImagesKHR(mDevice->GetVkDevice(), theVulkanSwapchain, &mSwapchainImageCount, SwpachainImages);

	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to get Images");
	}
	else
	{
		mDevice->PopulatePresentableImages(SwpachainImages, 
			mSwapchainImageCount, 
			mCreateInfo.imageExtent.width, 
			mCreateInfo.imageExtent.height);
	}



	// 2 aquire and a release sempahore's for each image in the swapchian, to be safe until I start syncing reuse.
	mNumSemaphores = mSwapchainImageCount * 4;


	VkSemaphoreCreateInfo SemaphoreCreateInfo;
	memset(&SemaphoreCreateInfo, 0, sizeof(SemaphoreCreateInfo));

	SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	SemaphoreCreateInfo.pNext = nullptr;
	SemaphoreCreateInfo.flags = 0; //reserved for future use.

	for (uint32_t i = 0; i < mNumSemaphores; i++)
	{
		VkSemaphore tempSemaphore;
		result = vkCreateSemaphore(mDevice->getVkDevice(), &SemaphoreCreateInfo, nullptr, &tempSemaphore);

		if (result != VK_SUCCESS)
		{
			EngineLog("Failed to create semaphore");
		}
		else
		{
			mSemaphoreQueue.push_back(tempSemaphore);
		}
	}
}





void Swapchain::Update()
{
	SyncedPresentable nextFreePresentable;


	// Only attempt to acquire an image if were not holding them all.
	while(mSwapchainImageCount - mMinImageCount + 1 > mImagesHeld)
	{
		nextFreePresentable.mWaitForAcquireSemaphore = mSemaphoreQueue.front();
		mSemaphoreQueue.pop_front();

		nextFreePresentable.mWaitForPresentSemaphore = mSemaphoreQueue.front();
		mSemaphoreQueue.pop_front();

		mDevice->LockPresentQueue();
		VkResult result = vkAcquireNextImageKHR(mDevice->getVkDevice(),
			theVulkanSwapchain,
			UINT64_MAX,
			nextFreePresentable.mWaitForAcquireSemaphore,
			VK_NULL_HANDLE,
			&nextFreePresentable.mEngineImageIndex);
		mDevice->UnlockPresentQueue();

		if (result != VK_SUCCESS)
		{
			mSemaphoreQueue.push_front(nextFreePresentable.mWaitForPresentSemaphore);
			mSemaphoreQueue.push_front(nextFreePresentable.mWaitForAcquireSemaphore);
			EngineLog("Failed to acquire image", VulkanResultString(result));
		}
		else
		{
			mDevice->AddToAvailableQueue(nextFreePresentable);
			mImagesHeld++;
		}

	}
	
	bool PresentableIsReady = mDevice->GetFromPresentQueue(nextFreePresentable);

	if (PresentableIsReady)
	{
		VkPresentInfoKHR thePresentInfo;
		memset(&thePresentInfo, 0, sizeof(VkPresentInfoKHR));
		thePresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		thePresentInfo.pNext = nullptr;
		thePresentInfo.waitSemaphoreCount = 1;
		thePresentInfo.pWaitSemaphores = &nextFreePresentable.mWaitForPresentSemaphore;
		thePresentInfo.swapchainCount = 1;
		thePresentInfo.pSwapchains = &theVulkanSwapchain;
		thePresentInfo.pImageIndices = &nextFreePresentable.mEngineImageIndex;

		mDevice->LockPresentQueue();
		VkResult result = vkQueuePresentKHR(mDevice->GetVkPresentQueue(), &thePresentInfo);
		mDevice->UnlockPresentQueue();

		mImagesHeld--;

		if (result != VK_SUCCESS)
		{
			EngineLog("Failed to present Image");
		}


		mSemaphoreQueue.push_back(nextFreePresentable.mWaitForAcquireSemaphore);
		mSemaphoreQueue.push_back(nextFreePresentable.mWaitForPresentSemaphore);

#if VULKAN_FRAMERATE_LOGGER
		static int frames = 0;
		static std::chrono::time_point<std::chrono::high_resolution_clock> mPrevP =
			std::chrono::high_resolution_clock::now();

		static std::chrono::time_point<std::chrono::high_resolution_clock> mCurrP;

		frames++;

		if (frames > 0)
		{
			mCurrP = std::chrono::high_resolution_clock::now();

			uint64_t duration = std::chrono::duration_cast<std::chrono::microseconds>(mCurrP - mPrevP).count();

			mPrevP = mCurrP;
			float frameRate = 1000000.0 / duration;

			EngineLog("Frame time: ", duration);
			EngineLog("Frame rate: ", frameRate);
			frames = 0;
		}
#endif

	}
}
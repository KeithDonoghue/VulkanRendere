#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanBuffer.h"
#include "VulkanMemoryManager.h"
#include "DescriptorPool.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "VulkanRenderPass.h"
#include "ShaderModule.h"
#include "VulkanPipeline.h"
#include "RenderInstance.h"

#include "Windows.h"

#include <vector>




VulkanDevice::VulkanDevice(VkPhysicalDevice thePhysicalDevice, EngineWindow& theWindow):
mPhysicalDevice(thePhysicalDevice),
mWindow(theWindow)
{
	CheckSurfaceCapabilities();

	float queuePriorities[2] = { 1.0f, 1.0f };

	VkDeviceQueueCreateInfo QueueCreateInfo = {};
	QueueCreateInfo.pNext = NULL;
	QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	QueueCreateInfo.flags = 0; // Reserved For future use
	QueueCreateInfo.queueFamilyIndex = 0;
	QueueCreateInfo.queueCount = 2;
	QueueCreateInfo.pQueuePriorities = queuePriorities;



	std::vector<char *> DeviceExtensionsToEnable;
	DeviceExtensionsToEnable.push_back("VK_KHR_swapchain");


	VkDeviceCreateInfo DeviceCreateInfo = {};

	DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceCreateInfo.pNext = NULL;
	//DeviceCreateInfo.flags; Reserved For future use
	DeviceCreateInfo.enabledExtensionCount = DeviceExtensionsToEnable.size();
	DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtensionsToEnable.data();
	//DeviceCreateInfo.enabledLayerCount; Deprecated
	//DeviceCreateInfo.ppEnabledLayerNames; Depreceated
	DeviceCreateInfo.queueCreateInfoCount = 1;
	DeviceCreateInfo.pQueueCreateInfos = &QueueCreateInfo;



	VkResult theResult = vkCreateDevice(mPhysicalDevice, &DeviceCreateInfo, nullptr, &TheVulkanDevice);

	if (theResult != VK_SUCCESS)
	{
		OutputDebugString("Failed to create Device;");
	}
	else
	{
		OutputDebugString("Hurrah! Created a device.");
	}


	vkGetDeviceQueue(TheVulkanDevice, 0, 0, &mQueue);
	vkGetDeviceQueue(TheVulkanDevice, 0, 1, &mPresentQueue);
	CreateCommandPool();
	CreateMemoryManager();
	CreateDescriptorPool();
}






VulkanDevice::~VulkanDevice()
{
	vkDeviceWaitIdle(TheVulkanDevice);

	mCommandPool.reset();
	mMemoryManager.reset();
	mDescriptorPool.reset();
	
	std::lock_guard<std::mutex> lock1(mAvailableImageIndicesArrayLock);
	std::lock_guard<std::mutex> lock2(mPresentableImageIndicesArrayLock);

	for (SyncedPresentable it : mAvailableImageIndicesArray)
	{
		vkDestroySemaphore(TheVulkanDevice, it.mWaitForAcquireSemaphore, nullptr);
		vkDestroySemaphore(TheVulkanDevice, it.mWaitForPresentSemaphore, nullptr);
	}

	for (SyncedPresentable it : mPresentableImageIndicesArray)
	{
		vkDestroySemaphore(TheVulkanDevice, it.mWaitForAcquireSemaphore, nullptr);
		vkDestroySemaphore(TheVulkanDevice, it.mWaitForPresentSemaphore, nullptr);
	}


	vkDestroyDevice(TheVulkanDevice, nullptr);
}





void VulkanDevice::GetDeviceExtensionPointers()
{
	// VK_KHR_swapchain function pointers

	GET_DEVICE_PROC_ADDR(TheVulkanDevice, CreateSwapchainKHR);
	GET_DEVICE_PROC_ADDR(TheVulkanDevice, DestroySwapchainKHR);
	GET_DEVICE_PROC_ADDR(TheVulkanDevice, GetSwapchainImagesKHR);
	GET_DEVICE_PROC_ADDR(TheVulkanDevice, AcquireNextImageKHR);
	GET_DEVICE_PROC_ADDR(TheVulkanDevice, QueuePresentKHR);

}





void VulkanDevice::CheckSurfaceCapabilities()
{
#if ENGINE_LOGGING_ENABLED
	DumpSurfaceInfoToFile();
#else
	VkBool32  PresentingSupported;
	VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, 0, mWindow.GetSurface(), &PresentingSupported);

	memset(&mSurfaceCapabilities, 0, sizeof(VkSurfaceCapabilitiesKHR));

	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice,
		mWindow.GetSurface(),
		&mSurfaceCapabilities);



	uint32_t surfaceFormatCount;
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice,
		mWindow.GetSurface(),
		&surfaceFormatCount,
		nullptr);

	mSurfaceFormats.resize(surfaceFormatCount);

	result = vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice,
		mWindow.GetSurface(),
		&surfaceFormatCount,
		mSurfaceFormats.data());


	uint32_t presentModeCount;
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice,
		mWindow.GetSurface(),
		&presentModeCount,
		nullptr);

	mPresentModes.resize(presentModeCount);
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice,
		mWindow.GetSurface(),
		&presentModeCount,
		mPresentModes.data());

#endif
}





void VulkanDevice::DumpSurfaceInfoToFile()
{
	std::ofstream Log;
	Log.open("SurfaceInfoLog.txt");

	VkBool32  PresentingSupported;
	VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, 0, mWindow.GetSurface(), &PresentingSupported);
	if (result != VK_SUCCESS)
		Log << "vkGetPhysicalDeviceSurfaceSupportKHR call failed!" << std::endl;
	else if (PresentingSupported)
		Log << "Presenting supported! Hooray!" << std::endl;
	else
		Log << "Presenting NOT supported! boo!" << std::endl;


	memset(&mSurfaceCapabilities, 0, sizeof(VkSurfaceCapabilitiesKHR));

	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice,
		mWindow.GetSurface(),
		&mSurfaceCapabilities);

	if (result != VK_SUCCESS)
	{
		Log << "vkGetPhysicalDeviceSurfaceSupportKHR call failed!" << std::endl;
	}
	else
	{
		Log << "Device 0 Capabalities:" << std::endl;
		Log << std::endl;
		Log << "	uint32_t minImageCount: " << mSurfaceCapabilities.minImageCount << std::endl;
		Log << "	uint32_t maxImageCount:	" << mSurfaceCapabilities.maxImageCount << std::endl;
		Log << "	VkExtent2D currentExtent : " << std::endl;
		Log << "		width: " << mSurfaceCapabilities.currentExtent.width << std::endl;
		Log << "		height: " << mSurfaceCapabilities.currentExtent.height << std::endl;
		Log << "	VkExtent2D minImageExtent : " << std::endl;
		Log << "		width: " << mSurfaceCapabilities.minImageExtent.width << std::endl;
		Log << "		height: " << mSurfaceCapabilities.minImageExtent.height << std::endl;
		Log << "	VkExtent2D maxImageExtent : " << std::endl;
		Log << "		width: " << mSurfaceCapabilities.maxImageExtent.width << std::endl;
		Log << "		height: " << mSurfaceCapabilities.maxImageExtent.height << std::endl;
		Log << "	uint32_t maxImageArrayLayers: " << mSurfaceCapabilities.maxImageArrayLayers << std::endl;


		Log << "	VkSurfaceTransformFlagBitsKHR supportedTransforms: " << mSurfaceCapabilities.supportedTransforms << std::endl;
		Log << "	VkSurfaceTransformFlagBitsKHR currentTransform: " << mSurfaceCapabilities.currentTransform << std::endl;
		Log << "	VkCompositeAlphaFlagsKHR supportedCompositeAlpha: " << mSurfaceCapabilities.supportedCompositeAlpha << std::endl;
		Log << "	VkImageUsageFlags supportedUsageFlags: " << mSurfaceCapabilities.supportedUsageFlags << std::endl;
	}

	uint32_t surfaceFormatCount;
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice,
		mWindow.GetSurface(),
		&surfaceFormatCount,
		nullptr);

	mSurfaceFormats.resize(surfaceFormatCount);

	result = vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice,
		mWindow.GetSurface(),
		&surfaceFormatCount,
		mSurfaceFormats.data());


	if (result != VK_SUCCESS)
	{
		Log << "vkGetPhysicalDeviceSurfaceFormatsKHR call failed!" << std::endl;
	}
	else
	{
		for (uint32_t format = 0; format < surfaceFormatCount; format++)
		{
			Log << std::endl;
			Log << "Device 0 formats:	" << std::endl;
			Log << "	format	:		" << mSurfaceFormats[format].format << std::endl;
			Log << "	Colour Space:	" << mSurfaceFormats[format].colorSpace << std::endl;
		}

	}


	uint32_t presentModeCount;
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice,
		mWindow.GetSurface(),
		&presentModeCount,
		nullptr);

	mPresentModes.resize(presentModeCount);
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice,
		mWindow.GetSurface(),
		&presentModeCount,
		mPresentModes.data());

	if (result != VK_SUCCESS)
	{
		Log << "vkGetPhysicalDeviceSurfacePresentModesKHR call failed!" << std::endl;
	}
	else
	{
		for (uint32_t mode = 0; mode < surfaceFormatCount; mode++)
		{
			Log << std::endl;
			Log << "Device 0 Present modes:	" << std::endl;
			Log << "	mode	:		" << mPresentModes[mode] << std::endl;
		}

	}

	Log.close();
}





void VulkanDevice::PopulatePresentableImages(VkImage * ImageArray, uint32_t size, uint32_t width, uint32_t height)
{
	for (uint32_t i = 0; i < size; i++)
	{
		mPresentableImageArray.emplace_back(VulkanImage(this, ImageArray[i], width, height, true));
	}
}





void VulkanDevice::CreateCommandPool()
{
	mCommandPool = std::make_unique<CommandPool>(this);
}





void VulkanDevice::CreateMemoryManager()
{
	mMemoryManager = std::make_unique<VulkanMemMngr>(this);
}





void VulkanDevice::CreateDescriptorPool()
{
	mDescriptorPool = std::make_unique<DescriptorPool>(*this);
}





void VulkanDevice::AddToAvailableQueue(SyncedPresentable freeImage)
{
	mPresentableImageArray[freeImage.mEngineImageIndex].SetLayout(VK_IMAGE_LAYOUT_UNDEFINED);

	std::lock_guard<std::mutex> lock(mAvailableImageIndicesArrayLock);
	mAvailableImageIndicesArray.push_back(freeImage);
}





bool VulkanDevice::GetFromAvailableQueue( SyncedPresentable& nextPresentable)
{
	bool imageAvailable = false;

	std::lock_guard<std::mutex> lock(mAvailableImageIndicesArrayLock);

	if (!mAvailableImageIndicesArray.empty())
	{
		nextPresentable = mAvailableImageIndicesArray.front();
		mAvailableImageIndicesArray.pop_front();
		imageAvailable = true;
	}

	return imageAvailable;
}





void VulkanDevice::AddToPresentQueue(SyncedPresentable addPresentable)
{
	std::lock_guard<std::mutex> lock(mPresentableImageIndicesArrayLock);

	mPresentableImageIndicesArray.push_back(addPresentable);
}





bool VulkanDevice::GetFromPresentQueue(SyncedPresentable& nextPresentable)
{
	bool presentableAvailable = false;

	std::lock_guard<std::mutex> lock(mPresentableImageIndicesArrayLock);

	if (!mPresentableImageIndicesArray.empty())
	{
		nextPresentable = mPresentableImageIndicesArray.front();
		mPresentableImageIndicesArray.pop_front();
		presentableAvailable = true;
	}

	return presentableAvailable;
}







bool VulkanDevice::BeginFrame()
{
	bool isPresentableReady = GetFromAvailableQueue(mNextPresentable);

	if (!isPresentableReady)
		return false;

	mNextImage = mNextPresentable.mEngineImageIndex;

	static uint32_t nextRenderTarget = 0;

	//mRenderTarget = mColourImages[nextRenderTarget];
	//mCurrentRenderPass = mRenderPasses[nextRenderTarget];
	nextRenderTarget++;

	if (nextRenderTarget == 4)
		nextRenderTarget = 0;

	return true;
}
	




void VulkanDevice::Present()
{
	CommandBuffer * currentCommandBuffer = mCommandPool->GetCurrentCommandBuffer();

	// Finished render work, do boilerplate
	VkSemaphore pendingSignal = currentCommandBuffer->GetCompleteSignal();

	// Blit to presentable Image.
	currentCommandBuffer = mCommandPool->NextCmdBuffer();

	mPresentableImageArray[mNextImage].BlitFullImage(*mPresentationImage);
	currentCommandBuffer->GetImageReadyForPresenting(mPresentableImageArray[mNextImage]);

	// Add semaphores that blit to backbuffer needs to wait for.
	currentCommandBuffer->AddWaitSignal(pendingSignal);
	currentCommandBuffer->AddWaitSignal(mNextPresentable.mWaitForAcquireSemaphore);

	currentCommandBuffer->AddFinishSignal(mNextPresentable.mWaitForPresentSemaphore);

	// Submit Blit
	mCommandPool->NextCmdBuffer();

	AddToPresentQueue(mNextPresentable);
}


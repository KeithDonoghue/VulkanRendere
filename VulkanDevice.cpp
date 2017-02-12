#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanBuffer.h"
#include "VulkanMemoryManager.h"
#include "DescriptorPool.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "RenderPass.h"
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

	mPipeline.reset();
	mPipeline2.reset();

	mRenderInstance.reset();
	mRenderInstance2.reset();

	for (size_t i = 0; i < mDepthImages.size(); i++)
	{
		delete mDepthImages[i];
		delete mColourImages[i];
		delete mRenderPasses[i];
	}

	delete mVert;
	delete mFrag;
	delete mFrag2;



	delete mImage;
	
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





void  VulkanDevice::CreateRenderTargets(int width, int height)
{
	for (size_t i = 0; i < mPresentableImageArray.size(); i++)
	{
		mDepthImages.emplace_back(new VulkanImage(this, width, height, ImageType::VULKAN_IMAGE_DEPTH));
		mColourImages.emplace_back(new VulkanImage(this, width, height, ImageType::VULKAN_IMAGE_COLOR_RGBA8));
		mRenderPasses.emplace_back(new RenderPass(*this, *mDepthImages[i], *mColourImages[i]));
	}
}






void  VulkanDevice::CreateInitialData()
{
	mImage = new VulkanImage(this, "Resources/jpeg_bad.jpg");



	mVert = ShaderModule::CreateVertexShader(*this);
	mFrag = ShaderModule::CreateFragmentShader(*this);
	mFrag2 = ShaderModule::CreateFragmentShader2(*this);
	mPipeline = std::make_shared<VulkanPipeline>(*this, *mRenderPasses[0], *mVert, *mFrag);
	mPipeline2 = std::make_shared<VulkanPipeline>(*this, *mRenderPasses[0], *mVert, *mFrag2);
	mRenderInstance = std::make_shared<RenderInstance>(*this, mPipeline, *mImage);
	mRenderInstance2 = std::make_shared<RenderInstance>(*this, mPipeline2, *mImage);

	std::shared_ptr<VulkanBuffer> drawbuffer = VulkanBuffer::SetUpVertexBuffer(*this);
	std::shared_ptr<VulkanBuffer> indexDrawbuffer = VulkanBuffer::SetUpVertexIndexBuffer(*this);
	std::shared_ptr<VulkanBuffer> indexbuffer = VulkanBuffer::SetUpIndexBuffer(*this);

	indexbuffer->DoTheImportThing("Resources/cube.dae");


	VertexDraw	theDraw(6, 1, 0, 0, drawbuffer);
	IndexDraw	theIndexDraw(36, 2, 0, 0, 0, indexDrawbuffer, indexbuffer);

	mRenderInstance->SetDraw(theDraw);
	mRenderInstance2->SetDraw(theDraw);

	mRenderInstance->SetDraw(theIndexDraw);
	mRenderInstance2->SetDraw(theIndexDraw);
}





void VulkanDevice::Update()
{

	CommandBuffer * currentCommandBuffer = mCommandPool->GetCurrentCommandBuffer();
	SyncedPresentable nextPresentable;
	bool isPresentableReady = GetFromAvailableQueue(nextPresentable);

	if (!isPresentableReady)
		return;

	uint32_t nextImage = nextPresentable.mEngineImageIndex;


	// Render Work
	mColourImages[nextImage]->ClearImage(1.0f);
	mColourImages[nextImage]->BlitFullImage(*mImage);
	
	DoRender(nextImage);

	// Finished render work, do boilerplate
	VkSemaphore pendingSignal = currentCommandBuffer->GetCompleteSignal();
	currentCommandBuffer->SubmitCommandBuffer();
	mCommandPool->NextCmdBuffer();


	// Blit to presentable Image.
	currentCommandBuffer = mCommandPool->GetCurrentCommandBuffer();

	mPresentableImageArray[nextImage].BlitFullImage(*mColourImages[nextImage]);
	currentCommandBuffer->GetImageReadyForPresenting(mPresentableImageArray[nextImage]);

	// Add semaphores that blit to backbuffer needs to wait for.
	currentCommandBuffer->AddWaitSignal(pendingSignal);
	currentCommandBuffer->AddWaitSignal(nextPresentable.mWaitForAcquireSemaphore);

	currentCommandBuffer->AddFinishSignal(nextPresentable.mWaitForPresentSemaphore);

	// Submit Blit
	currentCommandBuffer->SubmitCommandBuffer();
	mCommandPool->NextCmdBuffer();


	AddToPresentQueue(nextPresentable);
}





void VulkanDevice::TakeInput(unsigned int keyPress)
{
	if (keyPress == 65)
		mRenderInstance->ChangeWorldPosition(1.0f, 0.0f, 0.0f);

	if (keyPress == 66)
		mRenderInstance->ChangeWorldPosition(-1.0f, 0.0f, 0.0f);

	if (keyPress == 67)
		mRenderInstance->ChangeWorldPosition(0.0f, 0.0f, 1.0f);

	if (keyPress == 68)
		mRenderInstance->ChangeWorldPosition(0.0f, 0.0f, -1.0f);

	if (keyPress == 37)
		mRenderInstance->ChangeWorldPosition(0.0f, -1.0f, 0.0f);

	if (keyPress == 39)
		mRenderInstance->ChangeWorldPosition(0.0f, 1.0f, 0.0f);
}





void VulkanDevice::DoRender(uint32_t nextImage)
{

		mRenderInstance->Draw(*mRenderPasses[nextImage]);

		//mRenderInstance2->Draw(*mRenderPasses[nextImage]);
}
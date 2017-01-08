#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanMemoryManager.h"

#include "DescriptorPool.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "RenderPass.h"
#include "ShaderModule.h"
#include "VulkanPipeline.h"




#include "Windows.h"

#define  ENGINE_LOGGING_ENABLED 1
#include "EngineLogging.h"

#include <vector>

VulkanDevice::VulkanDevice(VkPhysicalDevice thePhysicalDevice):
mPhysicalDevice(thePhysicalDevice)
{
	VkDeviceQueueCreateInfo QueueCreateInfo;
	memset(&QueueCreateInfo, 0, sizeof(QueueCreateInfo));

	float queuePriorities= 1.0f;

	QueueCreateInfo.pNext = NULL;
	QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	//QueueCreateInfo.flags; Reserved For future use
	QueueCreateInfo.queueFamilyIndex = 0;
	QueueCreateInfo.queueCount = 1;
	QueueCreateInfo.pQueuePriorities = &queuePriorities;




	std::vector<char *> DeviceExtensionsToEnable;
	DeviceExtensionsToEnable.push_back("VK_KHR_swapchain");


	VkDeviceCreateInfo DeviceCreateInfo;
	memset(&DeviceCreateInfo, 0, sizeof(DeviceCreateInfo));

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
	CreateCommandPool();
	CreateMemoryManager();
	CreateDescriptorPool();
}






VulkanDevice::~VulkanDevice()
{
	vkDeviceWaitIdle(TheVulkanDevice);

	vkDestroySampler(getVkDevice(), theSampler, nullptr);
	vkDestroyImageView(getVkDevice(), theView, nullptr);


	for (size_t i = 0; i < mDepthImages.size(); i++)
	{
		delete mDepthImages[i];
		delete mRenderPasses[i];
	}

	delete mVert;
	delete mFrag;
	delete mPipeline;


	delete mImage;
	delete mCommandPool;
	delete mMemoryManager;
	delete mDescriptorPool;

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





void VulkanDevice::CreateCommandPool()
{
	mCommandPool = new CommandPool(this);
}





void VulkanDevice::PopulatePresentableImages(VkImage * ImageArray, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		mPresentableImageArray.emplace_back(VulkanImage(this, ImageArray[i], true));
	}
}






void VulkanDevice::Update()
{

	CommandBuffer * currentCommandBuffer = mCommandPool->GetCurrentCommandBuffer();
	SyncedPresentable nextPresentable;
	bool isPresentableReady = GetFromAvailableQueue(nextPresentable);

	if (!isPresentableReady)
		return;

	uint32_t nextImage = nextPresentable.mEngineImageIndex;

	if (!inited)
	{
		inited = true;
		VkSamplerCreateInfo  samplerCreateInfo = {};

		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.pNext = NULL;
		samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
		samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.anisotropyEnable = VK_FALSE;
		samplerCreateInfo.maxAnisotropy = 1;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = 0.0f;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

		VkResult result = vkCreateSampler(getVkDevice(), &samplerCreateInfo, nullptr, &theSampler);
		if (result != VK_SUCCESS)
		{
			EngineLog("Failed to create Sampler");
		}
		VkImageViewCreateInfo view = {};
		view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view.pNext = NULL;
		view.image = mImage->getVkImage();
		view.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view.format = mImage->GetFormat();
		view.components =
		{
			VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A,
		};
		view.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		view.flags = 0;

		result = vkCreateImageView(getVkDevice(), &view, nullptr, &theView);
		if (result != VK_SUCCESS)
		{
			EngineLog("Failed to create Sampler");
		}

	}

	mPresentableImageArray[nextImage].ClearImage(1.0f);
	mPresentableImageArray[nextImage].CopyImageData(*mImage);

	currentCommandBuffer->DoDraw(*mRenderPasses[nextImage], *mPipeline, *mImage, theSampler, theView);
	currentCommandBuffer->GetImageReadyForPresenting(mPresentableImageArray[nextImage]);

		
	VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

	VkSubmitInfo theSubmitInfo;
	memset(&theSubmitInfo, 0, sizeof(VkSubmitInfo));
	theSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	theSubmitInfo.pNext = nullptr;
	theSubmitInfo.waitSemaphoreCount = 1;
	theSubmitInfo.pWaitSemaphores = &nextPresentable.mWaitForAcquireSemaphore;
	theSubmitInfo.pWaitDstStageMask = &stageFlags;
	theSubmitInfo.commandBufferCount = 1;
	theSubmitInfo.pCommandBuffers = currentCommandBuffer->GetVkCommandBufferAddr();
	theSubmitInfo.signalSemaphoreCount = 1;
	theSubmitInfo.pSignalSemaphores = &nextPresentable.mWaitForPresentSemaphore;

	currentCommandBuffer->EndCommandBuffer();
	
	LockQueue();
	VkResult  result = vkQueueSubmit(GetVkQueue(), 1, &theSubmitInfo, currentCommandBuffer->GetCompletionFence());
	UnlockQueue();

	mCommandPool->NextCmdBuffer();

	if (result != VK_SUCCESS)
	{
		EngineLog("Queue submission failed.");
	}

	AddToPresentQueue(nextPresentable);
}





void VulkanDevice::CreateMemoryManager()
{
	mMemoryManager = new VulkanMemMngr(this);
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





void  VulkanDevice::CreateInitialData()
{	
	mImage = new VulkanImage(this, 400, 400, ImageType::VULKAN_IMAGE_COLOR_RGBA8);

	mImage->ClearImage(1.0f);
	mImage->LoadDataToImage();


	
	for (size_t i = 0; i < mPresentableImageArray.size(); i++)
	{
		mDepthImages.emplace_back(new VulkanImage(this, 400, 400, ImageType::VULKAN_IMAGE_DEPTH));
		mRenderPasses.emplace_back(new RenderPass(*this, *mDepthImages[i], mPresentableImageArray[i]));		
	}

	mVert = ShaderModule::CreateVertexShader(*this);
	mFrag = ShaderModule::CreateFragmentShader(*this);
	mPipeline = new VulkanPipeline(*this, *mRenderPasses[0], *mVert, *mFrag);
}




void VulkanDevice::CreateDescriptorPool()
{
	mDescriptorPool = new DescriptorPool(*this);
}
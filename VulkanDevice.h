#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H 1

#include "ApiUsageHeader.h"

#include "WindowsWindow.h"

#include <vector>
#include <deque>
#include <mutex>
#include <memory>




#ifdef _WIN32
#define ERR_EXIT(err_msg, err_class)                                           \
    do {                                                                       \
            MessageBox(NULL, err_msg, err_class, MB_OK);                       \
        exit(1);                                                               \
			    } while (0)
#endif




#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                                  \
			    {                                                                          \
        fp##entrypoint =                                                 \
            (PFN_vk##entrypoint)vkGetDeviceProcAddr(dev, "vk" #entrypoint);    \
			if(fp##entrypoint == NULL)		\
							{\
					ERR_EXIT("vkGetDeviceProcAddr failed to find vk" #entrypoint,    \
                     "vkGetDeviceProcAddr Failure");                         \
							}\
			    }

class CommandPool;
class DescriptorPool;
class VulkanImage;
class VulkanMemMngr;
class VulkanRenderPass;
class ShaderModule;
class VulkanPipeline;
class RenderInstance;


typedef struct SyncedPresentable{
	uint32_t	mEngineImageIndex;
	VkSemaphore mWaitForPresentSemaphore;
	VkSemaphore mWaitForAcquireSemaphore;
} SyncedPresentable;


class VulkanDevice
{
public:
	VulkanDevice(VkPhysicalDevice, EngineWindow&);
	~VulkanDevice();

	void CheckSurfaceCapabilities();
	const VkSurfaceCapabilitiesKHR GetSurfaceCapabilities() const { return mSurfaceCapabilities;  }
	void DumpSurfaceInfoToFile();

	VkDevice getVkDevice()
	{
		return TheVulkanDevice;
	}

	VkDevice GetVkDevice()
	{
		return TheVulkanDevice;
	}

	VkPhysicalDevice GetVkPhysicalDevice()
	{
		return mPhysicalDevice;
	}

	VkQueue GetVkQueue()
	{
		return mQueue;
	}
	
	VkQueue GetVkPresentQueue()
	{
		return mPresentQueue;
	}

	CommandPool& GetCommandPool() { return *mCommandPool; }
	DescriptorPool& GetDescriptorPool() { return *mDescriptorPool; }

	void GetDeviceExtensionPointers();
	void CreateCommandPool();
	void CreateMemoryManager();
	void PopulatePresentableImages(VkImage *, uint32_t, uint32_t, uint32_t);
	void AddToAvailableQueue(SyncedPresentable);
	VulkanMemMngr& GetMemManager() { return *mMemoryManager;  };

	bool GetFromAvailableQueue(SyncedPresentable&);
	void AddToPresentQueue(SyncedPresentable);
	bool GetFromPresentQueue(SyncedPresentable&);
	void CreateInitialData();
	void CreateRenderTargets(int, int);
	void SetPresImage(VulkanImage * theImage)
	{
		mPresentationImage = theImage;
	}

	bool BeginFrame();
	void Present();
	void DoRendering();
	void TakeInput(unsigned int);


	void CreateDescriptorPool();


	void LockQueue()	{ mQueueLock.lock() ; }
	void UnlockQueue()	{ mQueueLock.unlock() ; }

	void LockPresentQueue()	{ mPresentQueueLock.lock(); }
	void UnlockPresentQueue()	{ mPresentQueueLock.unlock(); }

private:
	void init();

private:
	VkDevice			TheVulkanDevice;
	VkPhysicalDevice	mPhysicalDevice;
	EngineWindow&		mWindow;
	
	VkSurfaceCapabilitiesKHR		mSurfaceCapabilities;
	std::vector<VkSurfaceFormatKHR>	mSurfaceFormats;
	std::vector<VkPresentModeKHR>	mPresentModes;

	VkQueue mQueue;
	std::mutex mQueueLock;

	VkQueue mPresentQueue;
	std::mutex mPresentQueueLock;

	VkDeviceQueueCreateInfo mQueueCreateInfo;
	VkDeviceCreateInfo mCreateInfo;

	VulkanImage * mImage, *mRenderTarget;
	VulkanRenderPass * mCurrentRenderPass;

	std::vector<VulkanImage*> mDepthImages;
	std::vector<VulkanImage*> mColourImages;
	std::vector<VulkanRenderPass*> mRenderPasses;

	std::unique_ptr<DescriptorPool> mDescriptorPool;
	std::unique_ptr<CommandPool>	mCommandPool;
	std::unique_ptr<VulkanMemMngr> mMemoryManager;


	std::shared_ptr<ShaderModule> mVert;
	std::shared_ptr<ShaderModule> mFrag, mFrag2;
	std::shared_ptr<VulkanPipeline> mPipeline, mPipeline2;
	std::shared_ptr<RenderInstance> mRenderInstance, mRenderInstance2;

	VulkanImage * mPresentationImage;
	uint32_t mNextImage;
	SyncedPresentable mNextPresentable;



	std::vector<VulkanImage>			mPresentableImageArray;
	std::deque<SyncedPresentable>		mAvailableImageIndicesArray;
	std::mutex							mAvailableImageIndicesArrayLock;

	std::deque<SyncedPresentable>		mPresentableImageIndicesArray;
	std::mutex							mPresentableImageIndicesArrayLock;

	// VK_KHR_swapchain function pointers

	PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
	PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
	PFN_vkQueuePresentKHR fpQueuePresentKHR;



	VkSampler theSampler;
	VkImageView theView;
	bool inited = false;

};
#endif
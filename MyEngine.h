#ifndef MY_ENGINE_HDR 
#define MY_ENGINE_HDR 1

#include "ApiUsageHeader.h"
#include "ShaderModule.h"
#include "EngineImage.h"

#include "Windows.h"


#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <atomic>
#include <memory>

class EngineWindow;
class Swapchain;
class VulkanDevice;
class VulkanRenderPass;
class VulkanPipeline;
class RenderInstance;
class VulkanBuffer;


#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                               \
	    {                                                                          \
        vk##entrypoint =                                                 \
            (PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint); \
			if(vk##entrypoint == NULL)		\
			{\
			ERR_EXIT("vkGetInstanceProcAddr failed to find vk" #entrypoint,    \
                     "vkGetInstanceProcAddr Failure");                         \
			}\
	    }



class MyEngine{
public:
	MyEngine();
	~MyEngine();
	void InitLoader();
	void InitLayersAndExtensions();
	void SetWindowOffsetAndSize(int, int, int, int);
	void CreateVulkanInstance();
	void SelectPhysicalDevice();
	void CreateDevice();
	void GetInstanceExtensionPointers();
	void DoPhysicalDeviceStuff();
	void SetUpDebugReportStuff();
	void DestroyDebugReportStuff();
	void CreateSwapchain();
	void SpawnUpdateThread();
	void Run();
	void Update();
	void TakeInput(unsigned int);


	void  CreateRenderTargets(int, int, uint32_t);
	void  MyEngine::CreateInitialData();

	EngineImage * CreateEngineImage(std::string);
	void SetImage(EngineImage&);
	void SetUpTargets();




	std::string	GetName()	{ return name; }
	void		CreateGameWindow(HINSTANCE);
	void		FinishedFrameWork();
	void		PrintLocation(char * file = __FILE__){ OutputDebugString(file); }

private:
	std::string name;

	std::ofstream mLogFile;
	std::ofstream mInformationLogFile;
	std::ofstream mMemLogFile;
	std::ofstream mLoaderLogFile;
	std::ofstream mErrorLogFile;
	std::ofstream mPerfLogFile;


	std::thread mRenderThread;
	std::atomic<bool> mFinish;

	EngineWindow *	m_TheWindow;
	Swapchain * m_TheSwapchain;
	VulkanDevice *  mVulkanDevice;

	std::vector<VulkanImage*> mDepthImages;
	std::vector<VulkanImage*> mColourImages;
	std::vector<VulkanRenderPass*> mRenderPasses;

	VulkanImage * mRenderTarget;
	VulkanRenderPass * mCurrentRenderPass;


	std::vector<std::shared_ptr<ShaderModule>> mShaders;
	EngineImage * mImage;

	std::shared_ptr<VulkanPipeline> mPipeline, mPipeline2;
	std::shared_ptr<RenderInstance> mRenderInstance, mRenderInstance2;

	int mWindowWidth, mWindowHeight;

	std::vector<std::string>		m_AvailableExtensionNames;
	std::vector<std::string>		m_AvailableLayerNames;

	std::vector<std::string>		m_EnabledInstanceExtensions;
	std::vector<std::string>		m_EnabledInstanceLayers;


	VkInstance  TheVulkanInstance ;

	std::vector<VkPhysicalDevice>		m_AvailablePhysicalDevices;
	VkPhysicalDeviceProperties			mDeviceProperties;


	VkDebugReportCallbackEXT m_callback;
	VkDebugReportCallbackEXT m_InformationCallback;

	friend VKAPI_ATTR VkBool32 VKAPI_CALL FirstAllPurposeDebugReportCallback(
		VkDebugReportFlagsEXT       flags,
		VkDebugReportObjectTypeEXT  objectType,
		uint64_t                    object,
		size_t                      location,
		int32_t                     messageCode,
		const char*                 pLayerPrefix,
		const char*                 pMessage,
		void*                       pUserData);
};


VKAPI_ATTR VkBool32 VKAPI_CALL FirstAllPurposeDebugReportCallback(
	VkDebugReportFlagsEXT       flags,
	VkDebugReportObjectTypeEXT  objectType,
	uint64_t                    object,
	size_t                      location,
	int32_t                     messageCode,
	const char*                 pLayerPrefix,
	const char*                 pMessage,
	void*                       pUserData);


#endif // MY_ENGINE_HDR 

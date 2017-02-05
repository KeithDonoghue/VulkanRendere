#ifndef MY_ENGINE_HDR 
#define MY_ENGINE_HDR 1


#define VK_USE_PLATFORM_WIN32_KHR 1

#include <vulkan/vulkan.h>

#include "Windows.h"


#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <atomic>

class EngineWindow;
class Swapchain;
class VulkanDevice;

#ifdef _WIN32
#define ERR_EXIT(err_msg, err_class)                                           \
    do {                                                                       \
            MessageBox(NULL, err_msg, err_class, MB_OK);                       \
        exit(1);                                                               \
		    } while (0)
#endif

#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                               \
	    {                                                                          \
        fp##entrypoint =                                                 \
            (PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint); \
			if(fp##entrypoint == NULL)		\
			{\
			ERR_EXIT("vkGetInstanceProcAddr failed to find vk" #entrypoint,    \
                     "vkGetInstanceProcAddr Failure");                         \
			}\
	    }


class MyEngine{
public:
	MyEngine();
	~MyEngine();
	void InitLayersAndExtensions();
	void SetWindowOffsetAndSize(int, int, int, int);
	void CreateVulkanInstance();
	void SelectPhysicalDevice();
	void CreateDevice();
	void GetInstanceExtensionPointers();
	void DoPhysicalDeviceStuff();
	void SetUpDebugReportStuff();
	void DestroyDebugReportStuff();
	void GetSurfaceCapabilities();
	void DumpSurfaceInfoToFile();
	void CreateSwapchain();
	void SpawnUpdateThread();
	void Run();
	void Update();
	void TakeInput(unsigned int);



	std::string	GetName()	{ return name; }
	void		CreateGameWindow(HINSTANCE);
	void		FinishedFrameWork();
	void PrintLocation(char * file = __FILE__){ OutputDebugString(file); }

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

	int mWindowWidth, mWindowHeight;

	std::vector<std::string>		m_AvailableExtensionNames;
	std::vector<std::string>		m_AvailableLayerNames;

	std::vector<std::string>		m_EnabledInstanceExtensions;
	std::vector<std::string>		m_EnabledInstanceLayers;

	VkSurfaceCapabilitiesKHR		mSurfaceCapabilities;

	VkInstance  TheVulkanInstance ;

	std::vector<VkPhysicalDevice>		m_AvailablePhysicalDevices;

	// EXT_debug_report function pointers

	PFN_vkCreateDebugReportCallbackEXT	fpCreateDebugReportCallback;
	PFN_vkDebugReportMessageEXT			fpDebugReportMessage;
	PFN_vkDestroyDebugReportCallbackEXT	fpDestroyDebugReportCallback;

	// VK_KHR_surface function pointers

	PFN_vkGetPhysicalDeviceSurfaceSupportKHR
		fpGetPhysicalDeviceSurfaceSupportKHR;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR
		fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR
		fpGetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR
		fpGetPhysicalDeviceSurfacePresentModesKHR;


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

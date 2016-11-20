#include "MyEngine.h"
#include "ApiUsageHeader.h"
#define VK_USE_PLATFORM_WIN32_KHR 1

#include "EngineLogging.h"


#include "WindowsWindow.h"
#include "Swapchain.h"
#include "VulkanDevice.h"

#pragma comment(linker, "/subsystem:windows")
#include <Windows.h>


#include <cstdlib>
#include <algorithm>
#include <iostream> 
#include <fstream>


// MS-Windows event handling function:

MyEngine::MyEngine() :name("TheEngine")
{
	mLogFile.open(name + "Log.txt");
	mInformationLogFile.open(name + "ObjectLog.txt");
	mInformationLogFile << "Creating" << std::endl;
}


MyEngine::~MyEngine()
{
	delete m_TheSwapchain;
	delete m_TheWindow;
	delete m_TheVulkanDevice;

	DestroyDebugReportStuff();
	vkDestroyInstance(TheVulkanInstance, nullptr);
	mLogFile.close();
	mInformationLogFile.close();
	EngineLog("44", 6);
}

void MyEngine::SetWindowOffsetAndSize(int x, int y, int width, int height)
{
	if (m_TheWindow == nullptr)
	{
		m_TheWindow = new EngineWindow(x, y, width, height);
	}
}



void MyEngine::GetInstanceExtensionPointers()
{
	// EXT_debug_report function pointers


	fpCreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(TheVulkanInstance, "vkCreateDebugReportCallbackEXT");
	fpDebugReportMessage = (PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(TheVulkanInstance, "vkDebugReportMessageEXT");
	fpDestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(TheVulkanInstance, "vkDestroyDebugReportCallbackEXT");


	// VK_KHR_surface function pointers

	
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetPhysicalDeviceSurfaceSupportKHR);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetPhysicalDeviceSurfaceCapabilitiesKHR)
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetPhysicalDeviceSurfaceFormatsKHR);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetPhysicalDeviceSurfacePresentModesKHR);
}





void MyEngine::SetUpDebugReportStuff()
{
	/* Setup callback creation information */



	VkDebugReportCallbackCreateInfoEXT DebugCallbackCreateinfo;
	DebugCallbackCreateinfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		DebugCallbackCreateinfo.pNext = nullptr;
		DebugCallbackCreateinfo.flags = VK_DEBUG_REPORT_DEBUG_BIT_EXT |
			VK_DEBUG_REPORT_WARNING_BIT_EXT |
			VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
			VK_DEBUG_REPORT_ERROR_BIT_EXT |
			VK_DEBUG_REPORT_DEBUG_BIT_EXT;
			DebugCallbackCreateinfo.pfnCallback = &FirstAllPurposeDebugReportCallback;
		DebugCallbackCreateinfo.pUserData = &mLogFile;

	/* Register the callback */
	VkResult result = fpCreateDebugReportCallback(TheVulkanInstance, &DebugCallbackCreateinfo, nullptr, &m_callback);
	if (result != VK_SUCCESS)
	{
		OutputDebugString("Didn't succeed\n");
	}

	DebugCallbackCreateinfo.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT;
	DebugCallbackCreateinfo.pUserData = &mInformationLogFile;



	/* Register the callback */
	result = fpCreateDebugReportCallback(TheVulkanInstance, &DebugCallbackCreateinfo, nullptr, &m_InformationCallback);
	if (result != VK_SUCCESS)
	{
		OutputDebugString("Didn't succeed\n");
	}
}

void MyEngine::DestroyDebugReportStuff()
{
	fpDestroyDebugReportCallback(TheVulkanInstance, m_callback, nullptr);
	fpDestroyDebugReportCallback(TheVulkanInstance, m_InformationCallback, nullptr);


}


void MyEngine::SelectPhysicalDevice()
{

	int DeviceIndex = -1;
	for (auto it : m_AvailablePhysicalDevices)
	{
		uint32_t PropertyCount, LayerCount;
		vkEnumerateDeviceExtensionProperties(it, NULL, &PropertyCount, NULL);
		VkExtensionProperties * ExtensionProperties = (VkExtensionProperties*)malloc(PropertyCount*sizeof(VkExtensionProperties));
		vkEnumerateDeviceExtensionProperties(it, NULL, &PropertyCount, ExtensionProperties);
		for (int i = 0; i < PropertyCount; i++)
		{
			char str[VK_MAX_EXTENSION_NAME_SIZE + 256];
			sprintf_s(str, "Device Extension name: %s spec num: %d\n", ExtensionProperties[i].extensionName, ExtensionProperties[i].specVersion);
			OutputDebugString(str);
		}
		uint32_t FamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(it, &FamilyCount, NULL);
		VkQueueFamilyProperties * FamilyProperties = (VkQueueFamilyProperties *)malloc(FamilyCount*sizeof(VkQueueFamilyProperties));
		vkGetPhysicalDeviceQueueFamilyProperties(it, &FamilyCount, FamilyProperties);
		OutputDebugString("Device\n");
		for (int i = 0; i < FamilyCount; i++)
		{
			OutputDebugString("Queue Family specs\n");
			if (FamilyProperties[i].queueFlags && VK_QUEUE_COMPUTE_BIT)
				OutputDebugString("Compute supported\n");
			if (FamilyProperties[i].queueFlags && VK_QUEUE_GRAPHICS_BIT)
				OutputDebugString("Graphics supported\n");
			if (FamilyProperties[i].queueFlags && VK_QUEUE_TRANSFER_BIT)
				OutputDebugString("Transfer supported\n");
			if (FamilyProperties[i].queueFlags && VK_QUEUE_SPARSE_BINDING_BIT)
				OutputDebugString("Sparse Binding supported\n");


			char str[256];
			sprintf_s(str, "Supports %d queues.\n", FamilyProperties[i].queueCount);
			OutputDebugString(str);

			OutputDebugString("Do we support Presenting (platform agnostic)?\n");
			VkBool32  PresentingSupported;
			VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(it, i, m_TheWindow->GetSurface(), &PresentingSupported);
			if (result != VK_SUCCESS)
				OutputDebugString("Call failed!\n");
			else if (PresentingSupported)
				OutputDebugString("Presenting supported! Hooray!\n");
			else
				OutputDebugString("Presenting NOT supported. Booo\n");

			OutputDebugString("Do we support Presenting? (Platform specific)\n");
			VkBool32  ExtraPresentingSupport = vkGetPhysicalDeviceWin32PresentationSupportKHR(it, i);

			if (ExtraPresentingSupport)
				OutputDebugString("Presenting supported! Hooray!\n");
			else
				OutputDebugString("Presenting NOT supported. Booo\n");
		}
	
	}
}

void MyEngine::InitLayersAndExtensions()
{

	uint32_t LayerCount;

	VkResult theResult = vkEnumerateInstanceLayerProperties(&LayerCount, NULL);
	VulkanResultString(theResult);

	
	char str[VK_MAX_DESCRIPTION_SIZE];
	sprintf_s(str, "NumLayers: %d\n", LayerCount);
	OutputDebugString(str);

	VkLayerProperties * theProperties = (VkLayerProperties*) malloc (LayerCount*sizeof(VkLayerProperties));

	theResult = vkEnumerateInstanceLayerProperties(&LayerCount, theProperties);
	VulkanResultString(theResult);

	for (int i = 0; i < LayerCount; i++)
	{
		m_AvailableLayerNames.push_back(theProperties[i].layerName);
		sprintf_s(str, "Layer Number %d: %s\n", i, theProperties[i].layerName);
		OutputDebugString(str);
	}


		uint32_t extensionCount;
		theResult = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
		sprintf_s(str, "Extension Count: %d\n", extensionCount);
		OutputDebugString(str);


		VkExtensionProperties * extensionProperties = (VkExtensionProperties*)malloc(extensionCount* sizeof(VkExtensionProperties));
		theResult = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensionProperties);

		for (int j = 0; j < extensionCount; j++)
		{
			m_AvailableExtensionNames.push_back(extensionProperties[j].extensionName);
			char tempString[VK_MAX_EXTENSION_NAME_SIZE];
			sprintf_s(tempString, "Extension %d: %s\n", j, extensionProperties[j].extensionName);
			OutputDebugString(tempString);
		}
	
		OutputDebugString("Going through layer extensions.\n");
		for (int i = 0; i < LayerCount; i++)
		{
			theResult = vkEnumerateInstanceExtensionProperties(theProperties[i].layerName , &extensionCount, NULL);
			VkExtensionProperties * LayerExtensionProperties = (VkExtensionProperties*)malloc(extensionCount* sizeof(VkExtensionProperties));
			theResult = vkEnumerateInstanceExtensionProperties(theProperties[i].layerName, &extensionCount, LayerExtensionProperties);
			OutputDebugString(theProperties[i].layerName);
			OutputDebugString(" ");
			for (int j = 0; j < extensionCount; j++)
			{
				char tempString[VK_MAX_EXTENSION_NAME_SIZE];
				sprintf_s(tempString, "Extension %d: %s\n", j, LayerExtensionProperties[j].extensionName);
				if (std::find(m_AvailableExtensionNames.begin(), m_AvailableExtensionNames.end(), LayerExtensionProperties[j].extensionName)
					 == m_AvailableExtensionNames.end())
				{
					sprintf_s(tempString, "Adding Extension %d: %s\n", j, LayerExtensionProperties[j].extensionName);

					m_AvailableExtensionNames.push_back(LayerExtensionProperties[j].extensionName);
				}
				OutputDebugString(tempString);
			}
		}
}


void MyEngine::CreateVulkanInstance()
{

	std::vector<const char*> UsingExtensions;
	std::vector<const char*> UsingLayers;



	/*
	for (int i = 0; i < m_AvailableLayerNames.size(); i++)
	UsingLayers.push_back(m_AvailableLayerNames[i].c_str());

	for (int i = 0; i < m_AvailableExtensionNames.size(); i++)
	UsingExtensions.push_back(m_AvailableExtensionNames[i].c_str());
	*/
	UsingLayers.push_back("VK_LAYER_LUNARG_api_dump");

	UsingLayers.push_back("VK_LAYER_LUNARG_standard_validation");


	UsingExtensions.push_back("VK_KHR_win32_surface");
	UsingExtensions.push_back("VK_KHR_surface");
	UsingExtensions.push_back("VK_EXT_debug_report");




	VkInstanceCreateInfo InstanceCreationInfo;
	memset(&InstanceCreationInfo, 0, sizeof(InstanceCreationInfo));
	InstanceCreationInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstanceCreationInfo.pNext = NULL;
	InstanceCreationInfo.enabledExtensionCount = UsingExtensions.size();
	InstanceCreationInfo.enabledLayerCount = UsingLayers.size();
	InstanceCreationInfo.ppEnabledExtensionNames = (const char* const*)UsingExtensions.data();
	InstanceCreationInfo.ppEnabledLayerNames = (const char * const *)UsingLayers.data();
	InstanceCreationInfo.pApplicationInfo = NULL;

	VkResult theResult = VK_SUCCESS;
	theResult = vkCreateInstance(&InstanceCreationInfo, nullptr, &TheVulkanInstance);
	if (theResult != VK_SUCCESS)
	{
		char str[256];
		sprintf_s(str, "Couldn't Create Vulkan instance %d\n", theResult);
		OutputDebugString(str);

	}


}

void MyEngine::DoPhysicalDeviceStuff()
{
	uint32_t DeviceCount;
	VkResult theResult = vkEnumeratePhysicalDevices(TheVulkanInstance, &DeviceCount, NULL);

	VkPhysicalDevice * PhysicalDeviceList = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * DeviceCount);

	theResult = vkEnumeratePhysicalDevices(TheVulkanInstance, &DeviceCount, PhysicalDeviceList);
	{
		char str[256];
		sprintf_s(str, "Number of Physical Devices: %d\n", DeviceCount);
		OutputDebugString(str);
	}

	for (int i = 0; i < DeviceCount; i++)
	{
		m_AvailablePhysicalDevices.push_back(PhysicalDeviceList[i]);
		VkPhysicalDeviceProperties DeviceProperties;
		vkGetPhysicalDeviceProperties(PhysicalDeviceList[i], &DeviceProperties);
		char str[256];
		uint32_t version = DeviceProperties.apiVersion;
		sprintf_s(str, "Version Major: %d Version minor: %d\n Device Type: %d \n Driver Version: %d\nDevice Name: %s\n", 
			VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), DeviceProperties.deviceType, DeviceProperties.driverVersion,
			DeviceProperties.deviceName);
		OutputDebugString(str);

	}
}





void MyEngine::CreateDevice()
{
	m_TheVulkanDevice = new VulkanDevice(m_AvailablePhysicalDevices[0]);

}





void MyEngine::CreateGameWindow(HINSTANCE Appinstance)
{
#ifdef _WIN32	
	m_TheWindow->Initialize(Appinstance);
	m_TheWindow->InitializeSurface(TheVulkanInstance);
#endif
}

void MyEngine::FinishedFrameWork()
{
	m_TheWindow->Update();
}


void MyEngine::GetSurfaceCapabilities()
{

}


void MyEngine::DumpSurfaceInfoToFile()
{
	std::ofstream Log;
	Log.open("SurfaceInfoLog.txt");

	VkBool32  PresentingSupported;
	VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(m_AvailablePhysicalDevices[0], 0, m_TheWindow->GetSurface(), &PresentingSupported);
	if (result != VK_SUCCESS)
		Log << "vkGetPhysicalDeviceSurfaceSupportKHR call failed!" << std::endl;
	else if (PresentingSupported)
		Log << "Presenting supported! Hooray!" << std::endl;
	else
		Log << "Presenting NOT supported! boo!" << std::endl;


	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	memset(&surfaceCapabilities, 0, sizeof(VkSurfaceCapabilitiesKHR));

	result = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(m_AvailablePhysicalDevices[0],
		m_TheWindow->GetSurface(),
		&surfaceCapabilities);

	if (result != VK_SUCCESS)
	{
		Log << "vkGetPhysicalDeviceSurfaceSupportKHR call failed!" << std::endl;
	}
	else
	{
		Log << "Device 0 Capabalities:" << std::endl;
		Log << std::endl;
		Log << "	uint32_t minImageCount: " << surfaceCapabilities.minImageCount << std::endl;
		Log << "	uint32_t maxImageCount:	" << surfaceCapabilities.maxImageCount << std::endl;
		Log << "	VkExtent2D currentExtent : " << std::endl;
		Log << "		width: " << surfaceCapabilities.currentExtent.width << std::endl;
		Log << "		height: " << surfaceCapabilities.currentExtent.height << std::endl;
		Log << "	VkExtent2D minImageExtent : " << std::endl;
		Log << "		width: " << surfaceCapabilities.minImageExtent.width << std::endl;
		Log << "		height: " << surfaceCapabilities.minImageExtent.height << std::endl;
		Log << "	VkExtent2D maxImageExtent : " << std::endl;
		Log << "		width: " << surfaceCapabilities.maxImageExtent.width << std::endl;
		Log << "		height: " << surfaceCapabilities.maxImageExtent.height << std::endl;
		Log << "	uint32_t maxImageArrayLayers: " << surfaceCapabilities.maxImageArrayLayers << std::endl;


		Log << "	VkSurfaceTransformFlagBitsKHR supportedTransforms: " << surfaceCapabilities.supportedTransforms << std::endl;
		Log << "	VkSurfaceTransformFlagBitsKHR currentTransform: " << surfaceCapabilities.currentTransform << std::endl;
		Log << "	VkCompositeAlphaFlagsKHR supportedCompositeAlpha: " << surfaceCapabilities.supportedCompositeAlpha << std::endl;
		Log << "	VkImageUsageFlags supportedUsageFlags: " << surfaceCapabilities.supportedUsageFlags << std::endl;
	}

	uint32_t surfaceFormatCount;
	result = fpGetPhysicalDeviceSurfaceFormatsKHR(m_AvailablePhysicalDevices[0],
		m_TheWindow->GetSurface(),
		&surfaceFormatCount,
		nullptr);

	VkSurfaceFormatKHR * surfaceFormats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR)* surfaceFormatCount);

	result = fpGetPhysicalDeviceSurfaceFormatsKHR(m_AvailablePhysicalDevices[0],
		m_TheWindow->GetSurface(),
		&surfaceFormatCount,
		surfaceFormats);


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
			Log << "	format	:		" << surfaceFormats[format].format << std::endl;
			Log << "	Colour Space:	" << surfaceFormats[format].colorSpace << std::endl;
		}

	}

	delete surfaceFormats;

	uint32_t presentModeCount;
	result = fpGetPhysicalDeviceSurfacePresentModesKHR(m_AvailablePhysicalDevices[0],
		m_TheWindow->GetSurface(),
		&presentModeCount,
		nullptr);

	VkPresentModeKHR * presentModes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR)*presentModeCount);
	result = fpGetPhysicalDeviceSurfacePresentModesKHR(m_AvailablePhysicalDevices[0],
		m_TheWindow->GetSurface(),
		&presentModeCount,
		presentModes);

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
			Log << "	mode	:		" << presentModes[mode] << std::endl;
		}

	}

	delete presentModes;

	Log.close();
}





void MyEngine::CreateSwapchain()
{

	m_TheSwapchain = new Swapchain(m_TheVulkanDevice, m_TheWindow);
	m_TheWindow->SetUpSwapChain(m_TheSwapchain);
}





VKAPI_ATTR VkBool32 VKAPI_CALL FirstAllPurposeDebugReportCallback(
	VkDebugReportFlagsEXT       flags,
	VkDebugReportObjectTypeEXT  objectType,
	uint64_t                    object,
	size_t                      location,
	int32_t                     messageCode,
	const char*                 pLayerPrefix,
	const char*                 pMessage,
	void*                       pUserData)
{

	std::ofstream * TheLogFile = static_cast<std::ofstream*>(pUserData);

	*TheLogFile << std::endl << "Hello" << std::endl;
	switch (flags)
	{
		case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
		{
			*TheLogFile << "Inside FirstAllPurposeDebugReportCallback VK_DEBUG_REPORT_INFORMATION_BIT_EXT" << std::endl;
			break;
		}
		case VK_DEBUG_REPORT_WARNING_BIT_EXT:
		{
			*TheLogFile << "Inside FirstAllPurposeDebugReportCallback VK_DEBUG_REPORT_WARNING_BIT_EXT" << std::endl;
			break;
		}
		case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
		{
			*TheLogFile << "Inside FirstAllPurposeDebugReportCallback VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT" << std::endl;
			break;
		}
		case VK_DEBUG_REPORT_ERROR_BIT_EXT:
		{
			*TheLogFile << "Inside FirstAllPurposeDebugReportCallback VK_DEBUG_REPORT_ERROR_BIT_EXT" << std::endl;
			DebugBreak();
			break;
		}
		case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
		{
			*TheLogFile << "Inside FirstAllPurposeDebugReportCallback VK_DEBUG_REPORT_DEBUG_BIT_EXT " << location << " " << objectType << std::endl;
			break;
		}
	}

	*TheLogFile <<  pLayerPrefix << std::endl;
	*TheLogFile <<  pMessage << std::endl;
	*TheLogFile <<  "Goodbye" << std::endl;

	return VK_FALSE;
}
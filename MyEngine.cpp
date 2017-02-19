#include "MyEngine.h"
#include "WindowsWindow.h"
#include "Swapchain.h"
#include "VulkanDevice.h"

#pragma comment(linker, "/subsystem:windows")
#include <Windows.h>


#include <cstdlib>
#include <algorithm>
#include <iostream> 
#include <fstream>
#include <thread>


#if VK_NO_PROTOTYPES


// EXT_debug_report function pointers

PFN_vkCreateDebugReportCallbackEXT	vkCreateDebugReportCallbackEXT;
PFN_vkDebugReportMessageEXT			vkDebugReportMessageEXT;
PFN_vkDestroyDebugReportCallbackEXT	vkDestroyDebugReportCallbackEXT;


// WSI functions
PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR;

// VK_KHR_surface function pointers
PFN_vkDestroySurfaceKHR 
vkDestroySurfaceKHR;
PFN_vkGetPhysicalDeviceSurfaceSupportKHR
vkGetPhysicalDeviceSurfaceSupportKHR;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR
vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR
vkGetPhysicalDeviceSurfaceFormatsKHR;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR
vkGetPhysicalDeviceSurfacePresentModesKHR;


//Swapchian

PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
PFN_vkQueuePresentKHR vkQueuePresentKHR;


//Core

PFN_vkCreateInstance vkCreateInstance;
PFN_vkDestroyInstance vkDestroyInstance;
PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties;
PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties;
PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
PFN_vkCreateDevice vkCreateDevice;
PFN_vkDestroyDevice vkDestroyDevice;
PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties;
PFN_vkGetDeviceQueue vkGetDeviceQueue;
PFN_vkQueueSubmit vkQueueSubmit;
PFN_vkQueueWaitIdle vkQueueWaitIdle;
PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
PFN_vkAllocateMemory vkAllocateMemory;
PFN_vkFreeMemory vkFreeMemory;
PFN_vkMapMemory vkMapMemory;
PFN_vkUnmapMemory vkUnmapMemory;
PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges;
PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges;
PFN_vkGetDeviceMemoryCommitment vkGetDeviceMemoryCommitment;
PFN_vkBindBufferMemory vkBindBufferMemory;
PFN_vkBindImageMemory vkBindImageMemory;
PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
PFN_vkGetImageSparseMemoryRequirements vkGetImageSparseMemoryRequirements;
PFN_vkGetPhysicalDeviceSparseImageFormatProperties vkGetPhysicalDeviceSparseImageFormatProperties;
PFN_vkQueueBindSparse vkQueueBindSparse;
PFN_vkCreateFence vkCreateFence;
PFN_vkDestroyFence vkDestroyFence;
PFN_vkResetFences vkResetFences;
PFN_vkGetFenceStatus vkGetFenceStatus;
PFN_vkWaitForFences vkWaitForFences;
PFN_vkCreateSemaphore vkCreateSemaphore;
PFN_vkDestroySemaphore vkDestroySemaphore;
PFN_vkCreateEvent vkCreateEvent;
PFN_vkDestroyEvent vkDestroyEvent;
PFN_vkGetEventStatus vkGetEventStatus;
PFN_vkSetEvent vkSetEvent;
PFN_vkResetEvent vkResetEvent;
PFN_vkCreateQueryPool vkCreateQueryPool;
PFN_vkDestroyQueryPool vkDestroyQueryPool;
PFN_vkGetQueryPoolResults vkGetQueryPoolResults;
PFN_vkCreateBuffer vkCreateBuffer;
PFN_vkDestroyBuffer vkDestroyBuffer;
PFN_vkCreateBufferView vkCreateBufferView;
PFN_vkDestroyBufferView vkDestroyBufferView;
PFN_vkCreateImage vkCreateImage;
PFN_vkDestroyImage vkDestroyImage;
PFN_vkGetImageSubresourceLayout vkGetImageSubresourceLayout;
PFN_vkCreateImageView vkCreateImageView;
PFN_vkDestroyImageView vkDestroyImageView;
PFN_vkCreateShaderModule vkCreateShaderModule;
PFN_vkDestroyShaderModule vkDestroyShaderModule;
PFN_vkCreatePipelineCache vkCreatePipelineCache;
PFN_vkDestroyPipelineCache vkDestroyPipelineCache;
PFN_vkGetPipelineCacheData vkGetPipelineCacheData;
PFN_vkMergePipelineCaches vkMergePipelineCaches;
PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
PFN_vkCreateComputePipelines vkCreateComputePipelines;
PFN_vkDestroyPipeline vkDestroyPipeline;
PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
PFN_vkCreateSampler vkCreateSampler;
PFN_vkDestroySampler vkDestroySampler;
PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout;
PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;
PFN_vkResetDescriptorPool vkResetDescriptorPool;
PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
PFN_vkFreeDescriptorSets vkFreeDescriptorSets;
PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;
PFN_vkCreateFramebuffer vkCreateFramebuffer;
PFN_vkDestroyFramebuffer vkDestroyFramebuffer;
PFN_vkCreateRenderPass vkCreateRenderPass;
PFN_vkDestroyRenderPass vkDestroyRenderPass;
PFN_vkGetRenderAreaGranularity vkGetRenderAreaGranularity;
PFN_vkCreateCommandPool vkCreateCommandPool;
PFN_vkDestroyCommandPool vkDestroyCommandPool;
PFN_vkResetCommandPool vkResetCommandPool;
PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
PFN_vkEndCommandBuffer vkEndCommandBuffer;
PFN_vkResetCommandBuffer vkResetCommandBuffer;
PFN_vkCmdBindPipeline vkCmdBindPipeline;
PFN_vkCmdSetViewport vkCmdSetViewport;
PFN_vkCmdSetScissor vkCmdSetScissor;
PFN_vkCmdSetLineWidth vkCmdSetLineWidth;
PFN_vkCmdSetDepthBias vkCmdSetDepthBias;
PFN_vkCmdSetBlendConstants vkCmdSetBlendConstants;
PFN_vkCmdSetDepthBounds vkCmdSetDepthBounds;
PFN_vkCmdSetStencilCompareMask vkCmdSetStencilCompareMask;
PFN_vkCmdSetStencilWriteMask vkCmdSetStencilWriteMask;
PFN_vkCmdSetStencilReference vkCmdSetStencilReference;
PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets;
PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer;
PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers;
PFN_vkCmdDraw vkCmdDraw;
PFN_vkCmdDrawIndexed vkCmdDrawIndexed;
PFN_vkCmdDrawIndirect vkCmdDrawIndirect;
PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect;
PFN_vkCmdDispatch vkCmdDispatch;
PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect;
PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
PFN_vkCmdCopyImage vkCmdCopyImage;
PFN_vkCmdBlitImage vkCmdBlitImage;
PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;
PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer;
PFN_vkCmdUpdateBuffer vkCmdUpdateBuffer;
PFN_vkCmdFillBuffer vkCmdFillBuffer;
PFN_vkCmdClearColorImage vkCmdClearColorImage;
PFN_vkCmdClearDepthStencilImage vkCmdClearDepthStencilImage;
PFN_vkCmdClearAttachments vkCmdClearAttachments;
PFN_vkCmdResolveImage vkCmdResolveImage;
PFN_vkCmdSetEvent vkCmdSetEvent;
PFN_vkCmdResetEvent vkCmdResetEvent;
PFN_vkCmdWaitEvents vkCmdWaitEvents;
PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
PFN_vkCmdBeginQuery vkCmdBeginQuery;
PFN_vkCmdEndQuery vkCmdEndQuery;
PFN_vkCmdResetQueryPool vkCmdResetQueryPool;
PFN_vkCmdWriteTimestamp vkCmdWriteTimestamp;
PFN_vkCmdCopyQueryPoolResults vkCmdCopyQueryPoolResults;
PFN_vkCmdPushConstants vkCmdPushConstants;
PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass;
PFN_vkCmdNextSubpass vkCmdNextSubpass;
PFN_vkCmdEndRenderPass vkCmdEndRenderPass;

#endif

// MS-Windows event handling function:

MyEngine::MyEngine() :name("TheEngine"),
mFinish(false)
{
	mLogFile.open("DebugReportLogs/" + name +"Log.txt");
	mLoaderLogFile.open("DebugReportLogs/" + name + "LoaderLog.txt");
	mMemLogFile.open("DebugReportLogs/" + name + "MemoryLog.txt");
	mErrorLogFile.open("DebugReportLogs/" + name + "ErrorLog.txt");
	mPerfLogFile.open("DebugReportLogs/" + name + "PerfLog.txt");

	mInformationLogFile.open("DebugReportLogs/" + name + "InformationLog.txt");
	mInformationLogFile << "Creating" << std::endl;
}





MyEngine::~MyEngine()
{
	mFinish = true;
	mRenderThread.join();

	delete m_TheSwapchain;
	delete m_TheWindow;
	delete mVulkanDevice;


	DestroyDebugReportStuff();
	vkDestroyInstance(TheVulkanInstance, nullptr);

	mLoaderLogFile.close();
	mMemLogFile.close();
	mLogFile.close();
	mInformationLogFile.close();
	mErrorLogFile.close();
	mPerfLogFile.close();
}





void MyEngine::InitLoader()
{
	HMODULE VulkanLoader = LoadLibrary("vulkan-1-1-0-30-0.dll");

	vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)GetProcAddress(VulkanLoader, "vkGetInstanceProcAddr");

	vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)
		GetProcAddress(VulkanLoader, "vkEnumerateInstanceLayerProperties");

	vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)
		GetProcAddress(VulkanLoader, "vkEnumerateInstanceExtensionProperties");

	vkCreateInstance = (PFN_vkCreateInstance)
		GetProcAddress(VulkanLoader, "vkCreateInstance");
}





void MyEngine::SetWindowOffsetAndSize(int x, int y, int width, int height)
{
	mWindowWidth = width;
	mWindowHeight = height;

	if (m_TheWindow == nullptr)
	{
		m_TheWindow = new EngineWindow(*this, x, y, width, height);
	}
}





void MyEngine::GetInstanceExtensionPointers()
{

#if VK_NO_PROTOTYPES
	// EXT_debug_report function pointers

	vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(TheVulkanInstance, "vkCreateDebugReportCallbackEXT");
	vkDebugReportMessageEXT = (PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(TheVulkanInstance, "vkDebugReportMessageEXT");
	vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(TheVulkanInstance, "vkDestroyDebugReportCallbackEXT");



	// VK_KHR_surface function pointers

	// WSI functions
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateWin32SurfaceKHR);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetPhysicalDeviceWin32PresentationSupportKHR);

	// VK_KHR_surface function pointers
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroySurfaceKHR);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetPhysicalDeviceSurfaceSupportKHR);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetPhysicalDeviceSurfaceFormatsKHR);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetPhysicalDeviceSurfacePresentModesKHR);

	//Swapchain

	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateSwapchainKHR);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroySwapchainKHR);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetSwapchainImagesKHR);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, AcquireNextImageKHR);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, QueuePresentKHR);

	//core

	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyInstance);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, EnumeratePhysicalDevices);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetPhysicalDeviceFeatures);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetPhysicalDeviceFormatProperties);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetPhysicalDeviceImageFormatProperties);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetPhysicalDeviceProperties);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetPhysicalDeviceQueueFamilyProperties);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetPhysicalDeviceMemoryProperties);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetInstanceProcAddr);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetDeviceProcAddr);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateDevice);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyDevice);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, EnumerateDeviceExtensionProperties);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, EnumerateDeviceLayerProperties);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetDeviceQueue);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, QueueSubmit);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, QueueWaitIdle);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DeviceWaitIdle);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, AllocateMemory);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, FreeMemory);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, MapMemory);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, UnmapMemory);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, FlushMappedMemoryRanges);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, InvalidateMappedMemoryRanges);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetDeviceMemoryCommitment);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, BindBufferMemory);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, BindImageMemory);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetBufferMemoryRequirements);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetImageMemoryRequirements);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetImageSparseMemoryRequirements);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetPhysicalDeviceSparseImageFormatProperties);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, QueueBindSparse);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateFence);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyFence);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, ResetFences);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetFenceStatus);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, WaitForFences);

	// CreateSemaphore part of Windows API, defined to CreateSemaphoreA
	vkCreateSemaphore = (PFN_vkCreateSemaphore)vkGetInstanceProcAddr(TheVulkanInstance, "vkCreateSemaphore");
	//	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateSemaphore);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroySemaphore);

	// CreateEvent part of Windows API, defined to CreateEventA
	vkCreateEvent = (PFN_vkCreateEvent)vkGetInstanceProcAddr(TheVulkanInstance, "vkCreateEvent");
	//	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateEvent);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyEvent);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetEventStatus);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, SetEvent);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, ResetEvent);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateQueryPool);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyQueryPool);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetQueryPoolResults);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateBuffer);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyBuffer);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateBufferView);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyBufferView);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateImage);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyImage);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetImageSubresourceLayout);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateImageView);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyImageView);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateShaderModule);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyShaderModule);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreatePipelineCache);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyPipelineCache);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetPipelineCacheData);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, MergePipelineCaches);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateGraphicsPipelines);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateComputePipelines);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyPipeline);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreatePipelineLayout);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyPipelineLayout);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateSampler);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroySampler);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateDescriptorSetLayout);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyDescriptorSetLayout);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateDescriptorPool);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyDescriptorPool);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, ResetDescriptorPool);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, AllocateDescriptorSets);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, FreeDescriptorSets);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, UpdateDescriptorSets);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateFramebuffer);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyFramebuffer);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateRenderPass);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyRenderPass);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, GetRenderAreaGranularity);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CreateCommandPool);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, DestroyCommandPool);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, ResetCommandPool);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, AllocateCommandBuffers);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, FreeCommandBuffers);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, BeginCommandBuffer);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, EndCommandBuffer);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, ResetCommandBuffer);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdBindPipeline);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdSetViewport);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdSetScissor);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdSetLineWidth);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdSetDepthBias);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdSetBlendConstants);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdSetDepthBounds);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdSetStencilCompareMask);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdSetStencilWriteMask);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdSetStencilReference);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdBindDescriptorSets);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdBindIndexBuffer);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdBindVertexBuffers);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdDraw);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdDrawIndexed);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdDrawIndirect);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdDrawIndexedIndirect);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdDispatch);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdDispatchIndirect);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdCopyBuffer);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdCopyImage);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdBlitImage);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdCopyBufferToImage);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdCopyImageToBuffer);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdUpdateBuffer);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdFillBuffer);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdClearColorImage);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdClearDepthStencilImage);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdClearAttachments);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdResolveImage);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdSetEvent);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdResetEvent);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdWaitEvents);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdPipelineBarrier);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdBeginQuery);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdEndQuery);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdResetQueryPool);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdWriteTimestamp);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdCopyQueryPoolResults);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdPushConstants);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdBeginRenderPass);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdNextSubpass);
	GET_INSTANCE_PROC_ADDR(TheVulkanInstance, CmdEndRenderPass);



#endif
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
		DebugCallbackCreateinfo.pUserData = this;

	/* Register the callback */
	VkResult result = vkCreateDebugReportCallbackEXT(TheVulkanInstance, &DebugCallbackCreateinfo, nullptr, &m_callback);
	if (result != VK_SUCCESS)
	{
		OutputDebugString("Didn't succeed\n");
	}

	DebugCallbackCreateinfo.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT;
	DebugCallbackCreateinfo.pUserData = this;



	/* Register the callback */
	result = vkCreateDebugReportCallbackEXT(TheVulkanInstance, &DebugCallbackCreateinfo, nullptr, &m_InformationCallback);
	if (result != VK_SUCCESS)
	{
		OutputDebugString("Didn't succeed\n");
	}
}





void MyEngine::DestroyDebugReportStuff()
{
	vkDestroyDebugReportCallbackEXT(TheVulkanInstance, m_callback, nullptr);
	vkDestroyDebugReportCallbackEXT(TheVulkanInstance, m_InformationCallback, nullptr);
}





void MyEngine::SelectPhysicalDevice()
{

	int DeviceIndex = -1;
	for (auto it : m_AvailablePhysicalDevices)
	{
		uint32_t PropertyCount;
		vkEnumerateDeviceExtensionProperties(it, NULL, &PropertyCount, NULL);
		VkExtensionProperties * ExtensionProperties = (VkExtensionProperties*)malloc(PropertyCount*sizeof(VkExtensionProperties));
		vkEnumerateDeviceExtensionProperties(it, NULL, &PropertyCount, ExtensionProperties);
		for (uint32_t i = 0; i < PropertyCount; i++)
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
		for (uint32_t i = 0; i < FamilyCount; i++)
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

	for (uint32_t i = 0; i < LayerCount; i++)
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

		for (uint32_t j = 0; j < extensionCount; j++)
		{
			m_AvailableExtensionNames.push_back(extensionProperties[j].extensionName);
			char tempString[VK_MAX_EXTENSION_NAME_SIZE];
			sprintf_s(tempString, "Extension %d: %s\n", j, extensionProperties[j].extensionName);
			OutputDebugString(tempString);
		}
	
		OutputDebugString("Going through layer extensions.\n");
		for (uint32_t i = 0; i < LayerCount; i++)
		{
			theResult = vkEnumerateInstanceExtensionProperties(theProperties[i].layerName , &extensionCount, NULL);
			VkExtensionProperties * LayerExtensionProperties = (VkExtensionProperties*)malloc(extensionCount* sizeof(VkExtensionProperties));
			theResult = vkEnumerateInstanceExtensionProperties(theProperties[i].layerName, &extensionCount, LayerExtensionProperties);
			OutputDebugString(theProperties[i].layerName);
			OutputDebugString(" ");
			for (uint32_t j = 0; j < extensionCount; j++)
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
	

#if ENGINE_LOGGING_ENABLED
	UsingLayers.push_back("VK_LAYER_LUNARG_api_dump");
	UsingLayers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif

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

	for (uint32_t i = 0; i < DeviceCount; i++)
	{
		m_AvailablePhysicalDevices.push_back(PhysicalDeviceList[i]);
		vkGetPhysicalDeviceProperties(PhysicalDeviceList[i], &mDeviceProperties);
		char str[256];
		uint32_t version = mDeviceProperties.apiVersion;
		sprintf_s(str, "Version Major: %d Version minor: %d\n Device Type: %d \n Driver Version: %d\nDevice Name: %s\n", 
			VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), mDeviceProperties.deviceType, mDeviceProperties.driverVersion,
			mDeviceProperties.deviceName);
		OutputDebugString(str);

	}
}





void MyEngine::CreateDevice()
{
	mVulkanDevice = new VulkanDevice(m_AvailablePhysicalDevices[0], *m_TheWindow);

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





void MyEngine::CreateSwapchain()
{

	m_TheSwapchain = new Swapchain(mVulkanDevice, *m_TheWindow);
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
	MyEngine * theEngine = static_cast<MyEngine*>(pUserData);

	if (memcmp(pLayerPrefix, "MEM", 3) == 0)
	{
		theEngine->mMemLogFile << std::endl << "#####################" << std::endl;
		theEngine->mMemLogFile << pMessage << std::endl;
		return VK_FALSE;

	}

	if (memcmp(pLayerPrefix, "loader", 6) == 0)
	{
		theEngine->mLoaderLogFile << std::endl << "#####################" << std::endl;
		theEngine->mLoaderLogFile << pMessage << std::endl;
		return VK_FALSE;
	}

	switch (flags)
	{
		case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
		{
			theEngine->mInformationLogFile << std::endl << "###################################### VK_DEBUG_REPORT_INFORMATION_BIT_EXT" << std::endl;
			theEngine->mInformationLogFile << pLayerPrefix << std::endl;
			theEngine->mInformationLogFile << pMessage << std::endl;
			break;
		}
		case VK_DEBUG_REPORT_WARNING_BIT_EXT:
		{
			theEngine->mLogFile << std::endl << "###################################### VK_DEBUG_REPORT_WARNING_BIT_EXT" << std::endl;
			theEngine->mLogFile << pLayerPrefix << std::endl;
			theEngine->mLogFile << pMessage << std::endl;

			break;
		}
		case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
		{
			theEngine->mPerfLogFile << std::endl << "###################################### VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT" << std::endl;
			theEngine->mPerfLogFile << pLayerPrefix << std::endl;
			theEngine->mPerfLogFile << pMessage << std::endl;
			break;
		}
		case VK_DEBUG_REPORT_ERROR_BIT_EXT:
		{
			theEngine->mErrorLogFile << std::endl << "###################################### VK_DEBUG_REPORT_ERROR_BIT_EXT" << std::endl;
			theEngine->mErrorLogFile << pLayerPrefix << std::endl;
			theEngine->mErrorLogFile << pMessage << std::endl;

			break;
		}
		case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
		{
			theEngine->mLogFile << std::endl << "###################################### VK_DEBUG_REPORT_DEBUG_BIT_EXT " << location << " " << objectType << std::endl;
			theEngine->mLogFile << pLayerPrefix << std::endl;
			theEngine->mLogFile << pMessage << std::endl;
			break;
		}
	}

	
	return VK_FALSE;
}





void MyEngine::SpawnUpdateThread()
{
	mRenderThread =  std::thread(&MyEngine::Run, std::ref(*this));
}





void MyEngine::Run()
{
	mVulkanDevice->CreateRenderTargets(mWindowWidth, mWindowHeight);
	mVulkanDevice->CreateInitialData();

	while (!mFinish)
	{
		Update();
	}

}





void MyEngine::Update()
{	
	mVulkanDevice->Update();
}





void MyEngine::TakeInput(unsigned int keyPress)
{
	EngineLog("key: ", keyPress);
	mVulkanDevice->TakeInput(keyPress);
}
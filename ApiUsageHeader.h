#include <stdio.h>
#include <Windows.h>

#include <vulkan/vulkan.h>

void VulkanResultString(VkResult ResultToCheck)
{
	char returnString[500];
	switch (ResultToCheck){
	case VK_SUCCESS:
		sprintf_s(returnString, "Command successfully completed\n");
	case VK_INCOMPLETE:
		sprintf_s(returnString, "A return array was too small for the result\n");
		/*
			• VK_SUBOPTIMAL_KHR A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully.


			Error codes
			• VK_ERROR_OUT_OF_HOST_MEMORY A host memory allocation has failed.
			• VK_ERROR_OUT_OF_DEVICE_MEMORY A device memory allocation has failed.
			• VK_ERROR_INITIALIZATION_FAILED Initialization of an object could not be completed for implementation - specific reasons.
			• VK_ERROR_DEVICE_LOST The logical or physical device has been lost.See Lost Device
			• VK_ERROR_MEMORY_MAP_FAILED Mapping of a memory object has failed.
			• VK_ERROR_LAYER_NOT_PRESENT A requested layer is not present or could not be loaded.
			• VK_ERROR_EXTENSION_NOT_PRESENT A requested extension is not supported.
			• VK_ERROR_FEATURE_NOT_PRESENT A requested feature is not supported.
			• VK_ERROR_INCOMPATIBLE_DRIVER The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation - specific reasons.
			• VK_ERROR_TOO_MANY_OBJECTS Too many objects of the type have already been created.
			• VK_ERROR_FORMAT_NOT_SUPPORTED A requested format is not supported on this device.
			• VK_ERROR_FRAGMENTED_POOL A requested pool allocation has failed due to fragmentation of the pool’s memory.
			• VK_ERROR_SURFACE_LOST_KHR A surface is no longer available.
			• VK_ERROR_NATIVE_WINDOW_IN_USE_KHR The requested window is already connected to a VkSurfaceKHR, or to some other non - Vulkan API.
			• VK_ERROR_OUT_OF_DATE_KHR A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail.Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface.
			• VK_ERROR_INCOMPATIBLE_DISPLAY_KHR The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image.
			*/

	}
	OutputDebugString(returnString);

}
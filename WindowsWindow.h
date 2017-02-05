#ifndef WINDOWS_WINDOW_H
#define WINDOWS_WINDOW_H 1

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR 1
#pragma comment(linker, "/subsystem:windows")

#include <vulkan/vulkan.h>
#include <string>
#include <atomic>


class Swapchain;
class MyEngine;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class EngineWindow {

public:
	EngineWindow();
	~EngineWindow();
	EngineWindow(MyEngine&, int x, int y, int width, int height);
	void Initialize(HINSTANCE);
	void Redraw();
	void Update();
	void Resize(uint32_t, uint32_t);
	void InitializeSurface(VkInstance);
	void SetUpSwapChain(Swapchain*);
	void SendKeyEvent(unsigned int);
	VkSurfaceKHR GetSurface(){ return m_TheVulkanPresentSurface; }
	VkExtent2D GetExtent();


private:
	std::string name;
	std::atomic<bool> mInitialized;
	int mWidth;
	int mHeight;
	int mXoffset;
	int mYoffset;
	HWND m_handle;
	HINSTANCE m_WindowsAppInstance;

	Swapchain * mSwapchain;
	MyEngine & mEngine;

	VkInstance		m_InstanceForSurface;
	VkSurfaceKHR	m_TheVulkanPresentSurface;
};
#endif WINDOWS_WINDOW_H
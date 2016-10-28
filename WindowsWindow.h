#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR 1
#pragma comment(linker, "/subsystem:windows")

#include <vulkan/vulkan.h>
#include <string>


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class EngineWindow {

public:
	EngineWindow();
	~EngineWindow();
	EngineWindow(int x, int y, int width, int height);
	void Initialize(HINSTANCE);
	void Redraw();
	void InitializeSurface(VkInstance);
	void SetUpSwapChain();
	VkSurfaceKHR GetSurface(){ return m_TheVulkanPresentSurface; }
private:
	std::string name;
	int m_width;
	int m_height;
	int x_offset;
	int y_offset;
	HWND m_handle;
	HINSTANCE m_WindowsAppInstance;

	VkInstance		m_InstanceForSurface;
	VkSurfaceKHR	m_TheVulkanPresentSurface;
};
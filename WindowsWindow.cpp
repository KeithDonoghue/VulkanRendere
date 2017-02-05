#include "WindowsWindow.h"
#include "MyEngine.h"
#ifndef _MSC_VER
#define _ISOC11_SOURCE /* for aligned_alloc() */
#endif


#define ENGINE_LOGGING_ENABLED 1
#include "Swapchain.h"
#include "EngineLogging.h"


EngineWindow::EngineWindow(MyEngine & theEngine, int x, int y, int width, int height):
	mEngine(theEngine),
	mWidth(width),
	mHeight(height),
	mXoffset(x),
	mYoffset( y),
	name("Destroyer")
{
	mInitialized.store(0);
}





EngineWindow::~EngineWindow()
{
	vkDestroySurfaceKHR(m_InstanceForSurface,m_TheVulkanPresentSurface,nullptr);
}





void EngineWindow::Initialize(HINSTANCE AppInstance)
{
	WNDCLASSEX win_class;
	m_WindowsAppInstance = AppInstance;

	// Initialize the window class structure:
	win_class.cbSize = sizeof(WNDCLASSEX);
	win_class.style = CS_HREDRAW | CS_VREDRAW;
	win_class.lpfnWndProc = WndProc;
	win_class.cbClsExtra = 0;
	win_class.cbWndExtra = 0;
	win_class.hInstance = AppInstance; // hInstance
	win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	win_class.lpszMenuName = NULL;
	win_class.lpszClassName = name.c_str();
	win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	// Register window class:
	if (!RegisterClassEx(&win_class)) {
		// It didn't work, so try to give a useful error:
		printf("Unexpected error trying to start the application!\n");
		fflush(stdout);
		exit(1);
	}


	// Create window with the registered class:
	RECT wr = { 0, 0, mWidth, mHeight };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	m_handle = CreateWindowEx(0,
		name.c_str(),           // class name
		name.c_str(),           // app name
		WS_OVERLAPPEDWINDOW | // window style
		WS_VISIBLE | WS_SYSMENU,
		mXoffset, mYoffset,           // x/y coords
		wr.right - wr.left, // width
		wr.bottom - wr.top, // height
		NULL,               // handle to parent
		NULL,               // handle to menu
		AppInstance,   // hInstance
		this);              // no extra parameters

}





void EngineWindow::Update()
{
	RedrawWindow(m_handle, NULL, NULL, RDW_INTERNALPAINT);
}





void EngineWindow::Redraw()   
{
	
	if (!mInitialized.load())
	{
		mInitialized.store(true);
	}
	else
	{
		mSwapchain->Update();
	}
}





void EngineWindow::SetUpSwapChain(Swapchain* theSwapchain)
{
	mSwapchain = theSwapchain;
}





void EngineWindow::Resize(uint32_t width, uint32_t height)
{
	mWidth = width;
	mHeight = height;
	mSwapchain->Resize(width, height);
}





// MS-Windows event handling function:
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	CREATESTRUCT * temp;
	EngineWindow * presentable;


	switch (uMsg) {
	case WM_SIZE:
		EngineLog("Resizing: ", wParam, HIWORD(lParam), LOWORD(lParam));
		presentable = reinterpret_cast<EngineWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (presentable)
			presentable->Resize(LOWORD(lParam), HIWORD(lParam));
		return TRUE;
	case WM_KEYDOWN:
		EngineLog("KeyDown", wParam);
		presentable = reinterpret_cast<EngineWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (presentable)
			presentable->SendKeyEvent(wParam);
	case WM_CREATE:
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_NCCREATE:
		temp = reinterpret_cast<CREATESTRUCT*>(lParam);
		presentable = reinterpret_cast<EngineWindow *>(temp->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (DWORD_PTR)presentable);
		return TRUE;
	case WM_PAINT:
		presentable = reinterpret_cast<EngineWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (presentable)
			presentable->Redraw();

	default:
		break;
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}





VkExtent2D EngineWindow::GetExtent()
{
	VkExtent2D extent = {};
	extent.width = mWidth;
	extent.height = mHeight;
	return extent;
}





void EngineWindow::InitializeSurface(VkInstance assosciatedInstance)
{

	m_InstanceForSurface = assosciatedInstance;
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = nullptr;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.hinstance = m_WindowsAppInstance;
	surfaceCreateInfo.hwnd = m_handle;
	
	VkResult result = vkCreateWin32SurfaceKHR(assosciatedInstance, &surfaceCreateInfo, nullptr, &m_TheVulkanPresentSurface);

	PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR fpGetPhysicalDeviceWin32PresentationSupportKHR =
		(PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)vkGetInstanceProcAddr(m_InstanceForSurface, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
	if (fpGetPhysicalDeviceWin32PresentationSupportKHR)
		OutputDebugString("Found\n");
	else
		OutputDebugString("Not Found\n");
}




void EngineWindow::SendKeyEvent(unsigned int key)
{
	mEngine.TakeInput(key);
}



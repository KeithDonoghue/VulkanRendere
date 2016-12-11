#include "MyEngine.h"


#include "Windows.h"
#include "WinBase.h"

#include <stdio.h>
#include <iostream>



FILE * stream;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR pCmdLine, int nCmdShow) {
	MyEngine Mine;

	freopen_s(&stream, "C:\\Users\\keith\\Documents\\stdout.txt", "w", stdout);
	freopen_s(&stream, "C:\\Users\\keith\\Documents\\stderr.txt", "w", stderr);



	Mine.SetWindowOffsetAndSize(300, 100, 400, 400);
	Mine.InitLayersAndExtensions();
	Mine.CreateVulkanInstance();
	Mine.GetInstanceExtensionPointers();

	Mine.SetUpDebugReportStuff();
	Mine.CreateGameWindow(hInstance);

	Mine.DoPhysicalDeviceStuff();
	Mine.SelectPhysicalDevice();
	Mine.CreateDevice();
	Mine.DumpSurfaceInfoToFile();
	Mine.CreateSwapchain();

	Mine.SpawnUpdateThread();


	bool Quit = false;
	MSG message;
	while (!Quit)
	{
		PeekMessageA(&message, NULL, 0, 0, PM_REMOVE);
		if (message.message == WM_QUIT)
		{
			Quit = true;
		}
		else {

			/* Translate and dispatch to event queue*/

			TranslateMessage(&message);

			DispatchMessage(&message);

			//cout << "Message don't Quit" << std::endl;


		}
		Mine.FinishedFrameWork();
	}


}

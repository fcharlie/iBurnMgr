///

#include "Precompiled.h"
#include "MetroWindow.h"
#include "MUIController.h"

//#include "APIController.h"
CAppModule _Module;



int WINAPI wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Metro::MUI::muiController.Init();
	_Module.Init(nullptr, hInstance);
	MSG msg;
	::InitCommonControls();
	Metro::MetroWindow iMetroWindow;
	
	RECT rect = { (::GetSystemMetrics(SM_CXSCREEN) - 720) / 2, (::GetSystemMetrics(SM_CYSCREEN) - 450) / 2, (::GetSystemMetrics(SM_CXSCREEN) + 720) / 2, (::GetSystemMetrics(SM_CYSCREEN) + 450) / 2 };
	if (iMetroWindow.Create(nullptr, rect, METRO_INTERNAL_WINDOWLNAME, WS_OVERLAPPED | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_ACCEPTFILES) == nullptr)
	{
		return -1;
	}
	DWORD dwExit = 0;
	iMetroWindow.ShowWindow(nCmdShow);
	iMetroWindow.UpdateWindow();

	while (GetMessage(&msg, nullptr, 0, 0)>0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	dwExit = iMetroWindow.GetExitCode();
	::CoUninitialize();
	_Module.Term();
	return dwExit;
}
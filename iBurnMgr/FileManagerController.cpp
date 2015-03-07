/*********************************************************************************************************
* FileManagerController.cpp
* Note: iBurnMgr FileManagerController
* E-mail:<forcemz@outlook.com>
* Data: @2015.03
* Copyright (C) 2015 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#include "Precompiled.h"
#include <Uxtheme.h>
#include <strsafe.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#include "APIController.h"

#pragma comment(lib,"propsys.lib")
#pragma comment(lib,"shlwapi.lib")

typedef COMDLG_FILTERSPEC FileType;

typedef struct _DialogInfo{
	wchar_t DlgTitle[256];
	wchar_t DefualtType[256];
	//FileType* filetype;
}DialogInfo;

const FileType FileArg[] =
{
	{ L"ISO/UDF file (*.iso)", L"*.iso" },
	{ L"Onther Image file (*.img;*.dvd；*.bin)", L"*.img;*.dvd;*.bin" },
	{ L"All Files (*.*)", L"*.*" }
};
UINT Argc = ARRAYSIZE(FileArg);
//DialogInfo dlf={L"打开源文件",L"cpp"};
void ReportErrorEx(LPCWSTR pszFunction, HRESULT hr)
{
	wchar_t szMessage[200];
	if (SUCCEEDED(StringCchPrintf(szMessage, ARRAYSIZE(szMessage),
		L"%s failed w/hr 0x%08lx", pszFunction, hr)))
	{
		MessageBox(NULL, szMessage, L"Error", MB_ICONERROR);
	}
}

LRESULT OpenImageFile(HWND hWnd, PWSTR Imagename)
{
	HRESULT hr = S_OK;

	// Create a new common open file dialog.
	IFileDialog *pfd = NULL;
	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pfd));
	if (SUCCEEDED(hr))
	{
		if (SUCCEEDED(hr))
		{
			hr = pfd->SetTitle(L"Select the Image file");
		}

		// Specify file types for the file dialog.
		if (SUCCEEDED(hr))
		{
			hr = pfd->SetFileTypes(Argc, FileArg);
			if (SUCCEEDED(hr))
			{
				// Set the selected file type index to ISO Image.
				hr = pfd->SetFileTypeIndex(1);
			}
		}

		// Set the default extension to be added to file names as ".iso"
		if (SUCCEEDED(hr))
		{
			hr = pfd->SetDefaultExtension(L"*.iso");
		}

		// Show the open file dialog.
		if (SUCCEEDED(hr))
		{
			hr = pfd->Show(hWnd);
			if (SUCCEEDED(hr))
			{
				// Get the result of the open file dialog.
				IShellItem *psiResult = NULL;
				hr = pfd->GetResult(&psiResult);
				if (SUCCEEDED(hr))
				{
					PWSTR pszPath = NULL;
					hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
					if (SUCCEEDED(hr))
					{
						wcscpy_s(Imagename, (1024*32-1), pszPath);
						//MessageBox(hWnd, pszPath, L"The selected file is", MB_OK);
						CoTaskMemFree(pszPath);
					}
					psiResult->Release();
				}
			}
			else
			{
				if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
				{
					// User cancelled the dialog...
				}
			}
		}

		pfd->Release();
	}

	// Report the error. 
	if (FAILED(hr))
	{
		// If it's not that the user cancelled the dialog, report the error in a 
		// message box.
		if (hr != HRESULT_FROM_WIN32(ERROR_CANCELLED))
		{
			ReportErrorEx(L"OnOpenAFile", hr);
		}
	}
	return hr;
}
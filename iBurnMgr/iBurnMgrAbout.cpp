/*********************************************************************************************************
* iBurnMgrAbout.cpp
* Note: iBurnMgr iBurnMgrAbout
* E-mail:<forcemz@outlook.com>
* Date: @2015.03
* Copyright (C) 2015 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#include "Precompiled.h"
#include <windows.h>
#include <commctrl.h>
#include <strsafe.h>
#include "resource.h"
#ifndef ASSERT
#	ifdef _DEBUG
#		include <assert.h>
#		define ASSERT(x) assert( x )
#		define ASSERT_HERE assert( FALSE )
#	else// _DEBUG
#		define ASSERT(x) 
#	endif//_DEBUG
#endif//ASSERT

///////////////////////////////////////////////////////////////////
// a handy macro to get the number of characters (not bytes!) 
// a string buffer can hold

#ifndef _tsizeof 
#	define _tsizeof( s )  (sizeof(s)/sizeof(s[0]))
#endif//_tsizeof

#include <comdef.h>
#include <taskschd.h>

HRESULT CALLBACK
TaskDialogCallbackProc(
__in HWND hwnd,
__in UINT msg,
__in WPARAM wParam,
__in LPARAM lParam,
__in LONG_PTR lpRefData
)
{
	switch (msg)
	{
	case TDN_CREATED:
		::SetForegroundWindow(hwnd);
		break;
	case TDN_RADIO_BUTTON_CLICKED:
		break;
	case TDN_BUTTON_CLICKED:
		break;
	case TDN_HYPERLINK_CLICKED:
		ShellExecute(hwnd, NULL, (LPCTSTR)lParam, NULL, NULL, SW_SHOWNORMAL);
		break;
	}

	return S_OK;
}

LRESULT WINAPI CreateTaskDialogIndirectFd(
	__in HWND		hwndParent,
	__in HINSTANCE	hInstance,
	__out_opt int *	pnButton,
	__out_opt int *	pnRadioButton
	)
{
	TASKDIALOGCONFIG tdConfig;
	BOOL bElevated = FALSE;
	memset(&tdConfig, 0, sizeof(tdConfig));

	tdConfig.cbSize = sizeof(tdConfig);

	tdConfig.hwndParent = hwndParent;
	tdConfig.hInstance = hInstance;
	tdConfig.dwFlags =
		TDF_ALLOW_DIALOG_CANCELLATION |
		TDF_EXPAND_FOOTER_AREA |
		TDF_POSITION_RELATIVE_TO_WINDOW |
		TDF_ENABLE_HYPERLINKS;

	//tdConfig.cRadioButtons = ARRAYSIZE(buttons);
	//tdConfig.pRadioButtons = buttons;
	tdConfig.nDefaultRadioButton = *pnRadioButton;

	tdConfig.pszWindowTitle = L"About Metro iBurnMgr";

	tdConfig.pszMainInstruction = _T("Force Metro iBurnMgr");

	//tdConfig.pszMainIcon = bElevated ? TD_SHIELD_ICON : TD_INFORMATION_ICON;
	tdConfig.hMainIcon = static_cast<HICON>(LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON_IBURN)));
	tdConfig.dwFlags |= TDF_USE_HICON_MAIN;

	TCHAR szContent[256]; // should be long enough

	wcscpy_s(szContent, L"Copyright © 2017 The ForceStudio. All Rights Reserved.");

	tdConfig.pszContent = szContent;
	WCHAR urlStr[1024] = { 0 };
	::LoadStringW(GetModuleHandle(nullptr), IDR_APP_URL_STRING, urlStr, 1024);
	WCHAR szStr[2048] = { 0 };
	wsprintfW(szStr, L"For more information about this tool,\nVisit: <a href=\"%s\">Force\xAEStudio</a>", urlStr);
	tdConfig.pszExpandedInformation = szStr;
	tdConfig.pszCollapsedControlText = _T("More information");
	tdConfig.pszExpandedControlText = _T("Less information");
	tdConfig.pfCallback = TaskDialogCallbackProc;

	HRESULT hr = TaskDialogIndirect(&tdConfig, pnButton, pnRadioButton, NULL);

	return hr;
}

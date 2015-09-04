/*********************************************************************************************************
* ResolveBootSupervisor.cpp
* Note: iBurnMgr ResolveBootSupervisor
* E-mail:<forcemz@outlook.com>
* Date: @2015.03
* Copyright (C) 2015 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#include "Precompiled.h"
#include <string>
#include "APIController.h"
#include "ResolveBootSupervisor.h"

WCHAR g_resbootlog[100] = { 0 };

/*
Function: BootFileCopy
*/
bool BootFileCopy(const wchar_t *searchPath,wchar_t *filelist[],const wchar_t *latter){
	for (auto i=0; filelist[i] != nullptr; i++){

	}
	return false;
}


namespace ResolveSupervisor{
	DWORD WINAPI ResolveSupervisorThread(LPVOID lParam)
	{
		DWORD dRet;
		ResolveData* redata = static_cast<ResolveData*>(lParam);
		HWND hWnd = static_cast<HWND>(redata->m_hWnd);
		wcscpy_s(g_resbootlog, L"Begin to repair the boot USB drive...\0");
		PostMessage(hWnd, Metro::METRO_MULTITHREAD_MSG, MET_RESOLVEBS, (LPARAM)g_resbootlog);
		if (ResolveSupervisor::FixUSBDeviceBoot(redata->latter.c_str()))
		{
			wcscpy_s(g_resbootlog, L"Resolve USB drive boot Success\0");
			PostMessage(hWnd, Metro::METRO_MULTITHREAD_MSG, MET_RESOLVEBS, (LPARAM)g_resbootlog);
			PostMessage(hWnd, Metro::METRO_THREAD_RATESTATUS_MSG, METRO_FIX_UNLOCK, METRO_RATE_UNLOCK_OK);
			dRet = 0;
		}
		else{
			wcscpy_s(g_resbootlog, L"Resolve USB drive boot Failure\0");
			PostMessage(hWnd, Metro::METRO_MULTITHREAD_MSG, MET_RESOLVEBS, (LPARAM)g_resbootlog);
			PostMessage(hWnd, Metro::METRO_THREAD_RATESTATUS_MSG, METRO_FIX_UNLOCK, METRO_RATE_UNLOCK_ERR);
			dRet = 1;
		}
		return dRet;
	}

	bool ResolveSupervisor::FixUSBDeviceBoot(LPCWSTR DeviceLetter)
	{
		TCHAR BCDPath[MAX_UNC_PATH] = { 0 };
		PROCESS_INFORMATION pi;
		STARTUPINFO sInfo;
		DWORD dwExitCode;
		ZeroMemory(&sInfo, sizeof(sInfo));
		sInfo.cb = sizeof(sInfo);
		sInfo.dwFlags = STARTF_USESHOWWINDOW;
		sInfo.wShowWindow = SW_HIDE;
		ZeroMemory(&pi, sizeof(pi));
		GetModuleFileName(NULL, BCDPath, MAX_UNC_PATH);
		(wcsrchr(BCDPath, _T('\\')))[0] = 0;
		(wcsrchr(BCDPath, _T('\\')))[0] = 0;
		wcscat_s(BCDPath, L"\\BCDBoot\\bootsect.exe");
		if (!_waccess_s(BCDPath, 0) == 0)
		{
			return false;
		}
		wcscat_s(BCDPath, L" /NT60 ");
		wcscat_s(BCDPath, DeviceLetter);
		DWORD result = CreateProcessW(NULL, BCDPath, NULL, NULL, NULL, CREATE_NO_WINDOW, NULL, NULL, &sInfo, &pi);
		if (result == TRUE)
		{
			CloseHandle(pi.hThread);
			if (WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess, INFINITE))
			{
				TerminateProcess(pi.hProcess, 11);
			}
			GetExitCodeProcess(pi.hProcess, &dwExitCode);
			CloseHandle(pi.hProcess);
			if (dwExitCode != 0)
			{
				return false;
			}
			return true;
		}
		return false;
	}
	ResolveSupervisor::ResolveSupervisor()
	{

	}
}

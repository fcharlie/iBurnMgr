/*********************************************************************************************************
* DecompressSupervision.cpp
* Note: iBurnMgr DecompressSupervision
* E-mail:<forcemz@outlook.com>
* Date: @2015.03
* Copyright (C) 2017 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#include "Precompiled.h"
#include "DecompressSupervision.h"
#include "APIController.h"
#include "ResolveBootSupervisor.h"


static ResolveSupervisor::ResolveData radate = { {0}, 0 };
static Decompress::SupervisorData *SpData = nullptr;
WCHAR deslog[200] = { 0 };
float decst = 0;
UINT drate = 0;
DWORD dwThreadId = 0;


static bool FormatFailedCallbackInc(const wchar_t *msg,bool status,void *data){
	if (!data||!msg) return false;
	HWND hWnd = reinterpret_cast<HWND>(data);
	int nButton;
	///SendMessageW(hWnd, Metro::METRO_MULTITHREAD_MSG, MET_DECOMPRESS, (LPARAM)msg);
	if (status){
		TaskDialog(hWnd, nullptr, L"Format Notice", L"Message:", msg, TDCBF_YES_BUTTON | TDCBF_NO_BUTTON,
			TD_INFORMATION_ICON, &nButton);
	}
	else{
		TaskDialog(hWnd, nullptr, L"Format Error", L"Message:", msg, TDCBF_YES_BUTTON | TDCBF_NO_BUTTON,
			TD_ERROR_ICON, &nButton);
	}
	return true;
}

/* 7Zip 
UInt32 WINAPI CreateObject(const GUID *clsID, const GUID *interfaceID, void **outObject);
UInt32 WINAPI GetNumberOfMethods(UInt32 *numMethods);
UInt32 WINAPI GetMethodProperty(UInt32 index, PROPID propID, PROPVARIANT *value);
UInt32 WINAPI GetNumberOfFormats(UInt32 *numFormats);
UInt32 WINAPI GetHandlerProperty(PROPID propID, PROPVARIANT *value);
UInt32 WINAPI GetHandlerProperty2(UInt32 index, PROPID propID, PROPVARIANT *value);
UInt32 WINAPI SetLargePageMode();
*/



namespace Decompress{
	DWORD WINAPI DecompressDetectRate(LPVOID lParam)
	{
		if (SpData == nullptr) return 1;
		//SupervisorData* SpData = static_cast<SupervisorData*>(lParam);
		DWORD exitcode;
		HWND hWnd = static_cast<HWND>(SpData->lParam);
		if (Supervisor::CreateDecompressInvoke(hWnd,SpData->image,SpData->latter))
		{
			wcscpy_s(deslog, L"Extracting the image file success");
			//PostMessage(hWnd, Metro::METRO_THREAD_RATESTATUS_MSG, METRO_RATE_ENDOK, 0);
			exitcode = 0;
		}
		else{
			wcscpy_s(deslog, L"Extracting the image file Failure");
			//PostMessage(hWnd, Metro::METRO_THREAD_RATESTATUS_MSG, METRO_RATE_ADNORMAL, 0);
			exitcode = 1;
		}
		PostMessageW(hWnd, Metro::METRO_MULTITHREAD_MSG, MET_DECOMPRESS, (LPARAM)deslog);
		PostMessageW(hWnd, Metro::METRO_THREAD_RATESTATUS_MSG, METRO_RATE_ENDOK, 0);
		return 0;
	}
	DWORD WINAPI DecompressSupervisorThread(LPVOID Spdata)
	{
		drate = 0;
		SpData =static_cast<SupervisorData*>(Spdata);
		Supervisor *supervisor = new Supervisor(SpData->image, SpData->latter, SpData->lParam);
		HWND hWnd = static_cast<HWND>(SpData->lParam);
		if (!supervisor->CreateDecompressFormat())
		{
			PostMessage(hWnd, Metro::METRO_THREAD_RATESTATUS_MSG, METRO_RATE_FORMAT, METRO_RATE_FORMAT_ERR);
			wcscpy_s(deslog, L"Quick Format Disk Failure.");
			PostMessage(hWnd, Metro::METRO_MULTITHREAD_MSG, MET_DECOMPRESS, (LPARAM)deslog);
			return 2;
		}
		dwThreadId = GetCurrentThreadId();
		wcscpy_s(deslog, L"Quick Format Disk Success.");
		SendMessage(hWnd, Metro::METRO_MULTITHREAD_MSG, MET_DECOMPRESS, (LPARAM)deslog);
		PostMessage(hWnd, Metro::METRO_THREAD_RATESTATUS_MSG, METRO_RATE_FORMAT, METRO_RATE_FORMAT_SUC);
		Sleep(1000);
		wcscpy_s(deslog, L"The Second Step: Extracting the image file...");
		SendMessage(hWnd, Metro::METRO_MULTITHREAD_MSG, MET_DECOMPRESS, (LPARAM)deslog);
		PostMessage(hWnd, Metro::METRO_THREAD_RATESTATUS_MSG, METRO_RATE_STATUS, drate);

		DWORD dwmThread;
		DWORD dExitCode;
		DWORD rate = 1;
		HANDLE hThread = CreateThread(NULL, 0, Decompress::DecompressDetectRate, &SpData, 0, &dwmThread);
		while (true)
		{
			GetExitCodeThread(hThread, &dExitCode);
			if (dExitCode != STILL_ACTIVE)
				break;
			Sleep(10000);
			if (rate <=70)
				rate += 1;
			if (rate > 70)
				PostMessage(hWnd, Metro::METRO_THREAD_RATESTATUS_MSG, METRO_RATE_BLOCK, rate);
			else
			    PostMessage(hWnd, Metro::METRO_THREAD_RATESTATUS_MSG, METRO_RATE_STATUS, rate);
		}
		if (!dExitCode == 0)
		{
			PostMessage(hWnd, Metro::METRO_THREAD_RATESTATUS_MSG, METRO_RATE_ADNORMAL, 0);
			return 1;
		}
		CloseHandle(hThread);
		PostMessage(hWnd, Metro::METRO_THREAD_RATESTATUS_MSG, METRO_RATE_ENDOK, 0);
		radate.latter = SpData->latter;
		radate.m_hWnd = static_cast<HWND>(SpData->lParam);
		DWORD dwThreadID;
		HANDLE rbhThread = CreateThread(NULL, 0, ResolveSupervisor::ResolveSupervisorThread, &radate, 0, &dwThreadID);
		CloseHandle(rbhThread);
		return 0;
	}

	Supervisor::Supervisor(LPSupervisorData lPSpData)
	{
		Supervisor(lPSpData->image, lPSpData->latter, lPSpData->lParam);
	}
	Supervisor::Supervisor(std::wstring img, LPCWSTR latter, LPVOID lParam) :m_image(img), m_latter(latter)
	{
		hParents = static_cast<HWND>(lParam);
		m_latter = m_latter.substr(0, 2);
	}
	bool Supervisor::CreateDecompressFormat()
	{
		wcscpy_s(deslog, L"The First Step: Format USB device ");
		wcscat_s(deslog, m_latter.c_str());
		PostMessageW(hParents, Metro::METRO_MULTITHREAD_MSG, MET_DECOMPRESS, LPARAM(deslog));
		int nButton;
		TaskDialog(hParents, NULL, 
			L"Will be formatted USB device", L"Format Warning!", 
			L"The formatting process is not reversible,Select Yes to continue", 
			TDCBF_YES_BUTTON| TDCBF_NO_BUTTON,
			TD_WARNING_ICON, &nButton);

		if (nButton != IDYES)
		{
			PostMessageW(hParents, Metro::METRO_THREAD_RATESTATUS_MSG, METRO_CANCLE_OPT, METRO_CANCLE_OPT_FORMAT);
			Sleep(1500);
			return false;
		}
		if (IVdsVolumeFormat(this->m_latter.c_str(),L"Installer",FormatFailedCallbackInc,this->hParents)==TRUE)
		{
			PostMessageW(hParents, Metro::METRO_THREAD_RATESTATUS_MSG, METRO_RATE_FORMAT, METRO_RATE_FORMAT_SUC);
		}
		else{
			PostMessageW(hParents, Metro::METRO_THREAD_RATESTATUS_MSG, METRO_RATE_FORMAT, METRO_RATE_FORMAT_ERR);			
			return false;
		}
		return true;
	}
	bool WINAPI Supervisor::CreateDecompressInvoke(HWND hWnd,std::wstring img, LPWSTR latter)
	{
		WCHAR _7zCmd[MAX_PATH] = { 0 };
		//HWND m_hWnd = static_cast<HWND>(hWnd);
		GetModuleFileNameW(GetModuleHandle(nullptr), _7zCmd, MAX_UNC_PATH);
		(wcsrchr(_7zCmd, _T('\\')))[0] = 0;
		(wcsrchr(_7zCmd, _T('\\')))[0] = 0;
		wcscat_s(_7zCmd, LR"(\utility\7z\7z.exe)");
		if (!_waccess_s(_7zCmd, 0) == 0)
		{
			return false;
		}
		wcscat_s(_7zCmd, L" x ");
		wcscat_s(_7zCmd, img.c_str());
		wcscat_s(_7zCmd, L" -aoa -y -o");
		wcscat_s(_7zCmd, latter);
		wcscat_s(_7zCmd, L"\\");
		//_waccess_s()
		PROCESS_INFORMATION pi;
		STARTUPINFO sInfo;
		DWORD dwExitCode;
		ZeroMemory(&sInfo, sizeof(sInfo));
		sInfo.cb = sizeof(sInfo);
		sInfo.dwFlags = STARTF_USESHOWWINDOW;
		sInfo.wShowWindow = SW_HIDE;
		ZeroMemory(&pi, sizeof(pi));

		DWORD result = CreateProcessW(NULL, _7zCmd, NULL, NULL, NULL, CREATE_NO_WINDOW, NULL, NULL, &sInfo, &pi);
		if (result == TRUE)
		{
			CloseHandle(pi.hThread);
			PostMessageW(hWnd, Metro::METRO_MULTITHREAD_MSG, MET_POST_PID_ADD, pi.dwProcessId);
			WaitForSingleObject(pi.hProcess, INFINITE);
			PostMessageW(hWnd, Metro::METRO_MULTITHREAD_MSG, MET_POST_PID_CLEAR, 0);
			GetExitCodeProcess(pi.hProcess, &dwExitCode);
			CloseHandle(pi.hProcess);
			if (dwExitCode != 0 &&dwExitCode!=1)
			{
				return false;
			}
			return true;
		}
		return false;
	}
}

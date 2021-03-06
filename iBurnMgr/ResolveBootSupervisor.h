/*********************************************************************************************************
* ResolveBootSupervisor.h
* Note: iBurnMgr ResolveBootSupervisor
* E-mail:<forcemz@outlook.com>
* Date: @2015.03
* Copyright (C) 2017 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#ifndef RESOLVEBOOTSUPERVISOR
#define RESOLVEBOOTSUPERVISOR
#ifndef _STRING_
#include <string>
#endif

namespace ResolveSupervisor{
	typedef struct _ResolveData{
		std::wstring latter;
		HWND m_hWnd;
	}ResolveData;
	DWORD WINAPI ResolveSupervisorThread(LPVOID lParam);
	enum DriveTarget{
		OSNT6=1,
		OSNT5=2
	};
	class ResolveSupervisor{
	public:
		static bool FixUSBDeviceBoot(LPCWSTR DeviceLetter);
		ResolveSupervisor();
	};
}

#endif

/*********************************************************************************************************
* APIController.h
* Note: iBurnMgr APIController
* E-mail:<forcemz@outlook.com>
* Date: @2015.03
* Copyright (C) 2015 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#ifndef APICONTROLLER_H
#define APICONTROLLER_H

#define NAMESPACEMETRO namespace Metro{
#define ENDNAMESPACE }

#define  METROCLASSNAME L"Force.Metro.Native.iBurnMgr"
#define  METRO_INTERNAL_WINDOWLNAME  L"Metro.Native.Window"

///Must Include Window.h
/// nameofimage must 32*1024-1
LRESULT DiscoverInstallerIMAGE(HWND hWnd, PWSTR nameofimage);

#define MET_INSPECTIONEC                        1
#define MET_DECOMPRESS				2
#define MET_RESOLVEBS				3
#define MET_POST_PID_ADD                	4
#define MET_POST_PID_CLEAR             		5
////PostMessage WPARAM
#define METRO_RATE_FORMAT			0 //IsInvalid ->false
#define METRO_RATE_STATUS			1
#define METRO_RATE_BLOCK          		2
#define METRO_RATE_ADNORMAL			3
#define METRO_RATE_ENDOK			4
///////
#define METRO_FIX_UNLOCK			5
#define METRO_FIX_STATUS			6
#define METRO_CANCLE_OPT         		7


////PostMessage LPARAM
#define METRO_RATE_UNLOCK_OK                    1
#define METRO_RATE_UNLOCK_ERR                   2
#define METRO_RATE_FORMAT_SUC			0
#define METRO_RATE_FORMAT_ERR			1

#define METRO_CANCLE_OPT_FORMAT		        0
#define METRO_CANCLE_DECOMPRESS			1
#define METRO_CANCLE_FIXBOOT			2
///METRO_RATE_STATUS ->lParam is Percentage progress




namespace Metro{
	extern const UINT  METRO_MULTITHREAD_MSG;
	extern const UINT METRO_THREAD_RATESTATUS_MSG;
	typedef struct _PidKillHlp{
		DWORD Pid;
		bool status;
	}PidKillHlp;
	typedef struct _ThreadMgrHlp{
		DWORD ThreadId;
		bool status;
	}ThreadMgrHlp;
}

 

#endif

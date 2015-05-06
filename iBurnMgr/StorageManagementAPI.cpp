/*********************************************************************************************************
* StorageManagementAPI.cpp
* Note: iBurnMgr StorageManagementAPI
* E-mail:<forcemz@outlook.com>
* Date: @2015.03
* Copyright (C) 2015 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#include "Precompiled.h"
#include <vds.h>
#include <Shlobj.h>
#include <Msi.h>

//#include <rpc.h>
#pragma comment(lib,"UUID.LIB")
#pragma comment(lib,"Shell32.lib")

#pragma region Disable_Format_Code
typedef struct
{
	DWORD Lines;
	PCHAR Output;
} TEXTOUTPUT, *PTEXTOUTPUT;

typedef enum
{
	FMIFS_UNKNOWN0,
	FMIFS_UNKNOWN1,
	FMIFS_UNKNOWN2,
	FMIFS_UNKNOWN3,
	FMIFS_UNKNOWN4,
	FMIFS_UNKNOWN5,
	FMIFS_UNKNOWN6,
	FMIFS_UNKNOWN7,
	FMIFS_FLOPPY,
	FMIFS_UNKNOWN9,
	FMIFS_UNKNOWN10,
	FMIFS_REMOVABLE,
	FMIFS_HARDDISK,
	FMIFS_UNKNOWN13,
	FMIFS_UNKNOWN14,
	FMIFS_UNKNOWN15,
	FMIFS_UNKNOWN16,
	FMIFS_UNKNOWN17,
	FMIFS_UNKNOWN18,
	FMIFS_UNKNOWN19,
	FMIFS_UNKNOWN20,
	FMIFS_UNKNOWN21,
	FMIFS_UNKNOWN22,
	FMIFS_UNKNOWN23,
} FMIFS_MEDIA_FLAG;
typedef enum
{
	PROGRESS,
	DONEWITHSTRUCTURE,
	UNKNOWN2,
	UNKNOWN3,
	UNKNOWN4,
	UNKNOWN5,
	INSUFFICIENTRIGHTS,
	FSNOTSUPPORTED,
	VOLUMEINUSE,
	UNKNOWN9,
	UNKNOWNA,
	DONE,
	UNKNOWNC,
	UNKNOWND,
	OUTPUT,
	STRUCTUREPROGRESS,
	CLUSTERSIZETOOSMALL,
} CALLBACKCOMMAND;
typedef BOOLEAN(__stdcall *PFMIFSCALLBACK)(CALLBACKCOMMAND Command, DWORD SubAction, PVOID ActionInfo);
typedef VOID(__stdcall *PFORMATEX)(PWCHAR DriveRoot, DWORD MediaFlag, PWCHAR Format, PWCHAR Label, BOOL QuickFormat, DWORD ClusterSize, PFMIFSCALLBACK Callback);
typedef BOOLEAN(__stdcall *PENABLEVOLUMECOMPRESSION)(PWCHAR DriveRoot, BOOL Enable);
PFORMATEX FormatEx;
PENABLEVOLUMECOMPRESSION EnableVolumeCompression;

//初始化
bool InitFormatEx()
{
	LoadLibrary(L"fmifs.dll");
	if (!(FormatEx = (PFORMATEX)GetProcAddress(GetModuleHandle(L"fmifs.dll"), "FormatEx")))
		return FALSE;
	if (!(EnableVolumeCompression = (PENABLEVOLUMECOMPRESSION)GetProcAddress(GetModuleHandle(L"fmifs.dll"), "EnableVolumeCompression")))
		return FALSE;
	return TRUE;
}

BOOLEAN __stdcall FormatExCallback(CALLBACKCOMMAND Command, DWORD Modifier, PVOID Argument)
{
	PDWORD percent;
	DWORD Error;
	PTEXTOUTPUT output;
	PBOOLEAN status;
	CHAR Buffer[4096];

	switch (Command)
	{
	case PROGRESS:    //格式化进度
		percent = (PDWORD)Argument;
		//DebugString("格式化进度: %d \n", *percent);
		break;

	case OUTPUT:
		output = (PTEXTOUTPUT)Argument;
		sprintf_s(Buffer, "%s", output->Output);
		break;

	case DONE:    //格式化完成
		status = (PBOOLEAN)Argument;
		if (*status == FALSE)
		{
			//DebugString("格式化未能成功完成(%d)\n", GetLastError());
			//MakePageFile(iMin, iMax, szPageFilePath);
			Error = TRUE;
		}
		else
		{
			//DebugString("格式化完成!");
			//MakePageFile(iMin, iMax, szPageFilePath);
		}

		break;
	default:
		break;
	}
	return TRUE;
}

bool FormatDriver(WCHAR *szDriver)
{
	PWCHAR Label = L"METROBOOT";
	WCHAR m_szDriver[10] = { 0 };
	wcscpy_s(m_szDriver, szDriver);
	wcscat_s(m_szDriver, L"\\");
	PWCHAR Format = L"NTFS";
	BOOL QuickFormat = TRUE;
	DWORD ClusterSize = 4096;

	if (!InitFormatEx())
		return FALSE;
	FormatEx(m_szDriver, FMIFS_FLOPPY, Format, Label, QuickFormat, ClusterSize, FormatExCallback);
	return true;
}
#pragma endregion

//DeviceIoControl  Invoke
BOOL WINAPI   FormatDiskIntegrated(LPCWSTR lpRoot)
{
	return TRUE;
}

//IVdsVolumeMF3
BOOL WINAPI  IVdsVolumeFormat()
{
	return TRUE;
}

/// use Storage Management API Only Windows 8 or later
BOOL WINAPI StorageManagementFormatAPI()
{
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////
/// Safe
DWORD WINAPI ShellFormatHD(HWND hWnd,UINT drive,UINT fmtId,UINT options)
{
	ATLASSERT(hWnd);
	return SHFormatDrive(hWnd, drive, fmtId, options);
}

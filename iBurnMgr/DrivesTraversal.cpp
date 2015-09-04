/*********************************************************************************************************
* DrivesTravesal.cpp
* Note: iBurnMgr DrivesTravesal
* E-mail:<forcemz@outlook.com>
* Date: @2015.03
* Copyright (C) 2015 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#include "Precompiled.h"
#include "EquipmentController.h"
#include "DecompressSupervision.h"

RemoveableDrive g_DriveList[26];


UINT WINAPI  DiscoverRemoveableDrives(ULONGLONG limitSize)
{
	UINT index = 0;
	wchar_t buffer[2048] = { 0 };
	wchar_t drivePath[9] = { 0 };
	wchar_t driveLetter[5] = { 0 };
	DWORD iSub = 0;
	DWORD iLength= GetLogicalDriveStringsW(2048, buffer);
	for (iSub = 0; iSub < iLength; iSub += 4)
	{
		wcscpy_s(drivePath, buffer+iSub);
		
		if (GetDriveTypeW(drivePath) == DRIVE_REMOVABLE&&GetVolumeInformationW(drivePath, NULL, 0, NULL, NULL, NULL, NULL, 0) == TRUE)
		{
			wcscpy_s(driveLetter, drivePath);
			(wcsrchr(driveLetter, _T(':')))[1] = 0;
			ULARGE_INTEGER lpFreeToCaller;
			ULARGE_INTEGER lpTotalSize;
			ULARGE_INTEGER lpFreeSize;
			if (GetDiskFreeSpaceExW(drivePath, &lpFreeToCaller, &lpTotalSize, &lpFreeSize) ==TRUE)
			{
				if (limitSize>0){
					if (lpTotalSize.QuadPart < limitSize)
						continue;
				}
				swprintf_s(g_DriveList[index].sizeInfo, L"%s  The total size of USB device: %4.1f GB |Free Space: %4.1f GB",
					drivePath,
					(float)(lpTotalSize.QuadPart) / (1024 * 1024 * 1024),
					(float)(lpFreeSize.QuadPart) / (1024 * 1024 * 1024));
				wcscpy_s(g_DriveList[index].driveLetter, driveLetter);
				wcscpy_s(g_DriveList[index].drivePath, drivePath);
				index++;
			}
		}
	}
	return index;
}

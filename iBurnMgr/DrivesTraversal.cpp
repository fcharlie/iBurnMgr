/*********************************************************************************************************
* DrivesTravesal.cpp
* Note: iBurnMgr DrivesTravesal
* E-mail:<forcemz@outlook.com>
* Data: @2015.03
* Copyright (C) 2015 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#include "Precompiled.h"
#include "EquipmentController.h"

DrivesEquipment DrivesList[26];


UINT WINAPI  TraversalEquipment()
{
	UINT xdw = 0;
	TCHAR Buffer[2048] = { 0 };
	TCHAR drivePath[9] = { 0 };
	TCHAR driveLetter[5] = { 0 };
	//dw = GetLogicalDrives();
	DWORD iSub = 0;
	DWORD iLength= GetLogicalDriveStrings(2048, Buffer);
	for (iSub = 0; iSub < iLength; iSub += 4)
	{
		wcscpy_s(drivePath, Buffer+iSub);
		if (GetDriveType(drivePath) == DRIVE_REMOVABLE)
		{
			if (GetVolumeInformation(drivePath, 0, 0, 0, 0, 0, 0, 0))
			{
				
				wcscpy_s(driveLetter, drivePath);
				(wcsrchr(driveLetter, _T(':')))[1] = 0;
				ULARGE_INTEGER lpFreeToCaller;
				ULARGE_INTEGER lpTotalSize;
				ULARGE_INTEGER lpFreeSize;
				TCHAR SizeInfoTo[70] = { 0 };
				TCHAR SizeInfoFree[70] = { 0 };
				if (GetDiskFreeSpaceEx(drivePath, &lpFreeToCaller, &lpTotalSize, &lpFreeSize) != 0)
				{

					swprintf_s(SizeInfoTo, L"  The total size of USB device: %4.1f GB\n", (float)(lpTotalSize.QuadPart) / (1024 * 1024 * 1024));
					swprintf_s(SizeInfoFree, L"  |Free Space: %4.1f GB\n", (float)(lpFreeSize.QuadPart) / (1024 * 1024 * 1024));
					wcscpy_s(DrivesList[xdw].driveLetter, driveLetter);
					wcscpy_s(DrivesList[xdw].SizeInfo, drivePath);
					wcscat_s(DrivesList[xdw].SizeInfo, SizeInfoTo);
					wcscat_s(DrivesList[xdw].SizeInfo, SizeInfoFree);
				}
				wcscpy_s(DrivesList[xdw].DrivesPath, drivePath);
				DrivesList[xdw].Id = xdw;
				xdw++;

			}
		}
	}
	return xdw;
}
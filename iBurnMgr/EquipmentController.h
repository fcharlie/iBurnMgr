/*********************************************************************************************************
* EquipmentController.h
* Note: iBurnMgr EquipmentController
* E-mail:<forcemz@outlook.com>
* Date: @2015.03
* Copyright (C) 2015 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#ifndef EQUIPMENTCONTROLLER_H
#define EQUIPMENTCONTROLLER_H

typedef struct _RemoveableDrive{
	wchar_t driveLetter[4];
	wchar_t drivePath[8];
	wchar_t sizeInfo[160];
}RemoveableDrive;

UINT WINAPI  DiscoverRemoveableDrives(ULONGLONG limitSize = 0);

extern RemoveableDrive g_DriveList[26];
#endif

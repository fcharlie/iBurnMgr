/*********************************************************************************************************
* EquipmentController.h
* Note: iBurnMgr EquipmentController
* E-mail:<forcemz@outlook.com>
* Date: @2015.03
* Copyright (C) 2015 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#ifndef EQUIPMENTCONTROLLER_H
#define EQUIPMENTCONTROLLER_H

typedef struct _Equipment{
	unsigned int Id;
	wchar_t driveLetter[5];
	wchar_t DrivesPath[9];
	wchar_t SizeInfo[260];
}DrivesEquipment;

UINT WINAPI  TraversalEquipment();

extern DrivesEquipment DrivesList[26];
#endif

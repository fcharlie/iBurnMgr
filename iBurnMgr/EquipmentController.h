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
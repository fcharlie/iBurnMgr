/*********************************************************************************************************
* UniversalDiscFormatDecode.cpp
* Note: iBurnMgr UniversalDiscFormatDecode
* E-mail:<forcemz@outlook.com>
* Date: @2015.03
* Copyright (C) 2015 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#include "Precompiled.h"
#include "UniversalDiscFormat.h"
#include <imapi2fs.h>

template<class Interface>
inline void
SafeRelease(
Interface **ppInterfaceToRelease
)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}

///Namespace begin
NAMESPACEMETRO
UNNAMESPACE

UniversalDiscFormatDecode::UniversalDiscFormatDecode(std::wstring image, std::wstring drive)
:m_Image(image), 
m_drive(drive),
m_pIDiscFormat2Data(NULL)
{

}
////////Clear resource
UniversalDiscFormatDecode::~UniversalDiscFormatDecode()
{
	SafeRelease(&m_pIDiscFormat2Data);
}

///Namespace End
UNNAMESPACEEND


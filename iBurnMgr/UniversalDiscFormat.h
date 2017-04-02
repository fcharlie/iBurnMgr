/*********************************************************************************************************
* UniversalDiskFormatModel.h
* Note: iBurnMgr UniversalDiskFormatModel
* E-mail:<forcemz@outlook.com>
* Date: @2015.03
* Copyright (C) 2017 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#ifndef UniversalDiskFormat
#define UniversalDiskFormat

#define DISABLE_NOW

#include <string>
#include <imapi2.h>
#include <imapi2error.h>
#include <imapi2fs.h>
#include <imapi2fserror.h>
#include "APIController.h"

///////////
#define UNNAMESPACE namespace Decode{
#define UNNAMESPACEEND }}
//START
NAMESPACEMETRO
UNNAMESPACE

typedef HRESULT(*DecodeRate)(float, void *data);

class UniversalDiscFormatDecode{
private:
	IDiscFormat2Data* m_pIDiscFormat2Data;
	std::wstring m_Image;
	std::wstring m_drive;
public:
	enum DiscFormats{
		DiscISO9660=1,
		DiscJoliet=2,
		DiscUDF=3
	};
	UniversalDiscFormatDecode(std::wstring &image,std::wstring &drive);
	~UniversalDiscFormatDecode();
	HRESULT Decode(DecodeRate rate,void *data);
};


////End
UNNAMESPACEEND
#endif

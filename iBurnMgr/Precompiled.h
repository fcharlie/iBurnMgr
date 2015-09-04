/*********************************************************************************************************
* Precompiled.h
* Note: iBurnMgr Precompiled
* E-mail:<forcemz@outlook.com>
* Date: @2015.03
* Copyright (C) 2015 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#pragma once
#include <Windows.h>
#include <stdio.h>
#include <shlobj.h>
#include <shlguid.h>

#include <atlbase.h>
#include <atlapp.h>
#include <assert.h>

#ifndef MAX_UNC_PATH
#define MAX_UNC_PATH  (32*1024-1)
#endif

extern CAppModule _Module;

#include <atlwin.h>
#include <atlctl.h>
#include <atlctrls.h>

class CoInitializeSignal{
private:
	CoInitializeSignal(){
		auto hr=CoInitializeSecurity(
			NULL,
			-1,
			NULL,
			NULL,
			RPC_C_AUTHN_LEVEL_CONNECT,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			0,
			NULL
			);
		assert(hr);
	}
	CoInitializeSignal(CoInitializeSignal &);
	CoInitializeSignal &operator=(const CoInitializeSignal &);
public:
	~CoInitializeSignal(){
		CoUninitialize();
	}
	static CoInitializeSignal *Initialize(){
		static CoInitializeSignal in;
		return &in;
	}
};


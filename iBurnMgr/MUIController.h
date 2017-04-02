/*********************************************************************************************************
* MUIController.h
* Note: iBurnMgr MUIController
* E-mail:<forcemz@outlook.com>
* Date: @2015.03
* Copyright (C) 2017 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#ifndef METROMUI_CONTROLLER
#define METROMUI_CONTROLLER



#include <string>
#include <map>
#include <vector>
#include <Windows.h>
#include <fstream>

namespace Metro{
	namespace MUI{
		typedef struct _LocaleInfoTags{
			std::wstring localename;
			DWORD lcid;
		}LocaleInfo;
		typedef struct _LocnameBase{
			const char *name;
			DWORD id;
		}LocnameBase;
		class MultiUI{
		private:
			DWORD UILcId;
			std::wstring ifilename;
			std::map<std::wstring, std::wstring> m_langTree;
		public:
			MultiUI();
			std::wstring as_string(std::wstring &key,std::wstring &value);
			std::wstring as_string(const wchar_t *key, const wchar_t *value);
			bool loading();
			DWORD initialize();
		};
		extern MultiUI mutliui;
	}
}


#endif

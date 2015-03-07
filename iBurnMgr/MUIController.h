/*********************************************************************************************************
* MUIController.h
* Note: iBurnMgr MUIController
* E-mail:<forcemz@outlook.com>
* Data: @2015.03
* Copyright (C) 2015 The ForceStudio All Rights Reserved.
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
		class MUIController{
		private:
			DWORD UILcId;
			std::wstring ifilename;
			std::map<std::wstring, std::wstring> m_langTree;
		public:
			MUIController();
			std::wstring atString(std::wstring &key,std::wstring &value);
			std::wstring atString(const wchar_t *key, const wchar_t *value);
			bool MUIResourceLoader();
			DWORD Init();
		};
		extern MUIController muiController;
	}
}


#endif
/*********************************************************************************************************
* MUIController.cpp
* Note: iBurnMgr MUIController
* E-mail:<forcemz@outlook.com>
* Date: @2015.03
* Copyright (C) 2015 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#include "Precompiled.h"
#include "APIController.h"
#include "MUIController.h"
#include <stdio.h>
#include <stdlib.h>
#include <locale>
#include <fstream>

NAMESPACEMETRO
MUI::LocaleInfo localeinfo = {L"0",0};
namespace MUI{
	MUIController muiController;
	static LocnameBase localbase[] = {
		{ "chs", 2052 },
		{"cht", 1028},
		{"enu",1033}
	};
	const char *getLocaleName(DWORD lcid)
	{
		for (auto i : localbase)
		{
			if (i.id == lcid)
				return i.name;
		}
		return "english";
	}

	MUIController::MUIController()
	{

	}
	DWORD MUIController::Init()
	{
		UILcId = GetSystemDefaultLCID();
		WCHAR szLocal[LOCALE_NAME_MAX_LENGTH] = { 0 };
		WCHAR szlangFile[MAX_UNC_PATH] = {0};
		DWORD eror = ERROR_SUCCESS;
		std::wstring initfile;
		std::wstring langFile;
		GetModuleFileNameW(GetModuleHandleW(NULL), szlangFile, MAX_UNC_PATH);
		initfile = szlangFile;
		initfile+=L".ini";
		if (_waccess_s(initfile.c_str(), 4) == 0){
			int x=GetPrivateProfileStringW(L"iBurnMgr.Config", L"LocaleName", NULL, szLocal, LOCALE_NAME_MAX_LENGTH, initfile.c_str());
			localeinfo.lcid = GetPrivateProfileIntW(L"iBurnMgr.Config", L"LCID", NULL, initfile.c_str());
		}
		if (localeinfo.lcid == 0)
			localeinfo.lcid = UILcId;
		if (!wcslen(szLocal))
		{
			if (LCIDToLocaleName(UILcId, szLocal, LOCALE_NAME_MAX_LENGTH, LOCALE_ALLOW_NEUTRAL_NAMES) == 0)
			{
				eror = GetLastError();
			}
		}
		localeinfo.localename = szLocal;
		(wcsrchr(szlangFile, L'\\'))[1]=0;
		langFile = szlangFile;
		langFile += szLocal;
		langFile += L".lang";
		if (!_waccess_s(langFile.c_str(), 0) == 0)
		{
			langFile = szlangFile;
			langFile+=L"en.lang";
			localeinfo.localename = L"en-US";
		}
		this->ifilename = langFile;
		if (!MUIResourceLoader())
		{
			return 1;
		}
		//for (auto x : m_langTree)
		//{
		//	MessageBoxW(nullptr, x.second.c_str(), x.first.c_str(), MB_OK);
		//}
		return 0;
	}
	std::wstring MUIController::atString(std::wstring &key, std::wstring &value)
	{
		auto iter = m_langTree.find(key);
		if (iter == m_langTree.end())
			return value;
		return m_langTree.at(key);
	}
	std::wstring MUIController::atString(const wchar_t *key, const wchar_t *value)
	{
		std::wstring k = key;
		std::wstring v = value;
		return atString(k, v);
	}
	bool MUIController::MUIResourceLoader()
	{
		wchar_t szBuffer[4096];
		std::wstring newLineStr = L"\\n";
		std::wstring newLine = L"\n";
		std::wstring bufferString;
		std::wifstream file(this->ifilename);
		file.imbue(std::locale(getLocaleName(UILcId)));
		if (!file.is_open())
			return false;
		while (!file.eof())
		{
			file.getline(szBuffer,4096);
			bufferString = szBuffer;
			if (bufferString[0] != '#'&&bufferString[0] != '/'&&bufferString[0]!='[')
			{
				auto np = bufferString.find('=');
				if (np != bufferString.npos)
				{
					std::wstring value = bufferString.substr(np + 1);
					size_t index=0;
					while ((index = value.find(L"\\n")) != value.npos)
					{
						value.replace(index, 2, L"\n");
					}
					m_langTree.insert(std::pair<std::wstring, std::wstring>(bufferString.substr(0, np),value));
				}
			}
		}
		return true;
	}		
}
ENDNAMESPACE

#include "Precompiled.h"
#include "APIController.h"
#include "MUIController.h"
#include <stdio.h>
#include <stdlib.h>
#include <locale>

NAMESPACEMETRO
MUI::LocaleHlp localehlp = {L"0",0};
namespace MUI{
	std::auto_ptr<MUIController> MUIController::m_pInstance(NULL);
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
			localehlp.lcid = GetPrivateProfileIntW(L"iBurnMgr.Config", L"LCID", NULL, initfile.c_str());
		}
		if (localehlp.lcid == 0)
			localehlp.lcid = UILcId;
		if (!wcslen(szLocal))
		{
			if (LCIDToLocaleName(UILcId, szLocal, LOCALE_NAME_MAX_LENGTH, 0) == 0)
			{
				eror = GetLastError();
			}
		}
		localehlp.localename = szLocal;
		(wcsrchr(szlangFile, L'\\'))[1]=0;
		langFile = szlangFile;
		langFile += szLocal;
		langFile += L".lang";
		if (!_waccess_s(langFile.c_str(), 0) == 0)
		{
			langFile = szlangFile;
			langFile+=L"en.lang";
			localehlp.localename = L"en-US";
		}
		this->ifilename = langFile;
		if (!MUIInitLangFileFromatParse())
		{
			return 1;
		}
		return 0;
	}
	MUIController* MUIController::Instance()
	{
		if (!m_pInstance.get())
		{
			m_pInstance = std::auto_ptr < MUIController>(new MUIController());
		}
		return m_pInstance.get();
	}

	bool MUIController::MUIInitLangFileFromatParse()
	{
		m_line = std::vector<std::wstring>();
		langfile = std::wifstream(ifilename.c_str(), std::ios::binary);
		std::wstring str;
		wchar_t szLine[4069] = { 0 };
		langfile.imbue(std::locale(""));
		//wchar_t ch;
		//size_t index = 2;
		//int line=0;
		//while (!langfile.eof())
		//{
		//	langfile.seekg(1, std::ios::beg);
		//	wchar_t ch;
		//	langfile.read(&ch,1);
		//	if (ch = 0x000D)
		//	{
		//		MessageBox(nullptr, str.c_str(), L"0", MB_OK);
		//		line++;
		//		str.clear();
		//	}
		//	else{
		//		index += 2;
		//		str.append(ch, 1);
		//	}
		//}

		while (std::getline(langfile, str))
		{
			if (!str.find(L"#") == 0 && !str.find(L";") == 0)
			{
				std::string::size_type npos = str.find(L"\\n");
				if (npos < str.length())
				{
					str.replace(npos, 2, L"\n");
				}
				//MessageBox(NULL, str.c_str(), L"zr", MB_OK);
				m_line.push_back(str);
			}
			}
		this->langfile.close();
		this->m_langmap =std::map<std::wstring, std::wstring>();
		std::vector<std::wstring>::iterator it = m_line.begin();
		std::wstring s1, s2;
		while (it != this->m_line.end())
		{
			s1 = it->substr(0, it->find(L"="));
			s2 = it->substr(it->find(L"=") + 1, it->size() - 1);
			this->m_langmap.insert(std::map<std::wstring, std::wstring>::value_type(s1, s2));
			it++;
		}
		return true;
	}		
}
ENDNAMESPACE

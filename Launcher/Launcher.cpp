/*********************************************************************************************************
* Launcher.cpp
* Note: iBurnMgr Launcher
* E-mail:<forcemz@outlook.com>
* Date: @2015.03
* Copyright (C) 2015 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#include <Windows.h>
#include <wchar.h>
#include <Shlwapi.h>
#include <Shlobj.h>
#include <string>

#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"Shell32.lib")

#define UNC_PATH 32 * 1024 - 1

int WINAPI  wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPreInst,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow
	)
{
	WCHAR LauncherAppDirectory[MAX_PATH] = { 0 };
	WCHAR Home[MAX_PATH] = { 0 };
	///////
	WCHAR ifile[UNC_PATH] = { 0 };
	WCHAR LauncherAppPath[UNC_PATH] = { 0 };

	GetModuleFileNameW(hInstance, ifile, UNC_PATH);
	PathRemoveFileSpecW(ifile);
	std::wstring launcherDir = ifile;

	wcscat_s(ifile, L"\\launcher.ini");
	
	SHGetSpecialFolderPathW(NULL, Home, CSIDL_PERSONAL, FALSE);
	if (!PathFileExistsW(ifile)){
		MessageBoxW(nullptr, ifile, L"Not found iBurnMgr launcher configure [launcher.ini]", MB_OK | MB_ICONERROR);
		return 1;
	}
	PathRemoveFileSpecW(Home);
	GetPrivateProfileStringW(L"Launcher", L"LauncherAppPath", L"LauncherApp.exe", LauncherAppPath, UNC_PATH, ifile);
	GetPrivateProfileStringW(L"Launcher", L"LauncherAppDirectory", Home, LauncherAppDirectory, MAX_PATH, ifile);
	launcherDir += L"\\";
	launcherDir += LauncherAppPath;
	if (PathIsUNCW(LauncherAppPath));
	else if (LauncherAppPath[1] == ':');
	else if (PathFileExistsW(launcherDir.c_str())&&PathIsExe(launcherDir.c_str())){
		auto ret = ShellExecuteW(NULL, L"runas", launcherDir.c_str(), NULL, LauncherAppDirectory, SW_NORMAL);
		return ret != nullptr;
	}else{
		MessageBoxW(nullptr, LauncherAppPath, L"Not found vaild program ", MB_OK | MB_ICONERROR);
		return 1;
	}
	if (PathFileExistsW(LauncherAppPath) && PathIsExe(LauncherAppPath)){
		auto ret = ShellExecuteW(NULL, L"runas", LauncherAppPath, NULL, LauncherAppDirectory, SW_NORMAL);
		return ret != nullptr;
	}
	else{
		MessageBoxW(nullptr, LauncherAppPath, L"Cannot run this file", MB_OK | MB_ICONERROR);
	}
	return 0;
}

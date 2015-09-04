/*********************************************************************************************************
* StorageManagementAPI.cpp
* Note: iBurnMgr StorageManagementAPI
* E-mail:<forcemz@outlook.com>
* Date: @2015.03
* Copyright (C) 2015 The ForceStudio All Rights Reserved.
**********************************************************************************************************/
#include "Precompiled.h"
#include <Shlobj.h>
#include <winioctl.h>

#define DEFINED_GUID
#include <initguid.h>
#include <vds.h>
#pragma warning(disable: 4127) 
#include "DecompressSupervision.h"

#define SAFE_RELEASE(x)     if (x) { x->Release(); x = NULL; } 
#define SAFE_FREE(x)        if (x) { CoTaskMemFree(x); } 
#define GUID_LEN 50 

#define VAILED_LATTER(c) (c>='A'&&c<='Z'||c>='a'&&c<='z')

#pragma comment(lib,"UUID.LIB")
#pragma comment(lib,"Shell32.lib")

//wchar_t *HealthString[] =
//{
//	L"UNKNOWN",
//	L"HEALTHY",
//	L"REBUILDING",
//	L"STALE",
//	L"FAILING",
//	L"FAILING_REDUNDANCY",
//	L"FAILED_REDUNDANCY",
//	L"FAILED_REDUNDANCY_FAILING",
//	L"FAILED"
//};
//
//wchar_t *TypeString[] =
//{
//	L"UNKNOWN",
//	L"SIMPLE",
//	L"SPAN",
//	L"STRIPE",
//	L"MIRROR",
//	L"PARITY"
//};
//
//wchar_t *StatusString[] =
//{
//	L"UNKNOWN",
//	L"ONLINE",
//	L"NOT_READY",
//	L"NO_MEDIA",
//	L"FAILED"
//};
//
//wchar_t *TransitionStateString[] =
//{
//	L"UNKNOWN",
//	L"STABLE",
//	L"EXTENDING",
//	L"SHRINKING",
//	L"RECONFIGING"
//};
//
//wchar_t *FileSystemString[] =
//{
//	L"UNKNOWN",
//	L"RAW",
//	L"FAT",
//	L"FAT32",
//	L"NTFS",
//	L"CDFS",
//	L"UDF"
//};
//
//wchar_t *ExtentTypeString[] =
//{
//	L"UNKNOWN",
//	L"FREE",
//	L"DATA",
//	L"OEM",
//	L"ESP",
//	L"MSR",
//	L"LMD",
//	L"UNUSABLE"
//};

typedef struct EMessage{
	HRESULT id;
	const wchar_t *msg;
}EMessage;

static const wchar_t *ErrorCodesMessage(HRESULT hr){
	EMessage em[] = {
		{ 
			VDS_E_OPERATION_DENIED, 
			L"The operation is denied if the caller tries to format the system, boot, crashdump, hibernation, or pagefile volume." 
		},
		{ 
			VDS_E_OBJECT_STATUS_FAILED ,
			L"The volume has failed."
		}, {
			VDS_E_PACK_OFFLINE,
			L"The pack containing the volume is not accessible. All volumes in an offline pack are inaccessible."
		}, {
			VDS_E_FS_NOT_DETERMINED,
			L"The default file system could not be determined."
		}, {
			VDS_E_INCOMPATIBLE_FILE_SYSTEM,
			L"The file system is incompatible."
		}, {
			VDS_E_INCOMPATIBLE_MEDIA,
			L"The media is incompatible."
		}, {
			VDS_E_ACCESS_DENIED,
			L"Access is denied."
		}, {
			VDS_E_MEDIA_WRITE_PROTECTED,
			L"The media is write-protected."
		}, {
			VDS_E_BAD_LABEL,
			L"The label is not valid."
		},{
			VDS_E_CANT_QUICK_FORMAT,
			L"The volume cannot be quick-formatted."
		}, {
			VDS_E_IO_ERROR,
			L"An I/O error occurred during format."
		}, {
			VDS_E_VOLUME_TOO_SMALL,
			L"The volume size is too small to format."
		}, {
			VDS_E_VOLUME_TOO_BIG,
			L"The volume size is too large to format."
		}, {
			VDS_E_CLUSTER_SIZE_TOO_SMALL,
			L"The cluster size is too small to allow formatting."
		}, {
			VDS_E_CLUSTER_SIZE_TOO_BIG,
			L"The cluster size is too large to allow formatting."
		}, {
			VDS_E_CLUSTER_COUNT_BEYOND_32BITS,
			L"The number of clusters is too large to be represented as a 32-bit integer."
		}, {
			VDS_S_VOLUME_COMPRESS_FAILED,
			L"The file system is formatted but not compressed."
		}, {
			VDS_E_CANT_INVALIDATE_FVE,
			L"BitLocker encryption could not be disabled for the volume."
		}
	};
	for (auto &i : em){
		if (i.id == hr){
			return i.msg;
		}
	}
	return L"UNKNOWN ERROR";
}

/*
VDS_FSOF_NONE
No options are specified.
VDS_FSOF_FORCE
The format operation should be forced, even if the partition is in use.
VDS_FSOF_QUICK
Perform a quick format operation. A quick format does not verify each sector on the volume.
VDS_FSOF_COMPRESSION
Enable compression on the newly formatted file system volume. Compression is a feature of the NTFS file system; it cannot be set for other file systems such as FAT or FAT32.
VDS_FSOF_DUPLICATE_METADATA
Forces duplication of metadata for UDF 2.5 and above

*/
BOOL WINAPI  IVdsVolumeFormat(
	LPCWSTR    latter,
	LPWSTR    pwszLabel,
	FormatFailedCallback fcall,
	void *data)
{
	if (!VAILED_LATTER(latter[0]) || latter[1] != ':') return FALSE;
	HRESULT hr, AsyncHr;
	IVdsAsync      *pAsync = NULL;
	VDS_ASYNC_OUTPUT AsyncOut;
	if (CoInitializeSignal::Initialize()==nullptr){
		return false;
	}
	IVdsServiceLoader *pLoader;
	IUnknown *pUnk;
	ULONG ulFetched = 0;
	hr = CoCreateInstance(CLSID_VdsLoader,
		NULL,
		CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER,
		IID_IVdsServiceLoader,
		(void **)&pLoader
		);
	if (hr != S_OK) return false;
	IVdsService *pService;
	hr = pLoader->LoadService(NULL, &pService);
	pLoader->Release();
	pLoader = NULL;
	if (hr != S_OK){
		if (fcall){
			fcall(ErrorCodesMessage(hr),false,data);
		}
		return FALSE;
	}
	VDS_DRIVE_LETTER_PROP mDriveLetterPropArray[1];
	hr = pService->QueryDriveLetters(latter[0], 1, mDriveLetterPropArray);
	if (hr != S_OK){
		goto _bailout;
	}
	hr = pService->GetObjectW(mDriveLetterPropArray->volumeId, VDS_OT_VOLUME, &pUnk);
	if (hr != S_OK){
		goto _bailout;
	}
	IVdsVolume *pVolume;
	hr = pUnk->QueryInterface(IID_IVdsVolume, (void **)&pVolume);
	if (hr != S_OK){
		SAFE_RELEASE(pUnk);
		goto _bailout;
	}
	IVdsVolumeMF3 *pVolumeMF3;
	hr = pVolume->QueryInterface(IID_IVdsVolumeMF3, (void **)&pVolumeMF3);

	hr = pVolumeMF3->FormatEx2(L"NTFS", 1, 0, pwszLabel, VDS_FSOF_QUICK, &pAsync);
	hr = pAsync->Wait(&AsyncHr, &AsyncOut);
	if (FAILED(hr)){
		if (fcall){
			fcall(ErrorCodesMessage(hr),false,data);
		}
	}
	else if (FAILED(AsyncHr)){
		if (fcall){
			fcall(ErrorCodesMessage(hr),false,data);
		}
	}
	else{
		if (fcall){
			fcall(L"Format volume Success done. ", true,data);
		}
	}
	SAFE_RELEASE(pVolume);
	SAFE_RELEASE(pVolumeMF3);
_bailout:
	SAFE_RELEASE(pService);
	return hr==S_OK;
}


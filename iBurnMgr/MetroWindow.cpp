#include "Precompiled.h"
#include "MetroWindow.h"
#include "ToString.h"
#include "resource.h"
#include "EquipmentController.h"

//Model
#include "DecompressSupervision.h"
#include "InspectionEnvironment.h"
#include "ResolveBootSupervisor.h"

//#include <d2d1helper.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib,"dwrite.lib")
#pragma comment (lib,"Uxtheme.lib")
#pragma comment(lib,"dwmapi.lib")
#pragma comment(lib,"windowscodecs.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib,"imm32.lib")
#pragma comment(lib,"msimg32.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"usp10.lib")

NAMESPACEMETRO
extern MUI::LocaleInfo localeinfo;
TCHAR SizeStr[120] = { L" 0 Byte\0" };
#define DEFAULT_NOTES L"Irreversible behavior(Format),Task running don't remove or insert the device\nSupport: Windows 7 or Later And Corresponding Server OS"
std::wstring Notes;
static Decompress::SupervisorData SpData = {L"null",L"null",0};
static ResolveSupervisor::ResolveData reData = { L"null", NULL };
const UINT METRO_MULTITHREAD_MSG = RegisterWindowMessage(L"METRO_MULTITHREAD_MSG");
const UINT METRO_THREAD_RATESTATUS_MSG = RegisterWindowMessage(L"METRO_THREAD_RATESTATUS_MSG");
static std::wstring envinfo=L"No Check Environment";

static Metro::PidKillHlp pidkill = { 0, false };

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
#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}
typedef  D2D1_RECT_F FRECT;

FRECT  ToFloatRect(RECT rect)
{
	D2D_RECT_F frect;
	frect.left = (float)rect.left;
	frect.bottom = (float)rect.bottom;
	frect.right = (float)rect.right;
	frect.top = (float)rect.top;
	return frect;
}
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif


HRESULT LoadResourceBitmap(
	ID2D1RenderTarget* pRendertarget,
	IWICImagingFactory* pIWICFactory,
	PCWSTR resourceName,
	PCWSTR resourceType,
	UINT destinationWidth,
	UINT destinationHeight,
	ID2D1Bitmap** ppBitmap
	)
{
	HRESULT hr = S_OK;

	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICStream* pStream = NULL;
	IWICFormatConverter* pConverter = NULL;
	IWICBitmapScaler* pScaler = NULL;

	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	void* pImageFile = NULL;
	DWORD imageFileSize = 0;

	// Find the resource then load it
	imageResHandle = FindResource(HINST_THISCOMPONENT, resourceName, resourceType);
	hr = imageResHandle ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);

		hr = imageResDataHandle ? S_OK : E_FAIL;
	}

	// Lock the resource and calculate the image's size
	if (SUCCEEDED(hr))
	{
		// Lock it to get the system memory pointer
		pImageFile = LockResource(imageResDataHandle);

		hr = pImageFile ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr))
	{
		// Calculate the size
		imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);

		hr = imageFileSize ? S_OK : E_FAIL;
	}

	// Create an IWICStream object
	if (SUCCEEDED(hr))
	{
		// Create a WIC stream to map onto the memory
		hr = pIWICFactory->CreateStream(&pStream);
	}

	if (SUCCEEDED(hr))
	{
		// Initialize the stream with the memory pointer and size
		hr = pStream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(pImageFile),
			imageFileSize
			);
	}

	// Create IWICBitmapDecoder
	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream
		hr = pIWICFactory->CreateDecoderFromStream(
			pStream,
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
			);
	}

	// Retrieve a frame from the image and store it in an IWICBitmapFrameDecode object
	if (SUCCEEDED(hr))
	{
		// Create the initial frame
		hr = pDecoder->GetFrame(0, &pSource);
	}

	// Before Direct2D can use the image, it must be converted to the 32bppPBGRA pixel format.
	// To convert the image format, use the IWICImagingFactory::CreateFormatConverter method to create an IWICFormatConverter object, then use the IWICFormatConverter object's Initialize method to perform the conversion.
	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{
		// If a new width or height was specified, create and
		// IWICBitmapScaler and use it to resize the image.
		if (destinationWidth != 0 || destinationHeight != 0)
		{
			UINT originalWidth;
			UINT originalHeight;
			hr = pSource->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr))
			{
				if (destinationWidth == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
					destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
				}
				else if (destinationHeight == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
					destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				hr = pIWICFactory->CreateBitmapScaler(&pScaler);
				if (SUCCEEDED(hr))
				{
					hr = pScaler->Initialize(
						pSource,
						destinationWidth,
						destinationHeight,
						WICBitmapInterpolationModeCubic
						);
					if (SUCCEEDED(hr))
					{
						hr = pConverter->Initialize(
							pScaler,
							GUID_WICPixelFormat32bppPBGRA,
							WICBitmapDitherTypeNone,
							NULL,
							0.f,
							WICBitmapPaletteTypeMedianCut
							);
					}
				}
			}
		}

		else // use default width and height
		{
			hr = pConverter->Initialize(
				pSource,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				NULL,
				0.f,
				WICBitmapPaletteTypeMedianCut
				);
		}
	}

	// Finally, Create an ID2D1Bitmap object, that can be drawn by a render target and used with other Direct2D objects
	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap
		hr = pRendertarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
			);
	}

	SAFE_RELEASE(pDecoder);
	SAFE_RELEASE(pSource);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(pConverter);
	SAFE_RELEASE(pScaler);

	return hr;
}


MetroWindow::MetroWindow() :m_pDirect2dFactory(NULL),
m_pRenderTarget(NULL),
m_TitleClinetBrush(NULL),
m_pMinButtonActiveBrush(NULL),
m_pMetroButtonNsBrush(NULL),
m_pMetroButtonLsBrush(NULL),
m_pUITextBrush(NULL),
m_pLightWhiteBrush(NULL),
m_pControlTextBrush(NULL),
m_pDWriteTypography(NULL),
m_pCloseButtonClickBrush(NULL),
m_pWICFactory(NULL),
m_pBitmap(NULL),
m_pITextFormatTitle(NULL),
m_pITextFormatContent(NULL),
m_pIDWriteFactory(NULL),
IsInvalid(false),
_bMouseTrack(TRUE),
iseThreadID(0),
dwExit(0)
{
	if (IsUserAnAdmin())
	{
		windowTitle = L"Metro USB Drives Boot Manager [";
		windowTitle += MUI::muiController.atString(L"Adm", L"Administrator") + L"]";
	}
	else{
		windowTitle = L"Metro USB Drives Boot Manager";
	}
	Notes =MUI::muiController.atString(L"NoticeInfo", DEFAULT_NOTES);
	if (localeinfo.lcid == 2052)
	{
		FontTabel = 1;
	}
	else{
		FontTabel = 0;
	}
	MTNotices = L"Notices Center:";
	JobStatusRate = L"Task not start";
	ProcessInfo = L"Manager Task Rate:";
	copyright = L"Copyright \xA9 2015 The ForceStudio.";
	m_mbFind.bStatus = false;
	m_mbFind.caption =MUI::muiController.atString(L"OpenImage",L"Open Image...");
	m_FixBoot.bStatus = false;
	m_FixBoot.caption = MUI::muiController.atString(L"Fixboot", L"Fix Boot");
	m_Operate.bStatus = false;
	//m_Operate.caption =muiController->m_langmap[L"BMake"];
	m_Operate.caption = MUI::muiController.atString(L"BMake",L"Expand Image");
	USBdrive = MUI::muiController.atString(L"USBdrive", L"USB Drives:");
	ImageFile = MUI::muiController.atString(L"ImageFile", L"Image File:");
	ImageSize = MUI::muiController.atString(L"ImageSize", L"Image File:");
	Description = MUI::muiController.atString(L"Description", L"Description:");

	normalFont = MUI::muiController.atString(L"NormalFont", L"Segoe UI");
	
}
MetroWindow::~MetroWindow()
{
	SafeRelease(&m_pDirect2dFactory);
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_TitleClinetBrush);
	SafeRelease(&m_pMinButtonActiveBrush);
	SafeRelease(&m_pMetroButtonNsBrush);
	SafeRelease(&m_pMetroButtonLsBrush);
	SafeRelease(&m_pLightWhiteBrush);
	SafeRelease(&m_pUITextBrush);
	SafeRelease(&m_pCloseButtonClickBrush);
	SafeRelease(&m_pControlTextBrush);
	SafeRelease(&m_pDWriteTypography);
	SafeRelease(&m_pIDWriteFactory);
	SafeRelease(&m_pITextFormatTitle);
	SafeRelease(&m_pITextFormatContent);
	


	HANDLE hThread = OpenThread(DELETE, FALSE, this->iseThreadID);
	if (hThread)
	{
		TerminateThread(hThread, 0);
		CloseHandle(hThread);
	}

}

LRESULT MetroWindow::OnCreate(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	HICON hIcon = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_ICON_IBURN));
	SetIcon(hIcon,FALSE);
	ModifyStyle(WS_CAPTION, 0, SWP_FRAMECHANGED);
	::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);
	::SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, ::GetWindowLongPtr(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(m_hWnd, 0, 255, LWA_ALPHA/*LWA_COLORKEY*/);
	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	ChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);
	ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT logFont = { 0 };
	GetObject(hFont, sizeof(logFont), &logFont);
	DeleteObject(hFont);
	hFont = NULL;
    logFont.lfHeight = 19;
	logFont.lfWeight = FW_NORMAL;
	wcscpy_s(logFont.lfFaceName, L"Segoe UI");
	hFont = CreateFontIndirect(&logFont);

	InitCommonControls();
	//HWND hCob;
	//HWND hEdit;
	DWORD dwCobExStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY;
	DWORD dwCobStyle = WS_CHILDWINDOW | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_SORT | CBS_HASSTRINGS;
	DWORD dwEditExSt = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE;
	DWORD dwEditSt = WS_CHILDWINDOW | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL;

	DWORD dwpgExSt = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY;
	DWORD dwpgSt = WS_CHILDWINDOW | WS_CLIPSIBLINGS | WS_VISIBLE;

	//hCob = CreateWindowEx(dwCobExStyle, WC_COMBOBOX, L"Driver", dwCobStyle, 120, 60, 560, 24, m_hWnd, HMENU(IDC_COMBOX_DEVICE), _Module.m_hInst, NULL);
	RECT cbRect = { 120, 60, 680, 84 };
	RECT ceRect = {120,92,561,122};
	m_combox.Create(m_hWnd, cbRect, nullptr, dwCobStyle, dwCobExStyle, IDC_COMBOX_DEVICE, nullptr);
	//hEdit = CreateWindowEx(dwEditExSt, WC_EDIT, L"", dwEditSt, 120, 95, 441, 27, m_hWnd, HMENU(IDC_EDIT_IMAGE), _Module.m_hInst, NULL);
	m_edit.Create(m_hWnd, ceRect, nullptr, dwEditSt, dwEditExSt, IDC_EDIT_IMAGE, nullptr);
	RECT rect = {320,290,680,310};
	m_proge.Create(m_hWnd, rect, L"Progress Times", dwpgSt, dwpgExSt, HMENU(IDC_PROCESS_TIME), NULL);
	m_combox.SendMessage(WM_SETFONT, (WPARAM)hFont, lParam);
	m_edit.SendMessage( WM_SETFONT, (WPARAM)hFont, lParam);
	UINT usbnub = TraversalEquipment();
	if (usbnub > 0)
	{
		for (UINT i = 0; i < usbnub; i++){
			m_combox.SendMessage(CB_ADDSTRING, 0, (LPARAM) (DrivesList[i].SizeInfo));
		}
	}
	Initialize();
	//
	HANDLE hThread = CreateThread(NULL, 0, InspectionEnvironmentThread, m_hWnd, 0, &iseThreadID);
	CloseHandle(hThread);

	return S_OK;
}

LRESULT MetroWindow::OnPaint(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	//Invalidate(FALSE);
	PAINTSTRUCT ps;
	::BeginPaint(m_hWnd, &ps);
	//HDC dcBuffer = CreateCompatibleDC(hdc);
	OnRender();
	::EndPaint(m_hWnd, &ps);
	//ValidateRect(NULL);
	return 0;
}

LRESULT MetroWindow::OnErasebkgnd(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	return TRUE;
}

LRESULT MetroWindow::OnSize(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	GetClientRect(&m_rect);
	m_rexit.place.left = m_rect.right - 45;
	m_rexit.place.top = m_rect.top;
	m_rexit.place.bottom = m_rect.top + 24;
	m_rexit.place.right = m_rect.right-1;
	m_rmin.place.left = m_rect.right - 90;
	m_rmin.place.top = m_rect.top;
	m_rmin.place.bottom = m_rect.top + 24;
	m_rmin.place.right = m_rect.right - 45;

	xArea.left = 20;
	xArea.right = m_rect.right - 20;
	xArea.top = 50;
	xArea.bottom = xArea.top + 180;


	m_mbFind.place.left = xArea.right - 130;
	m_mbFind.place.top = xArea.bottom - 135;
	m_mbFind.place.bottom = xArea.bottom - 108;
	m_mbFind.place.right = xArea.right -20;

	m_FixBoot.place.left = m_rect.left + 500;
	m_FixBoot.place.top = m_rect.top+380;
	m_FixBoot.place.bottom = m_rect.bottom - 40;
	m_FixBoot.place.right = m_rect.right - 40;


	m_Operate.place.left = m_rect.left + 320;
	m_Operate.place.top = m_rect.top + 380;
	m_Operate.place.bottom = m_rect.bottom - 40;
	m_Operate.place.right = m_rect.right - 230;

	UINT width = LOWORD(lParam);
	UINT height = HIWORD(lParam);
	this->OnResize(width, height);
	return 0;
}
LRESULT MetroWindow::OnMouseLeave(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	SendMessage(WM_PAINT, 0, 0);
	return 0;
}
LRESULT MetroWindow::OnMouseMovie(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = m_hWnd;
	tme.dwFlags = TME_LEAVE | TME_HOVER;
	tme.dwHoverTime = 1;
	_bMouseTrack = TrackMouseEvent(&tme);
	if (wParam == MK_LBUTTON&&pt.y <= 24 && pt.x<m_rmin.place.left)
	{
		SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
		return 0;
	}
	SendMessage(WM_PAINT, 0, 0);
	return S_OK;
}
LRESULT MetroWindow::OnLButtonUp(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	if (pt.x >= m_rexit.place.left&&pt.x <= m_rexit.place.right&&pt.y >= m_rexit.place.top&&pt.y <= m_rexit.place.bottom)
	{
		SendMessage(WM_CLOSE, 0, 0);
		return 0;
	}
	if (pt.x >= m_rmin.place.left&&pt.x <= m_rmin.place.right&&pt.y >= m_rmin.place.top&&pt.y <= m_rmin.place.bottom)
	{
		ShowWindow(SW_MINIMIZE);
		return 0;
	}
	if (pt.x >= m_mbFind.place.left&&pt.x <= m_mbFind.place.right&&pt.y >= m_mbFind.place.top&&pt.y <= m_mbFind.place.bottom)
	{
		//MessageBox(L"View", L"ViewOpt", MB_OK);
		if (IsInvalid)
			return 0;
		this->FindISOImage();
		return 0;
	}
	if (pt.x >= m_FixBoot.place.left&&pt.x <= m_FixBoot.place.right&&pt.y >= m_FixBoot.place.top&&pt.y <= m_FixBoot.place.bottom)
	{
		if (IsInvalid)
			return 0;
		return this->OnFixBootDrive();
	}
	if (pt.x >= m_Operate.place.left&&pt.x <= m_Operate.place.right&&pt.y >= m_Operate.place.top&&pt.y <= m_Operate.place.bottom)
	{
		if (IsInvalid)
			return 0;
		return this->OnDecompress();
	}
	return 0;
}
LRESULT MetroWindow::OnLButtonDown(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{

	return 0;
}
LRESULT MetroWindow::OnRButtonUp(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	POINT pt;
	POINT ptc;
	GetCursorPos(&ptc);
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	//RECT rect=
	if (pt.x >= m_rexit.place.left&&pt.x <= m_rexit.place.right&&pt.y >= m_rexit.place.top&&pt.y <= m_rexit.place.bottom)
	{
		return 0;
	}
	if (pt.x >= m_rmin.place.left&&pt.x <= m_rmin.place.right&&pt.y >= m_rmin.place.top&&pt.y <= m_rmin.place.bottom)
	{
		return 0;
	}
	if (pt.y <= 24)
	{
		HMENU hMenuO = LoadMenu(_Module.m_hInst, MAKEINTRESOURCE(IDR_SYS_MENU));
		HMENU hMenu = GetSubMenu(hMenuO, 0);
		
		::TrackPopupMenuEx(hMenu, TPM_RIGHTBUTTON, ptc.x, ptc.y, m_hWnd, nullptr);
	}
	return 0;
}


LRESULT MetroWindow::OnDeviceChange(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	::SendMessage(::GetDlgItem(m_hWnd, IDC_COMBOX_DEVICE), CB_RESETCONTENT, 0, 0L);
	//This message is sent by an application to remove all items from the list box and edit control of a combo box.
	UINT umb = TraversalEquipment();
	if (umb>0)
	{
		for (UINT i = 0; i<umb; i++)
		{
			::SendMessage(::GetDlgItem(m_hWnd, IDC_COMBOX_DEVICE), CB_ADDSTRING, 0, (LPARAM)(DrivesList[i].SizeInfo));
		}
	}
	return 0;
}

LRESULT MetroWindow::OnDropfiles(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	WCHAR iPath[MAX_UNC_PATH] = { 0 };
	UINT nFileCnt = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, NULL, 0);
	if (DragQueryFile((HDROP)wParam, nFileCnt - 1, iPath, MAX_UNC_PATH))
	{
		std::wstring wstr = iPath;
		std::wstring::size_type pos = wstr.rfind(L".iso");
		if (pos != wstr.length() - 4)
			return 1;
		::SetWindowText(GetDlgItem(IDC_EDIT_IMAGE), wstr.c_str());
		ProcessInfo = L"Manager Task Rate:";
		JobStatusRate = L"Task not start";
		MTNotices = L"Notices> Enviroment Inspection:\n" + envinfo;
		m_proge.SetPos(0);
		HANDLE hFile;
		LARGE_INTEGER FileSize;
		hFile = CreateFile(wstr.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			GetFileSizeEx(hFile, &FileSize);
			swprintf_s(SizeStr, L" %lld Bytes ||%4.1f KB ||%4.2f MB ||%4.2f GB\0", FileSize.QuadPart, (float)FileSize.QuadPart / 1024, (float)FileSize.QuadPart / (1024 * 1024), (float)FileSize.QuadPart / (1024 * 1024 * 1024));
			SendMessage(WM_PAINT, 0, 0);
		}
		CloseHandle(hFile);
		//::MessageBox(m_hWnd, iPath, L"Drag the image file is:", MB_OK | MB_ICONASTERISK);
	}
	return 0;
}
#pragma warning(disable:4244)
LRESULT MetroWindow::OnMultiThreadProcess(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	UINT bs = wParam;
	switch (bs)
	{
	case MET_INSPECTIONEC:
		MTNotices = L"Notices> Enviroment Inspection:\n";
		envinfo = (WCHAR*)lParam;
		MTNotices += (WCHAR*)lParam;
		break;
	case MET_DECOMPRESS:
		MTNotices = L"Notices> Decompress Status:\n";
		MTNotices += (WCHAR*)lParam;
		break;
	case MET_RESOLVEBS:
		MTNotices = L"Notices> Resolve Boot:\n";
		MTNotices += (WCHAR*)lParam;
		break;
	case MET_POST_PID_ADD:
		pidkill.Pid = (DWORD)lParam;
		pidkill.status = true;
		break;
	case MET_POST_PID_CLEAR:
		pidkill.Pid = 0;
		pidkill.status = false;
		break;
	default:
		MTNotices = L"Notices> Other Notices:";
		MTNotices += (WCHAR*)lParam;
		break;
	}	
	SendMessage(WM_PAINT, 0, 0);
	return 0;
}

LRESULT MetroWindow::OnDecompressOrFixBootRate(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bool lock = true;
	switch (wParam)
	{
	case METRO_RATE_FORMAT:
		{

		if (lParam == METRO_RATE_FORMAT_SUC)
		{
			m_proge.SetState(PBST_NORMAL);
			m_proge.SetPos(100);
			JobStatusRate = L"Decomperss <1.>Format Success.";
		}if (lParam == METRO_RATE_FORMAT_ERR){
			m_proge.SetPos(75);
			m_proge.SetState(PBST_ERROR);
			JobStatusRate = L"Decomperss <1.>Format Failure.";
			lock = false;
		}
		}
		break;
	case METRO_RATE_STATUS:
		m_proge.SetState(PBST_NORMAL);
		this->JobStatusRate =L"Decompress Rate: " +std::to_wstring(lParam)+L"%";
		m_proge.SetPos(lParam);
		break;
	case METRO_RATE_BLOCK:
		this->JobStatusRate = L"Decompress Rate: " + std::to_wstring(lParam) + L"%";
		m_proge.SetPos(lParam);
		m_proge.SetState(PBST_PAUSED);
		break;
	case METRO_RATE_ADNORMAL:
		this->JobStatusRate = L"Decompress Rate: " + std::to_wstring(lParam) + L"%";
		m_proge.SetPos(lParam+1);
		m_proge.SetState(PBST_ERROR);
		this->JobStatusRate += L" <Error Abend>";
		lock = false;
		break;
	case METRO_RATE_ENDOK:
		m_proge.SetState(PBST_NORMAL);
		m_proge.SetPos(100);
		this->JobStatusRate = L"Decompress Rate: The end of operation";
		//lock = false;
		break;
	case METRO_FIX_STATUS:
		m_proge.SetState(PBST_NORMAL);
		this->JobStatusRate = L"Resolve Boot Rate: " + std::to_wstring(lParam) + L"%";
		m_proge.SetPos(lParam);
		break;
	case METRO_FIX_UNLOCK:
		{
		if (lParam == METRO_RATE_UNLOCK_OK){
			this->JobStatusRate = L"Resolve Boot Rate: The end Of Fix Boot";
			m_proge.SetState(PBST_NORMAL);
			m_proge.SetPos(100);
		}
		if (lParam == METRO_RATE_UNLOCK_ERR)
		{
			this->JobStatusRate = L"Resolve Boot Rate: Abend Of Fix Boot";
			//m_proge.SetBarColor(0xFFD700);
			m_proge.SetPos(70);
			m_proge.SetState(PBST_ERROR);
			
		}
		lock = false;
		}
		break;
	case METRO_CANCLE_OPT:
		{
		if (lParam == METRO_CANCLE_OPT_FORMAT)
		{
			this->JobStatusRate = L"The format operation is canceled";
			//m_proge.SetBarColor(0xFFD700);
			m_proge.SetPos(10);
			m_proge.SetState(PBST_ERROR);
		}
		if (lParam == METRO_CANCLE_DECOMPRESS)
		{
			this->JobStatusRate = L"Decompression process is canceled";
			//m_proge.SetBarColor(0xFFD700);
			m_proge.SetPos(10);
			m_proge.SetState(PBST_ERROR);
		}
		if (lParam == METRO_CANCLE_FIXBOOT)
		{
			this->JobStatusRate = L"Fix boot operation is canceled";
			//m_proge.SetBarColor(0xFFD700);
			m_proge.SetPos(10);
			m_proge.SetState(PBST_ERROR);
		}
		IsInvalid = false;
		}
		break;
	default:
		break;
	}
	SendMessage(WM_PAINT, 0, 0);
	this->IsInvalid = lock;
	return 0;
}
#pragma warning(default:4244)

LRESULT MetroWindow::OnKillFocus(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	SendMessage(WM_PAINT, 0, 0);
	return 0;
}

LRESULT MetroWindow::OnMinSize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ShowWindow(SW_MINIMIZE);
	return 0;
}

LRESULT MetroWindow::OnSupport(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ShellExecute(m_hWnd, L"open",L"https://github.com/forcegroup",NULL,NULL,SW_MAXIMIZE);
	return 0;
}

LRESULT MetroWindow::OnMeClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendMessage(WM_CLOSE, 0, 0);
	return 0;
}

LRESULT MetroWindow::OnAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int	nButton = 0;
	int nRadioButton = 0;
	CreateTaskDialogIndirectFd(m_hWnd, GetModuleHandle(nullptr), &nButton, &nRadioButton);
	return 0;
}

LRESULT MetroWindow::OnClose(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	if (IsInvalid)
	{
		int nButton;
		TaskDialog(m_hWnd, NULL,
			L"Operation is in progress", L"Operation Warning!",
			L"Are you sure you want to force the end of the operation?",
			TDCBF_YES_BUTTON | TDCBF_NO_BUTTON,
			TD_WARNING_ICON, &nButton);
		if (nButton ==IDNO)
		{	

			return 1;
		}
		if (pidkill.status)
		{
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pidkill.Pid);
			if (hProcess)
			{
				TerminateProcess(hProcess, 13);
				CloseHandle(hProcess);
			}
		}
		dwExit = 12;
	}
	::DestroyWindow(m_hWnd);
	return 0;
}

LRESULT MetroWindow::OnDestory(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	PostQuitMessage(dwExit);
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////
////DirectUI Style MsgFun
LRESULT MetroWindow::OnDecompress()
{
	IsInvalid = true;
	m_proge.SetState(PBST_NORMAL);
	m_proge.SetPos(0);
	WCHAR szImage[MAX_UNC_PATH] = { 0 };
	int nButton;
	if (::GetWindowTextLength(::GetDlgItem(m_hWnd, IDC_EDIT_IMAGE)) < 8)
	{
		TaskDialog(m_hWnd, _Module.GetModuleInstance(),
			L"iBurnMgr Image Error", L"Image PATH Error!",
			L"No one really effective path!",
			TDCBF_OK_BUTTON,
			TD_ERROR_ICON, &nButton);
		IsInvalid = false;
		return 1;
	}
	::GetWindowText(GetDlgItem(IDC_EDIT_IMAGE), szImage, MAX_UNC_PATH);
	errno_t err = _waccess_s(szImage, 4);
	switch (err)
	{
	case 0:
		break;
	case EACCES:
		TaskDialog(m_hWnd, _Module.GetModuleInstance(),
			L"iBurnMgr Image Error", L"Image No read permission",
			szImage,
			TDCBF_OK_BUTTON,
			TD_ERROR_ICON, &nButton);
		IsInvalid = false;
		return EACCES;
	case ENOENT:
		TaskDialog(m_hWnd, _Module.GetModuleInstance(),
			L"iBurnMgr Image Error", L"The image file does not exist",
			szImage,
			TDCBF_OK_BUTTON,
			TD_ERROR_ICON, &nButton);
		IsInvalid = false;
		return ENOENT;
	case EINVAL:
		TaskDialog(m_hWnd, _Module.GetModuleInstance(),
			L"iBurnMgr Image Error", L"Invalid parameter",
			szImage,
			TDCBF_OK_BUTTON,
			TD_ERROR_ICON, &nButton);
		IsInvalid = false;
		return EINVAL;
	default:
		TaskDialog(m_hWnd, _Module.GetModuleInstance(),
			L"iBurnMgr Image Error", L"Unknown errorr",
			szImage,
			TDCBF_OK_BUTTON,
			TD_ERROR_ICON, &nButton);
		IsInvalid = false;
		return 3;
	}
	SpData.image = szImage;
	LRESULT i = SendMessage(GetDlgItem(IDC_COMBOX_DEVICE), CB_GETCURSEL, 0, 0L);
	if (0> i || i >= 26)
	{
		TaskDialog(m_hWnd, _Module.GetModuleInstance(),
			L"iBurnMgr USB Error", L"USB Error",
			L"No Select Device!",
			TDCBF_OK_BUTTON,
			TD_ERROR_ICON, &nButton);
		IsInvalid = false;
		return 1;
	}
	wcscpy_s(SpData.latter, 9, DrivesList[i].driveLetter);
	SpData.lParam = m_hWnd;
	ProcessInfo = L"Making Rate:";
	JobStatusRate = L"Task is started";
	DWORD dwThreadID;
	HANDLE hThread = CreateThread(NULL, 0, Decompress::DecompressSupervisorThread,&SpData, 0, &dwThreadID);
	CloseHandle(hThread);
	return 0;
}
LRESULT MetroWindow::OnFixBootDrive()
{
	IsInvalid = true;
	m_proge.SetState(PBST_NORMAL);
	m_proge.SetPos(0);
	LRESULT i = SendMessage(GetDlgItem(IDC_COMBOX_DEVICE), CB_GETCURSEL, 0, 0L);
	if (0> i || i >= 26)
	{
		int nButton;
		TaskDialog(m_hWnd, _Module.GetModuleInstance(),
			L"iBurnMgr USB Error", L"USB Error",
			L"No Select Device!",
			TDCBF_OK_BUTTON,
			TD_ERROR_ICON, &nButton);
		IsInvalid = false;
		return 1;
	}
	reData.latter = DrivesList[i].driveLetter;
	reData.m_hWnd = static_cast<HWND>(m_hWnd);
	ProcessInfo = L"Resolve Boot Rate:";
	JobStatusRate = L"Fix Boot is started";
	DWORD dwThreadID;
	HANDLE hThread = CreateThread(NULL, 0, ResolveSupervisor::ResolveSupervisorThread, &reData, 0, &dwThreadID);
	CloseHandle(hThread);
	return 0;
}

/////
void MetroWindow::FindISOImage()
{
	wchar_t szImageFile[MAX_UNC_PATH] = { 0 };
	if (OpenImageFile(m_hWnd, szImageFile) == S_OK)
	{
		::SetWindowText(GetDlgItem(IDC_EDIT_IMAGE), szImageFile);
		ProcessInfo = L"Manager Task Rate:";
		JobStatusRate = L"Task not start";
		MTNotices = L"Notices> Enviroment Inspection:\n"+envinfo;
		m_proge.SetPos(0);
		HANDLE hFile;
		LARGE_INTEGER FileSize;
		hFile = CreateFile(szImageFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			GetFileSizeEx(hFile, &FileSize);
			swprintf_s(SizeStr, L" %lld Bytes ||%4.1f KB ||%4.2f MB ||%4.2f GB\0", FileSize.QuadPart, (float)FileSize.QuadPart / 1024, (float)FileSize.QuadPart / (1024 * 1024), (float)FileSize.QuadPart / (1024 * 1024 * 1024));
			SendMessage(WM_PAINT, 0, 0);
		}
		CloseHandle(hFile);
		//::MessageBox(m_hWnd, szImageFile, L"Select the image file is:", MB_OK | MB_ICONASTERISK);
	}
}


/////D2D
HRESULT MetroWindow::Initialize()
{
	HRESULT hr;
	hr = CreateDeviceIndependentResources();
	FLOAT dpiX, dpiY;
	m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);
	
	return hr;
}

HRESULT MetroWindow::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

	return hr;
}

HRESULT MetroWindow::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRenderTarget)
	{
		RECT rc;
		::GetClientRect(m_hWnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
			);

		hr = m_pDirect2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hWnd, size),
			&m_pRenderTarget
			);
		if (SUCCEEDED(hr)){
		hr = CoCreateInstance(
				CLSID_WICImagingFactory1,
				NULL,
				CLSCTX_INPROC_SERVER,
				IID_IWICImagingFactory,
				reinterpret_cast<void **>(&m_pWICFactory)
				);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(m_pRenderTarget, m_pWICFactory,
				MAKEINTRESOURCE(IDP_EMOJI_LOG),
				L"PNG",
				0, 0, &m_pBitmap);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(0x3867B2),
				&m_pMinButtonActiveBrush
				);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Gainsboro),
				&m_TitleClinetBrush
				);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Firebrick),
				&m_pCloseButtonClickBrush
				);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Silver),
				&m_pUITextBrush
				);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::White),
				&m_pLightWhiteBrush
				);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::DarkOrange),
				&m_pMetroButtonNsBrush
				);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Orange),
				&m_pMetroButtonLsBrush
				);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black),
				&m_pControlTextBrush
				);
		}

	}

	return hr;
}

void MetroWindow::DiscardDeviceResources()
{
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_TitleClinetBrush);
	SafeRelease(&m_pMinButtonActiveBrush);
	SafeRelease(&m_pMetroButtonNsBrush);
	SafeRelease(&m_pMetroButtonLsBrush);
	SafeRelease(&m_pLightWhiteBrush);
	SafeRelease(&m_pUITextBrush);
	SafeRelease(&m_pCloseButtonClickBrush);
	SafeRelease(&m_pControlTextBrush);
}
HRESULT MetroWindow::OnRender()
{
	HRESULT hr = S_OK;
	//const wchar_t* szFont[2] = { L"Segeo UI", L"Microsoft Yahei"};
	
	hr = CreateDeviceResources();
	//IDWriteTextLayout 
	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_pIDWriteFactory));
	m_pIDWriteFactory->CreateTextFormat(
		normalFont.c_str(),
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		13.0f * 96.0f / 72.0f,
		localeinfo.localename.c_str(),
		&m_pITextFormatTitle
		);
	 //m_pITextFormatContent
	m_pIDWriteFactory->CreateTextFormat(
		normalFont.c_str(),
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		12.0f * 96.0f / 72.0f,
		localeinfo.localename.c_str(),
		&m_pITextFormatContent
		);
	if (SUCCEEDED(hr))
	{
		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Purple));
		////Draw Exit area
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		/////// Area Draw
		

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

		int width = static_cast<int>(rtSize.width);
		int height = static_cast<int>(rtSize.height);

#pragma warning(disable:4244)
#pragma warning(disable:4267)
		m_pRenderTarget->FillRectangle(D2D1::RectF(0, 0, rtSize.width, 36),m_TitleClinetBrush);
		D2D1_SIZE_F size = m_pBitmap->GetSize();
		D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(0.f, 0.f);

		m_pRenderTarget->DrawBitmap(
			m_pBitmap,
			D2D1::RectF(
			upperLeftCorner.x,
			upperLeftCorner.y,
			upperLeftCorner.x + size.width,
			upperLeftCorner.y + size.height)
			);

		//// Text Draw
		m_pRenderTarget->DrawTextW(windowTitle.c_str(), windowTitle.length(), m_pITextFormatTitle, D2D1::RectF(35, 5, 600, 25), m_pControlTextBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);

		//m_pITextFormatContent->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		m_pRenderTarget->DrawTextW(USBdrive.c_str(), USBdrive.size(), m_pITextFormatContent, D2D1::RectF(30, 60, 110, 85), m_pUITextBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		m_pRenderTarget->DrawTextW(ImageFile.c_str(), ImageFile.size(), m_pITextFormatContent, D2D1::RectF(30, 95, 110, 122), m_pUITextBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		m_pRenderTarget->DrawTextW(ImageSize.c_str(), ImageSize.size(), m_pITextFormatContent, D2D1::RectF(30, 130, 110, 157), m_pUITextBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		m_pRenderTarget->DrawTextW(Description.c_str(), Description.size(), m_pITextFormatContent, D2D1::RectF(30, 167, 110, 221), m_pUITextBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		m_pRenderTarget->DrawTextW(copyright.c_str(), copyright.size(), m_pITextFormatContent, D2D1::RectF(30, m_Operate.place.top, 400, m_Operate.place.bottom), m_pUITextBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		m_pITextFormatContent->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
		m_pRenderTarget->DrawTextW(ProcessInfo.c_str(), ProcessInfo.length(), m_pITextFormatContent, D2D1::RectF(320.0f, xArea.bottom + 30.0f, xArea.right, xArea.bottom + 200.0f), m_pUITextBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		m_pRenderTarget->DrawTextW(JobStatusRate.c_str(), JobStatusRate.length(), m_pITextFormatContent, D2D1::RectF(320.0f, xArea.bottom + 90.0f, xArea.right, xArea.bottom + 260.0f), m_pUITextBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		m_pRenderTarget->DrawTextW(MTNotices.c_str(), MTNotices.length(), m_pITextFormatContent, D2D1::RectF(xArea.left + 10.f, xArea.bottom + 20.0f, xArea.right / 2 - 60.f, xArea.bottom + 200.0f), m_pUITextBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		m_pRenderTarget->DrawTextW(Notes.c_str(), Notes.size(), m_pITextFormatContent, D2D1::RectF(120, 167, 690, 221), m_pUITextBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		m_pRenderTarget->DrawTextW(SizeStr, wcslen(SizeStr), m_pITextFormatContent, D2D1::RectF(120, 130, 680, 157), m_pUITextBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		/////////////////
		/// Button Draw
		m_pITextFormatContent->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		m_pITextFormatTitle->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		//Exit
		if (pt.x >= m_rexit.place.left&&pt.x <= m_rexit.place.right&&pt.y >= m_rexit.place.top&&pt.y <= m_rexit.place.bottom){
			m_pRenderTarget->FillRectangle(D2D1::RectF(m_rexit.place.left, m_rexit.place.top, m_rexit.place.right + 1, m_rexit.place.bottom), m_pCloseButtonClickBrush);
			m_pRenderTarget->DrawLine(D2D1::Point2F(m_rexit.place.left + 17.5f, m_rexit.place.top + 6.5f), D2D1::Point2F(m_rexit.place.right - 17.5f, m_rexit.place.bottom - 6.5f), m_pLightWhiteBrush, 1.0f);
			m_pRenderTarget->DrawLine(D2D1::Point2F(m_rexit.place.left + 17.5f, m_rexit.place.bottom - 6.5f), D2D1::Point2F(m_rexit.place.right - 17.5f, m_rexit.place.top + 6.5f), m_pLightWhiteBrush, 1.0f);
		}
		else{
			m_pRenderTarget->DrawLine(D2D1::Point2F(m_rexit.place.left + 17.5f, m_rexit.place.top + 6.5f), D2D1::Point2F(m_rexit.place.right - 17.5f, m_rexit.place.bottom - 6.5f), m_pControlTextBrush, 1.0f);
			m_pRenderTarget->DrawLine(D2D1::Point2F(m_rexit.place.left + 17.5f, m_rexit.place.bottom - 6.5f), D2D1::Point2F(m_rexit.place.right - 17.5f, m_rexit.place.top + 6.5f), m_pControlTextBrush, 1.0f);
		}
		//Min
		if (pt.x >= m_rmin.place.left&&pt.x < m_rmin.place.right&&pt.y >= m_rmin.place.top&&pt.y <= m_rmin.place.bottom)
		{
			m_pRenderTarget->FillRectangle(D2D1::RectF(m_rmin.place.left, m_rmin.place.top, m_rmin.place.right, m_rmin.place.bottom), m_pMinButtonActiveBrush);
			m_pRenderTarget->DrawLine(D2D1::Point2F(m_rmin.place.left + 17, m_rmin.place.top + 15), D2D1::Point2F(m_rmin.place.right - 17, m_rmin.place.top + 15), m_pLightWhiteBrush, 1.2f);
		}
		else{
			m_pRenderTarget->DrawLine(D2D1::Point2F(m_rmin.place.left + 17, m_rmin.place.top + 15), D2D1::Point2F(m_rmin.place.right - 17, m_rmin.place.top + 15), m_pControlTextBrush, 1.2f);
		}
		//find
		if (pt.x >= m_mbFind.place.left&&pt.x <= m_mbFind.place.right&&pt.y >= m_mbFind.place.top&&pt.y <= m_mbFind.place.bottom)
		{
			m_pRenderTarget->FillRectangle(D2D1::RectF(m_mbFind.place.left, m_mbFind.place.top, m_mbFind.place.right, m_mbFind.place.bottom), m_pMetroButtonLsBrush);
		}
		else{
			m_pRenderTarget->FillRectangle(D2D1::RectF(m_mbFind.place.left, m_mbFind.place.top, m_mbFind.place.right, m_mbFind.place.bottom), m_pMetroButtonNsBrush);
		}
		m_pRenderTarget->DrawTextW(m_mbFind.caption.c_str(), m_mbFind.caption.length(), m_pITextFormatContent, D2D1::RectF(m_mbFind.place.left, m_mbFind.place.top, m_mbFind.place.right, m_mbFind.place.bottom), m_pControlTextBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		//FixBoot
		if (pt.x >= m_FixBoot.place.left&&pt.x <= m_FixBoot.place.right&&pt.y >= m_FixBoot.place.top&&pt.y <= m_FixBoot.place.bottom)
		{
			m_pRenderTarget->FillRectangle(D2D1::RectF(m_FixBoot.place.left, m_FixBoot.place.top, m_FixBoot.place.right, m_FixBoot.place.bottom), m_pMetroButtonLsBrush);
		}
		else{
			m_pRenderTarget->FillRectangle(D2D1::RectF(m_FixBoot.place.left, m_FixBoot.place.top, m_FixBoot.place.right, m_FixBoot.place.bottom), m_pMetroButtonNsBrush);
		}
		m_pRenderTarget->DrawTextW(m_FixBoot.caption.c_str(), m_FixBoot.caption.length(), m_pITextFormatContent, D2D1::RectF(m_FixBoot.place.left, m_FixBoot.place.top, m_FixBoot.place.right, m_FixBoot.place.bottom), m_pControlTextBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		//Operate
		if (pt.x >= m_Operate.place.left&&pt.x <= m_Operate.place.right&&pt.y >= m_Operate.place.top&&pt.y <= m_Operate.place.bottom)
		{
			m_pRenderTarget->FillRectangle(D2D1::RectF(m_Operate.place.left, m_Operate.place.top, m_Operate.place.right, m_Operate.place.bottom), m_pMetroButtonLsBrush);
		}
		else{
			m_pRenderTarget->FillRectangle(D2D1::RectF(m_Operate.place.left, m_Operate.place.top, m_Operate.place.right, m_Operate.place.bottom), m_pMetroButtonNsBrush);
		}
		m_pRenderTarget->DrawTextW(m_Operate.caption.c_str(), m_Operate.caption.length(), m_pITextFormatContent, D2D1::RectF(m_Operate.place.left, m_Operate.place.top, m_Operate.place.right, m_Operate.place.bottom), m_pControlTextBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		
		////
#pragma warning(default:4244)
#pragma warning(default:4267)	
		m_pRenderTarget->DrawRectangle(D2D1::RectF(0.f, 0.f, rtSize.width, rtSize.height), m_pControlTextBrush, 1.0f, NULL);

		hr = m_pRenderTarget->EndDraw();
	}

	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardDeviceResources();
	}

	return hr;
}

void MetroWindow::OnResize(UINT width, UINT height)
{
	if (m_pRenderTarget)
	{
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}

ENDNAMESPACE
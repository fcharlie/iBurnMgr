#ifndef METROWINDOW_H
#define METROWINDOW_H
#include <string>
#include <d2d1.h>
#include <dwrite.h>
#include "resource.h"
#include "APIController.h"
#include "MUIController.h"
#include <wincodec.h>
//#define NAMESPACEMETRO namespace Metro{
//#define ENDNAMESPACE }

typedef CWinTraits<WS_OVERLAPPED | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE> CMetroWindowTraits;

LRESULT WINAPI CreateTaskDialogIndirectFd(
	__in HWND		hwndParent,
	__in HINSTANCE	hInstance,
	__out_opt int *	pnButton,
	__out_opt int *	pnRadioButton
	);
extern Metro::MUI::MUIController* muiController;

namespace Metro{

	typedef struct _MetroPushButton{
		RECT place;
		bool bStatus;
		std::wstring caption;
		_MetroPushButton(RECT rect, std::wstring itext, bool bstatus = false) :place(rect), caption(itext), bStatus(bstatus){}
		_MetroPushButton() :place({0,0,0,0}),caption(L"Default"),bStatus(false) {}
	}MetroPushButton;
	typedef struct _MetroLabel{
		RECT place;
		std::wstring text;
		_MetroLabel(RECT rect, std::wstring itext) :place(rect), text(itext){}
		_MetroLabel() :place({0,0,0,0}), text(L"MetroLabel"){}
	}MetroLabel;

	class MetroWindow :public CWindowImpl<MetroWindow, CWindow, CMetroWindowTraits>{
	private:
		std::wstring localename;
		DWORD FontTabel;
		ID2D1Factory* m_pDirect2dFactory;
		ID2D1HwndRenderTarget* m_pRenderTarget;
		ID2D1SolidColorBrush* m_TitleClinetBrush;
		ID2D1SolidColorBrush* m_pMinButtonActiveBrush;
		ID2D1SolidColorBrush* m_pMetroButtonNsBrush;
		ID2D1SolidColorBrush* m_pMetroButtonLsBrush;
		ID2D1SolidColorBrush* m_pUITextBrush;
		ID2D1SolidColorBrush* m_pCloseButtonClickBrush;
		ID2D1SolidColorBrush* m_pLightWhiteBrush;
		ID2D1SolidColorBrush* m_pControlTextBrush;

		IWICImagingFactory*		m_pWICFactory;
		ID2D1Bitmap* m_pBitmap;
		IDWriteTextFormat* m_pITextFormatTitle;
		IDWriteTextFormat* m_pITextFormatContent;
		IDWriteFactory* m_pIDWriteFactory;
		IDWriteTypography*    m_pDWriteTypography;
		HRESULT CreateDeviceIndependentResources();
		HRESULT Initialize();
		HRESULT CreateDeviceResources();
		void DiscardDeviceResources();
		HRESULT OnRender();
		void OnResize(
			UINT width,
			UINT height
			);
	public:
		MetroWindow();
		~MetroWindow();
		DECLARE_WND_CLASS(METROCLASSNAME)
		BEGIN_MSG_MAP(MetroWindow)
			MESSAGE_HANDLER(WM_CREATE,OnCreate)
			MESSAGE_HANDLER(WM_DESTROY,OnDestory)
			MESSAGE_HANDLER(WM_CLOSE, OnClose)
			MESSAGE_HANDLER(WM_PAINT,OnPaint)
			MESSAGE_HANDLER(WM_SIZE,OnSize)
			MESSAGE_HANDLER(WM_MOUSEMOVE,OnMouseMovie)
			MESSAGE_HANDLER(WM_MOUSELEAVE,OnMouseLeave)
			MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
			MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
			MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)
			MESSAGE_HANDLER(WM_DEVICECHANGE,OnDeviceChange)
			MESSAGE_HANDLER(WM_DROPFILES,OnDropfiles)
			MESSAGE_HANDLER(WM_ERASEBKGND,OnErasebkgnd)
			////
			MESSAGE_HANDLER(METRO_MULTITHREAD_MSG,OnMultiThreadProcess)
			MESSAGE_HANDLER(METRO_THREAD_RATESTATUS_MSG, OnDecompressOrFixBootRate)
			//MESSAGE_HANDLER(WM_MOUSELEAVE,OnMouseLeave)
			MESSAGE_HANDLER(WM_KILLFOCUS,OnKillFocus)
			COMMAND_ID_HANDLER(ID_SYS_MIN, OnMinSize)
			COMMAND_ID_HANDLER(ID_SYS_SUPPORT, OnSupport)
			COMMAND_ID_HANDLER(ID_SYS_EXIT,OnMeClose)
			COMMAND_ID_HANDLER(ID_SYS_ABOUT,OnAbout)
		END_MSG_MAP()
		LRESULT OnCreate(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
		LRESULT OnDestory(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
		LRESULT OnClose(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
		LRESULT OnPaint(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
		LRESULT OnSize(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
		LRESULT OnMouseMovie(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
		LRESULT OnLButtonUp(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
		LRESULT OnLButtonDown(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
		LRESULT OnRButtonUp(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
		LRESULT OnDeviceChange(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnMouseLeave(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnDropfiles(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnErasebkgnd(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

		LRESULT OnMultiThreadProcess(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnDecompressOrFixBootRate(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		//LRESULT OnActive(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnKillFocus(UINT nMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
		//WM_COMMAND
		LRESULT OnSupport(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		LRESULT OnMinSize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		LRESULT OnMeClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		LRESULT OnAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		void FindISOImage();
		LRESULT OnDecompress();
		LRESULT OnFixBootDrive();
		DWORD GetExitCode(){ return dwExit; }
	private:
		DWORD dwExit;
		bool IsInvalid;
		BOOL _bMouseTrack;
		MetroPushButton m_rexit;
		RECT m_rect;
		MetroPushButton m_rmin;
		RECT m_NcArea;
		RECT xArea;
		RECT xStatusArea;
		MetroPushButton m_mbFind;
		MetroPushButton m_FixBoot;
		MetroPushButton m_Operate;
		MetroLabel m_mtexts;
		CProgressBarCtrl m_proge;
		CEdit m_edit;
		CComboBox m_combox;
		DWORD iseThreadID;
		std::wstring USBdrive;
		std::wstring ImageFile;
		std::wstring ImageSize;
		std::wstring Description;
		std::wstring normalFont;
		std::wstring MTNotices;
		std::wstring JobStatusRate;
		std::wstring ProcessInfo;
		std::wstring windowTitle;
		std::wstring copyright;
	};
}
#endif
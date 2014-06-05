//
# ifndef DECOMPRESSSUPERVISION
#define  DECOMPRESSSUPERVISION
#include <string>

//////////////////////////
/// 1 Format Flash Disk
/// 2 7Z Decompress ISO Image
/// 3 Check End Return Success


namespace Decompress{
	typedef struct _SupervisorData{
		std::wstring image;
		wchar_t latter[9];
		LPVOID lParam;
	}SupervisorData,*LPSupervisorData;

	DWORD WINAPI DecompressSupervisorThread(LPVOID Spdata);

	class Supervisor{
	private:
		std::wstring m_image;
		std::wstring m_latter;
		HWND hParents;
		bool FormatUSBDevice();
	public:
		bool CreateDecompressFormat();
		static bool WINAPI CreateDecompressInvoke(HWND hWnd,std::wstring img, LPWSTR latter);
		Supervisor(LPSupervisorData lPSpData);
		Supervisor(std::wstring img, LPCWSTR latter,LPVOID lParam);
	};

}
#endif
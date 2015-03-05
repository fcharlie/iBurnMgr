#ifndef METROMUI_CONTROLLER
#define METROMUI_CONTROLLER

#define DISABLE_Now 1

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
			static std::auto_ptr<MUIController> m_pInstance;
		protected:
			MUIController();
		private:
			DWORD UILcId;
			std::wstring ifilename;

		public:
			static MUIController* Instance();
			std::map<std::wstring, std::wstring> m_langTree;
			std::wstring atString(std::wstring key,std::wstring value);
			bool MUIResourceLoader();
			DWORD Init();
		};
	}
}



#endif
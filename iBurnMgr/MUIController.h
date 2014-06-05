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
		typedef struct _LocaleHlp{
			std::wstring localename;
			DWORD lcid;
		}LocaleHlp;
		class MUIController{
			static std::auto_ptr<MUIController> m_pInstance;
		protected:
			MUIController();
		private:
			void GetLines();
			DWORD UILcId;
			std::wstring ifilename;
			int lines;
			std::wifstream langfile;
			std::vector<std::wstring> m_line;
		public:
			static MUIController* Instance();
			std::map<std::wstring, std::wstring> m_langmap;
			std::wstring QueryValueString(std::wstring key);
			DWORD ApplyChangeValue(std::wstring key, std::wstring value);
			bool MUIInitLangFileFromatParse();
			DWORD Init();
		};
	}
}



#endif
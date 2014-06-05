#include "Precompiled.h"
#include "UniversalDiskFormatModel.h"
#include <imapi2fs.h>

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

///Namespace begin
NAMESPACEMETRO
UNNAMESPACE

UniversalDiskFormatDecode::UniversalDiskFormatDecode(std::wstring image, std::wstring drive) :m_Image(image), m_drive(drive),
m_pIDiscFormat2Data(NULL)
{

}
////////Clear resource
UniversalDiskFormatDecode::~UniversalDiskFormatDecode()
{
	SafeRelease(&m_pIDiscFormat2Data);
}

///Namespace End
UNNAMESPACEEND


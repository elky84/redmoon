#pragma once

#include "Common.h"
#include "FunctionPtr.h"
#include "SmartPtr.h"

namespace Redmoon
{

class CPopup
{
	typedef std::map<WORD, CSmartPtr<CCallBack> > STL_MAP_CALLBACK;

public:
	CPopup(HWND& hWnd);
	~CPopup();

	bool Add(const tstring& strName, CCallBack* pcCallback);
	bool Seperate();
	bool Show();
	BOOL Command(WORD nID);

private:
	HWND& m_hWnd;
	HMENU m_hPopup;
	WORD m_nAccIdx;
	STL_MAP_CALLBACK m_stl_map_Callback;
};

} //namespace Redmoon
#include "Menu.h"

namespace Redmoon
{

CPopup::CPopup(HWND& hWnd) : m_hWnd(hWnd), m_hPopup(CreatePopupMenu()), m_nAccIdx(WM_USER + 500)
{

}

CPopup::~CPopup()
{
	DestroyMenu(m_hPopup);
}

bool CPopup::Add(const tstring& strName, CCallBack* pcCallback)
{
	if(!AppendMenu(m_hPopup, MF_STRING, m_nAccIdx, strName.c_str()))
	{
		LOG_ERROR(_T("%s Error. [%d, %s]"), __TFUNCTION__, m_nAccIdx, strName.c_str());
		return false;
	}

	m_stl_map_Callback.insert(STL_MAP_CALLBACK::value_type(m_nAccIdx, pcCallback));
	m_nAccIdx++;
	return true;
}

bool CPopup::Seperate()
{
	if(!AppendMenu(m_hPopup, MF_SEPARATOR, NULL, NULL))
	{
		LOG_ERROR(_T("%s Error."), __TFUNCTION__);
		return false;
	}
	return true;
}

bool CPopup::Show()
{
	if(!SetForegroundWindow(m_hWnd))
		return false;

	POINT pt;
	GetCursorPos(&pt);
	BOOL nID = TrackPopupMenu(m_hPopup, TPM_LEFTBUTTON | TPM_RETURNCMD, pt.x, pt.y, 0, m_hWnd, NULL);
	if(nID == 0)
		return false;

	Command((WORD)nID);
	return true;
}

BOOL CPopup::Command(WORD nID)
{
	STL_MAP_CALLBACK::iterator it = m_stl_map_Callback.find(nID);
	if(it == m_stl_map_Callback.end())
	{
		return FALSE;
	}

	CCallBack* pcCallback = it->second;
	return pcCallback->Interpret(NULL);
}

} //namespace Redmoon
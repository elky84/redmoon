// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// DBTest.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"
#include "resource.h"
#include "DBTestDlg.h"

static int GetTextLenEx(CListBox& box, LPCTSTR lpszText)
{
	CSize size;
	CDC *pDC = box.GetDC();

	// 현재 리스트박스의 폰트를 얻어와 DC에 적용시킨다.
	CFont* pOld = pDC->SelectObject(box.GetFont());

	// 스타일에 따라 약간의 오프셑 차이가 있다.
	if ((box.GetStyle() & LBS_USETABSTOPS) == 0)
	{
		size = pDC->GetTextExtent(lpszText, (int)_tcslen(lpszText));
		size.cx += 3;
	}
	else
	{
		size = pDC->GetTabbedTextExtent(lpszText, (int)_tcslen(lpszText), 0, NULL);
		size.cx += 2;
	}
	pDC->SelectObject(pOld);
	box.ReleaseDC(pDC);

	// 구한 문자열의 Pixel 단위를 넘긴다.
	return size.cx;
}

// 문자열을 리스트박스에 추가하는 함수.
static void AddStringEx(CListBox& box, CString str)
{
	// 우선 리스트박스에 문자열을 추가시킨다.
	box.InsertString(0, str);

	// 길이를 계산하여 기존 길이보다 넓으면 새로운 길이를 적용시킨다.
	int iExt = GetTextLenEx(box, str);
	if (iExt > box.GetHorizontalExtent())
		box.SetHorizontalExtent(iExt);
}

CDBTestDlg *g_pcThis = NULL;
void Alert(TCHAR *str, ...)
{
	TCHAR szString[512];
	va_list vl;
	va_start(vl, str);
	int len = _vsctprintf(str, vl);
	if(len <= 512)
	{
		_vstprintf((TCHAR*)szString, str, vl);
	}
	else
	{
		TCHAR szBuf[128];
		_stprintf(szBuf, _T("Log %d"), len);
	}
	va_end(vl);

	if(g_pcThis)
		AddStringEx(g_pcThis->m_listboxLog, szString);
}

void CopyToClipBoard(HWND hWnd, char *str)
{
	if(OpenClipboard(hWnd))
	{
		EmptyClipboard();
		HANDLE handle = INVALID_HANDLE_VALUE;

		int nStrLen = (int)strlen(str);
		int nNewLen = sizeof(TCHAR) * (nStrLen + 1);
		HGLOBAL hBlock = GlobalAlloc(GMEM_MOVEABLE, nNewLen);
		if(hBlock)
		{
			TCHAR *pszText = (TCHAR*)GlobalLock( hBlock );
			if(pszText)
			{
				memset(pszText, 0, nNewLen);
#ifdef _UNICODE
				CharToWChar(pszText, str);
#else
				strncpy(pszText, str, strlen(str));
#endif
				GlobalUnlock(hBlock);
#ifdef _UNICODE
				handle = SetClipboardData(CF_UNICODETEXT, hBlock);
#else
				handle = SetClipboardData(CF_TEXT, hBlock);
#endif
				DWORD result = GetLastError();
			}
		}

		CloseClipboard();
		// We must not free the object until CloseClipboard is called.
		if(hBlock)
		{
			GlobalFree(hBlock);
		}
	}
}

int GetCurSel(CListCtrl *listctrl)
{
	for(int i = listctrl->GetItemCount()-1; 0 <= i; i--) 
	{
		if(listctrl->GetItemState(i, LVIS_SELECTED) != 0) //해당하는 목록이 선택된 상태이면
		{
			return i;
		}
	}
	return -1;
}
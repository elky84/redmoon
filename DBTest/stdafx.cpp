// stdafx.cpp : ǥ�� ���� ���ϸ� ��� �ִ� �ҽ� �����Դϴ�.
// DBTest.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj���� �̸� �����ϵ� ���� ������ ���Ե˴ϴ�.

#include "stdafx.h"
#include "resource.h"
#include "DBTestDlg.h"

static int GetTextLenEx(CListBox& box, LPCTSTR lpszText)
{
	CSize size;
	CDC *pDC = box.GetDC();

	// ���� ����Ʈ�ڽ��� ��Ʈ�� ���� DC�� �����Ų��.
	CFont* pOld = pDC->SelectObject(box.GetFont());

	// ��Ÿ�Ͽ� ���� �ణ�� �����V ���̰� �ִ�.
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

	// ���� ���ڿ��� Pixel ������ �ѱ��.
	return size.cx;
}

// ���ڿ��� ����Ʈ�ڽ��� �߰��ϴ� �Լ�.
static void AddStringEx(CListBox& box, CString str)
{
	// �켱 ����Ʈ�ڽ��� ���ڿ��� �߰���Ų��.
	box.InsertString(0, str);

	// ���̸� ����Ͽ� ���� ���̺��� ������ ���ο� ���̸� �����Ų��.
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
		if(listctrl->GetItemState(i, LVIS_SELECTED) != 0) //�ش��ϴ� ����� ���õ� �����̸�
		{
			return i;
		}
	}
	return -1;
}
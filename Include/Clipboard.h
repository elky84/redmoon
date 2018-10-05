#pragma once

static void CopyToClipBoard(HWND hWnd, TCHAR *szStr)
{
	if(OpenClipboard(hWnd))
	{
		EmptyClipboard();
		HANDLE handle = INVALID_HANDLE_VALUE;

		size_t nStrLen = strlen(szStr);
		size_t nNewLen = sizeof(TCHAR) * (nStrLen + 1);
		HGLOBAL hBlock = GlobalAlloc(GMEM_MOVEABLE, nNewLen);
		if(hBlock)
		{
			TCHAR *pszText = (TCHAR*)GlobalLock( hBlock );
			if(pszText)
			{
				memset(pszText, 0, nNewLen);

				TCSCPY(pszText, nNewLen, szStr)

				GlobalUnlock(hBlock);
				handle = SetClipboardData(CF_UNICODETEXT, hBlock);
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
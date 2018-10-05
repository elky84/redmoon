#pragma once

#define WM_TRAYICON_MSG		WM_USER + 100

namespace Redmoon
{
	class CTray
	{
	public:
		static void Register(HWND hWnd, const tstring& strTip)
		{
			// 트레이에 아이콘 등록
			NOTIFYICONDATA  nid;
			nid.cbSize = sizeof(nid);
			nid.hWnd = hWnd; // 메인 윈도우 핸들
			nid.uID = IDR_MAINFRAME;  // 아이콘 리소스 ID
			nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; // 플래그 설정
			nid.uCallbackMessage = WM_TRAYICON_MSG; // 콜백메시지 설정
			nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); // 아이콘 로드 

			lstrcpy(nid.szTip, strTip.c_str()); 
			Shell_NotifyIcon(NIM_ADD, &nid);
			::SendMessage(hWnd, WM_SETICON, (WPARAM)TRUE, (LPARAM)nid.hIcon);

			ShowWindow(hWnd, SW_HIDE);
		}

		static void Change(HWND hWnd, const tstring& strTip)
		{
			// 변경
			NOTIFYICONDATA nid;
			nid.cbSize = sizeof(NOTIFYICONDATA);
			nid.hWnd = hWnd;
			nid.uID = IDR_MAINFRAME;
			nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			nid.uCallbackMessage = WM_TRAYICON_MSG;
			strcpy( nid.szTip, strTip.c_str() );
			nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
			Shell_NotifyIcon( NIM_MODIFY, &nid );
		}

		static void Delete(HWND hWnd)
		{
			Release(hWnd);
			ShowWindow(hWnd, SW_SHOW);
		}

		static void Release(HWND hWnd)
		{
			NOTIFYICONDATA nid;
			nid.cbSize = sizeof(NOTIFYICONDATA);
			nid.hWnd = hWnd;
			nid.uID = IDR_MAINFRAME;
			Shell_NotifyIcon( NIM_DELETE, &nid );
		}
	};
} //namespace Redmoon

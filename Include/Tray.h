#pragma once

#define WM_TRAYICON_MSG		WM_USER + 100

namespace Redmoon
{
	class CTray
	{
	public:
		static void Register(HWND hWnd, const tstring& strTip)
		{
			// Ʈ���̿� ������ ���
			NOTIFYICONDATA  nid;
			nid.cbSize = sizeof(nid);
			nid.hWnd = hWnd; // ���� ������ �ڵ�
			nid.uID = IDR_MAINFRAME;  // ������ ���ҽ� ID
			nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; // �÷��� ����
			nid.uCallbackMessage = WM_TRAYICON_MSG; // �ݹ�޽��� ����
			nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); // ������ �ε� 

			lstrcpy(nid.szTip, strTip.c_str()); 
			Shell_NotifyIcon(NIM_ADD, &nid);
			::SendMessage(hWnd, WM_SETICON, (WPARAM)TRUE, (LPARAM)nid.hIcon);

			ShowWindow(hWnd, SW_HIDE);
		}

		static void Change(HWND hWnd, const tstring& strTip)
		{
			// ����
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

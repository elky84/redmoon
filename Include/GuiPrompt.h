#pragma once

#include "PromptInterface.h"
#include <WindowsX.h>

namespace Redmoon
{

static LRESULT CALLBACK ExtWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#define IDC_PROMPT_EDITBOX	(WM_USER + 1600)
#define IDC_PROMPT_BUTTON	(WM_USER + 1601)

class CGuiPrompt  : public CPromptInterface
{
public:
	CGuiPrompt(const tstring& strNotice = _T("")) : CPromptInterface(strNotice), m_WndClassEx(), m_hWnd(NULL), m_hEditbox(NULL), m_hButton(NULL)
	{
		Start();
	}

	virtual ~CGuiPrompt()
	{

	}

	virtual void Begin()
	{
		m_WndClassEx.cbSize         = sizeof (m_WndClassEx);
		m_WndClassEx.style          = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
		m_WndClassEx.lpfnWndProc    = ExtWndProc;
		m_WndClassEx.cbClsExtra     = 0;
		m_WndClassEx.cbWndExtra     = DLGWINDOWEXTRA;
		m_WndClassEx.hInstance      = ::GetModuleHandle(NULL);
		m_WndClassEx.hIcon          = NULL;
		m_WndClassEx.hCursor        = ::LoadCursor(NULL, IDC_ARROW);
		m_WndClassEx.hbrBackground  = (HBRUSH)::GetStockObject(WHITE_BRUSH);
		m_WndClassEx.lpszMenuName   = NULL;
		m_WndClassEx.lpszClassName  = "GuiPrompt";
		m_WndClassEx.hIconSm        = NULL;

		if(!RegisterClassEx (&m_WndClassEx))
			return;

		m_hWnd = ::CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,  "GuiPrompt", NULL, WS_OVERLAPPEDWINDOW, 0, 0, 250, 130, NULL, NULL, ::GetModuleHandle(NULL), this);
		ProcessWindow(m_hWnd);

		m_hEditbox = ::CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,10, 10, 220, 25, m_hWnd,(HMENU)IDC_PROMPT_EDITBOX, NULL, this);
		ProcessWindow(m_hEditbox);

		m_hButton = ::CreateWindow("button", "Command", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 40, 220, 25, m_hWnd,(HMENU)IDC_PROMPT_BUTTON, NULL, this);
		ProcessWindow(m_hButton);
	}

	void ProcessWindow(HWND hWnd)
	{
		if(hWnd == NULL)
		{
			Stop();
		}
		else
		{
			ShowWindow(hWnd, SW_SHOW);
			UpdateWindow(hWnd);
		}
	}

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch(uMsg) 			
		{
			HANDLE_MSG(hWnd, WM_PAINT, OnPaint); // We need to handle on the WM_PAINT message 
			HANDLE_MSG(hWnd, WM_CLOSE, OnClose);
			HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		}
		return DefWindowProc (hWnd, uMsg, wParam, lParam) ;
	}

private:
	void PeekMessage()
	{
		MSG msg;
		if(::PeekMessage(&msg,m_hWnd,0,0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT)
				return;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void OnCommand(HWND hWnd, int nControlID, HWND hMenuWnd, UINT uEvent)
	{
		switch(nControlID)
		{
		case IDC_PROMPT_BUTTON:
			{
				Call();
			}
			break;
		case IDC_PROMPT_EDITBOX:
			{
				if(uEvent == EN_CHANGE)
				{
					char szText[128] = "";
					GetWindowText(hMenuWnd, szText, 128);
					m_strCommand = szText;
				}
			}
			break;
		}
	}

	void OnPaint(HWND hWnd)
	{
		UpdateWindow(hWnd);
		if(hWnd == m_hWnd)
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd,&ps);
			TextOut(hdc, 10, 75, m_strNotice.c_str(), (int)m_strNotice.size());
			EndPaint(hWnd,&ps);
		}
	}

	virtual void SetNotice(const tstring& strNotice)
	{
		__super::SetNotice(strNotice);
		SetWindowPos(m_hWnd, NULL, 0, 0, max(250, strNotice.size() * 10), 130, SWP_NOMOVE);
		SetWindowPos(m_hEditbox, NULL, 10, 10, max(250, strNotice.size() * 10) - 30, 25, SWP_NOMOVE);
		SetWindowPos(m_hButton, NULL, 10, 40, max(250, strNotice.size() * 10) - 30, 25, SWP_NOMOVE);
		::InvalidateRect(m_hWnd, NULL, TRUE);
	}

	void OnClose(HWND hWnd)
	{
		Stop();
	}

	virtual void Act()
	{
		PeekMessage();
	}

private:
	WNDCLASSEX m_WndClassEx;
	HWND m_hEditbox;
	HWND m_hButton;
	HWND m_hWnd;
};

static LRESULT CALLBACK ExtWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CGuiPrompt * guiprompt = NULL;
	if(uMsg == WM_CREATE)
	{
		guiprompt = (CGuiPrompt*)((LPCREATESTRUCT)lParam)->lpCreateParams;
	}

	if(guiprompt)
		return guiprompt->WindowProc(hwnd, uMsg, wParam, lParam);
	else
		return DefWindowProc (hwnd, uMsg, wParam, lParam);
}

} //namespace Redmoon
#pragma once

#include <memory.h>
#include "afxwin.h"
#include <vector>

#include "Str.h"
#include "Menu.h"
#include "FunctionPtr.h"

const int MAX_SHORTCUT_NAME = 50;
const int MAX_FILE_NAME = 1024;

struct SExecuteConfig{
	char m_szShortcutName[MAX_SHORTCUT_NAME];
	char m_szParameter[MAX_FILE_NAME];
	char m_szProgramPath[MAX_FILE_NAME];
	char m_szProgramFolder[MAX_FILE_NAME];
	
	SExecuteConfig(char * szShortcutname, char * szParameter, char * szProgramPath, char* szProgramFolder)
	{
		strcpy(m_szShortcutName, szShortcutname);
		strcpy(m_szParameter, szParameter);
		strcpy(m_szProgramPath, szProgramPath);
		strcpy(m_szProgramFolder, szProgramFolder);
	}

	bool operator ==(const SExecuteConfig &executeconfig) const 
	{
		if(stricmp(executeconfig.m_szShortcutName ,m_szShortcutName) != 0 ||
			stricmp(executeconfig.m_szParameter, m_szParameter) != 0 ||
			stricmp(executeconfig.m_szProgramPath, m_szProgramPath) != 0 ||
			stricmp(m_szProgramFolder, m_szProgramFolder) != 0)
		{
			return false;
		}
		return true;
	}

	bool operator != (const SExecuteConfig & refExecuteConfig)
	{
		return !(*this == refExecuteConfig);
	}
};

void Log(char *str, ...);

// CEasyExecDlg 대화 상자
class CEasyExecDlg : public CDialog
{
// 생성
public:
	CEasyExecDlg(CWnd* pParent = NULL);	// 표준 생성자
	~CEasyExecDlg();

// 대화 상자 데이터
	enum { IDD = IDD_EZEXEC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	CString m_strProgramPath;
	CString m_strParam;
	CString m_strProgramFolder;
	CListBox m_listboxParameter;
	CString m_strShortcutName;
	std::vector<SExecuteConfig> m_vecExecuteConfig;
	tstring m_strFileDialogPath;
	tstring m_strFinalUsingFileName;
	CStatic m_static_CFGFileName;
	BOOL m_bTrayIcon;
	Redmoon::CPopup m_cPopup;

	afx_msg void OnBnClickedButtonFolderFind();
	afx_msg void OnBnClickedButtonFileFind();
	afx_msg void OnBnClickedButtonExecute();
	afx_msg void OnBnClickedButtonSaveSetting();
	afx_msg void OnBnClickedButtonLoadSetting();
	afx_msg void OnLbnSelchangeListParameter();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonRemove();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnEnKillfocusEditTargetProgram();
	afx_msg void OnEnKillfocusEditParameter();

	void InitInputDlg();
	void Clear();
	void CopyExecuteConfig(const SExecuteConfig& refExecuteConfig);
	void CopyControlVariableValue(SExecuteConfig& refExecuteConfig);

	void LoadSetting();
	void SaveSetting();
	void LoadConfig(const char* szFileName);
	void SaveConfig(const char* szFileName);

	void SelectFolder();

	afx_msg LRESULT OnClose(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrayIconMsg(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnDestroy(WPARAM wParam, LPARAM lParam);

	bool Exit(void* pArgument);
	void DrawIcon();

	virtual BOOL UpdateData(BOOL bSaveAndValidate  = TRUE);

private:
	void RegisterTrayIcon();
	void DeleteTrayIcon();
};

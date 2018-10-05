// EasyExecDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "EasyExec.h"
#include "EasyExecDlg.h"

#include <afxtempl.h>

#include "DirectoryUtil.h"

#include "FileStream.h"

#include "LogManager.h"
#include ".\easyexecdlg.h"

#include "Tray.h"
#include "Icon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
@brief 문자열을 파일에 저장하는 메소드
@param fp 파일 포인터
@param szString 저장할 문자열
@desc 파일에 문자열길이 (unsigned short형)와 문자열 순서로 저장한다.
*/
inline void WriteString(FILE* fp, const char* szString)
{
	unsigned short unSize;
	char szBuffer[MAX_FILE_NAME] = "";

	unSize = (unsigned short)strlen(szString) + 1;
	fwrite(&unSize, sizeof(unSize), 1, fp);
	strcpy(szBuffer, szString);
	fwrite(szBuffer, unSize, 1, fp);
}

/**
@brief 문자열을 파일에서 읽어오는 메소드.
@param fp 파일 포인터
@return fp에서 읽은 문자열
@desc 파일에 문자열길이 (unsigned short형)와 문자열로 이어져 저장된 상황에서만 사용가능.
*/
inline const char* ReadString(FILE* fp)
{
	unsigned short unSize;
	static char szBuffer[MAX_FILE_NAME];
	memset(szBuffer, 0, sizeof(szBuffer));

	fread(&unSize, sizeof(unSize), 1, fp);
	fread(szBuffer, unSize, 1, fp);
	return szBuffer;
}

inline int CALLBACK MyCallbackFunc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	}
	return 0;
}

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CEasyExecDlg 대화 상자



CEasyExecDlg::CEasyExecDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEasyExecDlg::IDD, pParent)
	, m_strProgramPath(_T(""))
	, m_strParam(_T(""))
	, m_strShortcutName(_T(""))
	, m_bTrayIcon(FALSE)
	, m_cPopup(m_hWnd)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_cPopup.Add("&Exit", Redmoon::SetProc(this, CEasyExecDlg::Exit));
}

CEasyExecDlg::~CEasyExecDlg()
{
}

void CEasyExecDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TARGET_PROGRAM, m_strProgramPath);
	DDX_Text(pDX, IDC_EDIT_PARAMETER, m_strParam);
	DDX_Control(pDX, IDC_LIST_PARAMETER, m_listboxParameter);
	DDX_Text(pDX, IDC_EDIT_SHORTCUT_NAME, m_strShortcutName);
	DDX_Control(pDX, IDC_STATIC_CFGFILENAME, m_static_CFGFileName);
}

BEGIN_MESSAGE_MAP(CEasyExecDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_FILE_FIND, OnBnClickedButtonFileFind)
	ON_BN_CLICKED(IDC_BUTTON_EXECUTE, OnBnClickedButtonExecute)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_SETTING, OnBnClickedButtonSaveSetting)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_SETTING, OnBnClickedButtonLoadSetting)
	ON_LBN_SELCHANGE(IDC_LIST_PARAMETER, OnLbnSelchangeListParameter)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnBnClickedButtonModify)
	ON_EN_KILLFOCUS(IDC_EDIT_TARGET_PROGRAM, OnEnKillfocusEditTargetProgram)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER, OnEnKillfocusEditParameter)
	ON_BN_CLICKED(IDC_BUTTON_FOLDER_FIND, OnBnClickedButtonFolderFind)
	ON_MESSAGE(WM_CLOSE, OnClose)
	ON_MESSAGE(WM_TRAYICON_MSG, OnTrayIconMsg)
	ON_MESSAGE(WM_DESTROY, OnDestroy)
END_MESSAGE_MAP()


// CEasyExecDlg 메시지 처리기

BOOL CEasyExecDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.
	LoadSetting();
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CEasyExecDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ((nID & 0xFFF0) == SC_MINIMIZE)
	{
		RegisterTrayIcon();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CEasyExecDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);

		DrawIcon();
	}
	else
	{
		CDialog::OnPaint();
		DrawIcon();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CEasyExecDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CEasyExecDlg::OnBnClickedButtonFileFind()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY, "Execute Files(*.exe)|*.exe||");
	{
		Redmoon::CRestorePath restorepath;
		dlg.m_ofn.lpstrInitialDir = m_strFileDialogPath.c_str();
		if(dlg.DoModal() == IDCANCEL)
			return;

	}

	InitInputDlg();

	m_strFileDialogPath = dlg.GetPathName();
	m_strProgramPath = dlg.GetPathName();
	CString strFileName = dlg.GetFileName();

	char szBuffer[MAX_FILE_NAME] = "";
	memcpy(szBuffer, m_strProgramPath.GetBuffer(), m_strProgramPath.GetLength() - strFileName.GetLength());
	m_strProgramFolder = szBuffer;

	UpdateData(FALSE);
}

void CEasyExecDlg::DrawIcon()
{
	CClientDC dc(this);
	Redmoon::CIcon icon(m_strProgramPath.GetBuffer());

	if(icon.GetIcon())
		dc.DrawIcon(20, 20, icon.GetIcon());
	else
		dc.DrawIcon(20, 20, GetIcon(TRUE));
}

void CEasyExecDlg::OnBnClickedButtonExecute()
{
	UpdateData(TRUE);
	
	if(m_strProgramPath.GetLength() <= 0)
	{
		AfxMessageBox("프로그램 경로를 입력해주세요.");
		return;
	}

	::ShellExecute(NULL, "open", m_strProgramPath, m_strParam, m_strProgramFolder, SW_SHOWNORMAL);
}

void CEasyExecDlg::OnBnClickedButtonSaveSetting()
{
	CFileDialog dlg(FALSE, "cfg", NULL, OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY, "Config Files(*.cfg)|*.cfg||");
	{
		Redmoon::CRestorePath restorepath;
		tstring strCurrentDirectory = restorepath.GetCurrentDirectory();
		strCurrentDirectory += "\\cfg\\";
		dlg.m_ofn.lpstrInitialDir = strCurrentDirectory.c_str();
		if(dlg.DoModal() == IDCANCEL)
			return;
	}

	UpdateData(TRUE);
	CString strFilePath = dlg.GetPathName();
	SaveConfig(strFilePath.GetBuffer());
}

void CEasyExecDlg::OnBnClickedButtonLoadSetting()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY, "Config Files(*.cfg)|*.cfg||");
	{
		Redmoon::CRestorePath restorepath;
		tstring strCurrentDirectory = restorepath.GetCurrentDirectory();
		strCurrentDirectory += "\\cfg\\";
		dlg.m_ofn.lpstrInitialDir = strCurrentDirectory.c_str();
		if(dlg.DoModal() == IDCANCEL)
			return;
	}
	
	CString strFilePath = dlg.GetPathName();
	LoadConfig(strFilePath.GetBuffer());
	UpdateData(FALSE);
}

void CEasyExecDlg::OnLbnSelchangeListParameter()
{
	UpdateData(TRUE);

	int nCursel = m_listboxParameter.GetCurSel();
	if(nCursel < 0)
		return;

	SExecuteConfig &refFirstExecuteConfig = m_vecExecuteConfig[nCursel];
	CopyExecuteConfig(refFirstExecuteConfig);
	UpdateData(FALSE);
}

void CEasyExecDlg::OnBnClickedButtonAdd()
{
	UpdateData(TRUE);

	if(m_strShortcutName.GetLength() <= 0)
	{
		AfxMessageBox("바로가기 이름을 입력해주세요.");
		return;
	}

	SExecuteConfig cExecuteConfig(m_strShortcutName.GetBuffer(), m_strParam.GetBuffer(), m_strProgramPath.GetBuffer(), m_strProgramFolder.GetBuffer());
	m_vecExecuteConfig.insert(m_vecExecuteConfig.begin(), cExecuteConfig);
	m_listboxParameter.InsertString(0, m_strShortcutName);
	
	InitInputDlg();
}

void CEasyExecDlg::OnBnClickedButtonRemove()
{
	UpdateData(TRUE);

	int nCursel = m_listboxParameter.GetCurSel();
	if(nCursel < 0)
	{
		AfxMessageBox("바로가기를 선택하지 않으셨습니다.");
		return;
	}

	m_vecExecuteConfig.erase(m_vecExecuteConfig.begin() + nCursel);
	m_listboxParameter.DeleteString(nCursel);

	InitInputDlg();
}

void CEasyExecDlg::InitInputDlg()
{
	m_strProgramPath = "";
	m_strShortcutName = "";
	m_strParam = "";
	m_strFileDialogPath = "";
	UpdateData(FALSE);
}

void CEasyExecDlg::Clear()
{
	m_vecExecuteConfig.clear();
	m_listboxParameter.ResetContent();
	InitInputDlg();
}

void CEasyExecDlg::OnBnClickedButtonModify()
{
	UpdateData(TRUE);

	int nCursel = m_listboxParameter.GetCurSel();
	if(nCursel < 0)
	{
		AfxMessageBox("바로가기를 선택하지 않으셨습니다.");
		return;
	}

	CopyControlVariableValue(m_vecExecuteConfig[nCursel]);
	m_listboxParameter.DeleteString(nCursel);
	m_listboxParameter.InsertString(nCursel, m_strShortcutName);

	UpdateData(FALSE);
}

void CEasyExecDlg::OnEnKillfocusEditTargetProgram()
{
	UpdateData(TRUE);
	char szBuffer[MAX_FILE_NAME] = "";
	strncpy(szBuffer, m_strProgramPath.GetBuffer(), m_strProgramPath.GetLength());

	char tok[] = "\\";
	UINT filename_length = 0;
	char *szTemp = strtok(szBuffer, tok);
	while(szTemp != NULL)
	{
		filename_length = (UINT)strlen(szTemp);
		szTemp = strtok(NULL, tok);
	}

	memcpy(szBuffer, m_strProgramPath.GetBuffer(), m_strProgramPath.GetLength() - filename_length);
	m_strProgramFolder = szBuffer;
	UpdateData(FALSE);
}

void CEasyExecDlg::OnEnKillfocusEditParameter()
{
	UpdateData(TRUE);
}

void CEasyExecDlg::CopyExecuteConfig(const SExecuteConfig& refExecuteConfig)
{
	m_strParam = refExecuteConfig.m_szParameter;
	m_strShortcutName = refExecuteConfig.m_szShortcutName;
	m_strProgramPath = refExecuteConfig.m_szProgramPath;

	m_strProgramFolder = refExecuteConfig.m_szProgramFolder;

	m_strFileDialogPath = refExecuteConfig.m_szProgramPath;
}

void CEasyExecDlg::CopyControlVariableValue(SExecuteConfig& refExecuteConfig)
{
	if(0 < m_strParam.GetLength())
	{
		memset(refExecuteConfig.m_szParameter, 0, MAX_FILE_NAME);
		memcpy(refExecuteConfig.m_szParameter, m_strParam.GetBuffer(), m_strParam.GetLength());
	}

	if(0 < m_strShortcutName.GetLength())
	{
		memset(refExecuteConfig.m_szShortcutName, 0, MAX_SHORTCUT_NAME);
		memcpy(refExecuteConfig.m_szShortcutName, m_strShortcutName.GetBuffer(), m_strShortcutName.GetLength());
	}

	if(0 < m_strProgramPath.GetLength())
	{
		memset(refExecuteConfig.m_szProgramPath, 0, MAX_FILE_NAME);
		memcpy(refExecuteConfig.m_szProgramPath, m_strProgramPath.GetBuffer(), m_strProgramPath.GetLength());
	}

	if(0 < m_strProgramFolder.GetLength())
	{
		memset(refExecuteConfig.m_szProgramFolder, 0, MAX_FILE_NAME);
		memcpy(refExecuteConfig.m_szProgramFolder, m_strProgramFolder.GetBuffer(), m_strProgramFolder.GetLength());
	}
}

void CEasyExecDlg::LoadConfig(const char* szFileName)
{
	FILE *fp = fopen(szFileName, "rt") ;
	if(fp == NULL)
	{
		MessageBox("LoadConfig : File Load Failed");
		return;
	}

	fseek(fp, 0, SEEK_END);
	size_t nFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	Clear();

	unsigned int nParamCount = 0;
	fread(&nParamCount, sizeof(nParamCount), 1, fp);
	for(UINT i = 0; i < nParamCount; i++)
	{
		m_strProgramPath = ReadString(fp);

		m_strProgramFolder = ReadString(fp);
		m_strShortcutName = ReadString(fp);
		m_strParam = ReadString(fp);

		m_vecExecuteConfig.insert(m_vecExecuteConfig.begin(), SExecuteConfig(m_strShortcutName.GetBuffer(), m_strParam.GetBuffer(), m_strProgramPath.GetBuffer(), m_strProgramFolder.GetBuffer()));
		m_listboxParameter.InsertString(0, m_strShortcutName);
	}

	fclose(fp);
	m_static_CFGFileName.SetWindowText(szFileName);
}

void CEasyExecDlg::SaveConfig(const char* szFileName)
{
	FILE *fp = fopen(szFileName, "wt+");
	if(fp == NULL)
	{
		MessageBox("SaveConfig : File Save Failed");
		return;
	}

	unsigned int nParamCount = (unsigned int)m_vecExecuteConfig.size();
	fwrite(&nParamCount, sizeof(nParamCount), 1, fp);
	for(UINT i = 0; i < m_vecExecuteConfig.size(); i++)
	{
		WriteString(fp, m_vecExecuteConfig[i].m_szProgramPath);
		WriteString(fp, m_vecExecuteConfig[i].m_szProgramFolder);
		WriteString(fp, m_vecExecuteConfig[i].m_szShortcutName);
		WriteString(fp, m_vecExecuteConfig[i].m_szParameter);
	}
	fclose(fp);
	m_strFinalUsingFileName = szFileName;
	m_static_CFGFileName.SetWindowText(szFileName);
	SaveSetting();
}

void CEasyExecDlg::SaveSetting()
{
	tstring strCurrectDirectory = Redmoon::CDirectoryUtil::GetCurrentDirectory() + ".\\CFG";
	Redmoon::CDirectoryUtil::CreateDirectory(strCurrectDirectory);
	FILE *fp = fopen(".\\CFG\\Config.dat", "wt+");
	if(fp == NULL)
	{
		MessageBox("SaveSetting : File Save Failed");
		return;
	}

	WriteString(fp, m_strFinalUsingFileName.c_str());
	fclose(fp);
}



void CEasyExecDlg::LoadSetting()
{
	Redmoon::CRestorePath path;
	FILE *fp = fopen(".\\CFG\\Config.dat", "rt");
	if(fp == NULL)
	{
		return;
	}

	m_strFinalUsingFileName = ReadString(fp);

	LoadConfig(m_strFinalUsingFileName.c_str());
	fclose(fp);
}

void CEasyExecDlg::SelectFolder()
{
	TCHAR strPath[MAX_PATH];

	BROWSEINFO browse;

	browse.hwndOwner = this->m_hWnd;
	browse.pidlRoot = NULL;
	browse.pszDisplayName = NULL;
	browse.lpszTitle = "원하는 폴더를 선택 하세요.."; // 타이틀 지정
	browse.ulFlags = BIF_RETURNONLYFSDIRS;
	browse.lpfn = MyCallbackFunc; // 초기 경로를 설정하지 않으려면 NULL로 처리한다.
	// 초기 경로를 설정하려면 CallBack 함수를 연결 해주어야 한다.
	browse.lParam = (LPARAM)(LPCTSTR)m_strProgramFolder;

	LPITEMIDLIST pidl = SHBrowseForFolder(&browse);

	if(pidl == NULL) return;
	if(!SHGetPathFromIDList(pidl,strPath)) return;
	
	InitInputDlg();

	m_strFileDialogPath = strPath;
	m_strProgramPath = strPath;

	CString strFileName = strPath;

	char szBuffer[MAX_FILE_NAME] = "";
	memcpy(szBuffer, m_strProgramPath.GetBuffer(), m_strProgramPath.GetLength() - strFileName.GetLength());
	m_strProgramFolder = szBuffer;

	UpdateData(FALSE);
}
void CEasyExecDlg::OnBnClickedButtonFolderFind()
{
	SelectFolder();
}

LRESULT CEasyExecDlg::OnClose(WPARAM wParam, LPARAM lParam)
{
	RegisterTrayIcon();
	return 0;
}

LRESULT CEasyExecDlg::OnTrayIconMsg(WPARAM wParam, LPARAM lParam)
{
	if(lParam == WM_LBUTTONDBLCLK)
	{  
		DeleteTrayIcon();
	}
	else if(lParam == WM_RBUTTONDOWN)
	{ 
		m_cPopup.Show();
	}
	return 0;
}

void CEasyExecDlg::RegisterTrayIcon()
{
	Redmoon::CTray::Register(m_hWnd, "EasyExec");
	m_bTrayIcon = TRUE;
}

void CEasyExecDlg::DeleteTrayIcon()
{
	Redmoon::CTray::Delete(m_hWnd);
	m_bTrayIcon = FALSE;
}

BOOL CEasyExecDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return CDialog::OnCommand(wParam, lParam);
}

bool CEasyExecDlg::Exit(void* pArgument)
{
	PostMessage(WM_QUIT);
	return true;
}

LRESULT CEasyExecDlg::OnDestroy(WPARAM wParam, LPARAM lParam)
{
	if(m_bTrayIcon)
	{
		Redmoon::CTray::Release(m_hWnd);
	}
	return 0;
}

BOOL CEasyExecDlg::UpdateData(BOOL bSaveAndValidate /* = TRUE */)
{
	RECT rect = {20, 20, 52, 52};
	InvalidateRect(&rect);

	return __super::UpdateData(bSaveAndValidate);
}
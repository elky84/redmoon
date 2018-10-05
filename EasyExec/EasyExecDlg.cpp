// EasyExecDlg.cpp : ���� ����
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
@brief ���ڿ��� ���Ͽ� �����ϴ� �޼ҵ�
@param fp ���� ������
@param szString ������ ���ڿ�
@desc ���Ͽ� ���ڿ����� (unsigned short��)�� ���ڿ� ������ �����Ѵ�.
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
@brief ���ڿ��� ���Ͽ��� �о���� �޼ҵ�.
@param fp ���� ������
@return fp���� ���� ���ڿ�
@desc ���Ͽ� ���ڿ����� (unsigned short��)�� ���ڿ��� �̾��� ����� ��Ȳ������ ��밡��.
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

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����

// ����
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


// CEasyExecDlg ��ȭ ����



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


// CEasyExecDlg �޽��� ó����

BOOL CEasyExecDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.
	LoadSetting();
	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CEasyExecDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);

		DrawIcon();
	}
	else
	{
		CDialog::OnPaint();
		DrawIcon();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
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
		AfxMessageBox("���α׷� ��θ� �Է����ּ���.");
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
		AfxMessageBox("�ٷΰ��� �̸��� �Է����ּ���.");
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
		AfxMessageBox("�ٷΰ��⸦ �������� �����̽��ϴ�.");
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
		AfxMessageBox("�ٷΰ��⸦ �������� �����̽��ϴ�.");
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
	browse.lpszTitle = "���ϴ� ������ ���� �ϼ���.."; // Ÿ��Ʋ ����
	browse.ulFlags = BIF_RETURNONLYFSDIRS;
	browse.lpfn = MyCallbackFunc; // �ʱ� ��θ� �������� �������� NULL�� ó���Ѵ�.
	// �ʱ� ��θ� �����Ϸ��� CallBack �Լ��� ���� ���־�� �Ѵ�.
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
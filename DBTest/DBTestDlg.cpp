// DBTestDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "DBTest.h"
#include "DBTestDlg.h"
#include "tinyxml.h"
#include "Str.h"
#include "ParamTypeText.h"
#include "SPList.h"
#include ".\dbtestdlg.h"
#include "DirectoryUtil.h"

//#include "CON_ACCOUNT_RECORD.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CDBTestDlg 대화 상자




CDBTestDlg::CDBTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDBTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pcLoginDlg.Reset(new CLoginDlg(&m_DB));
	g_pcThis = this;

	m_psSelectedDBInfo = NULL;
}

CDBTestDlg::~CDBTestDlg()
{
	m_pcLoginDlg.Reset();
}

void CDBTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TABLES, m_listctrlTables);
	DDX_Control(pDX, IDC_LIST_FIELDS, m_listctrlFields);
	DDX_Control(pDX, IDC_LIST_LOG, m_listboxLog);
	DDX_Control(pDX, IDC_STATIC_CURRENT_DBINFO, m_LableDBInfo);
	DDX_Control(pDX, IDC_LIST_SP, m_listCtrlSP);
	DDX_Control(pDX, IDC_LIST_SP_PARAMETERS, m_listctrlSPParameters);
}

BEGIN_MESSAGE_MAP(CDBTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TABLES, &CDBTestDlg::OnLvnItemchangedListTables)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SP,		&CDBTestDlg::OnLvnItemchangedListSP)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TABLES,		&CDBTestDlg::OnNMDblclkListTable)

	ON_BN_CLICKED(IDC_BUTTON_SP_CODE_GENERATE,	&CDBTestDlg::OnBnClickedButtonSpCodeGenerate)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN,				&CDBTestDlg::OnBnClickedButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON_TABLE_CODE_GENERATE,		&CDBTestDlg::OnBnClickedButtonTableCodeGenerate)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH_TABLELIST, &CDBTestDlg::OnBnClickedButtonRefreshTablelist)
	ON_BN_CLICKED(IDC_BUTTON_SP_LOAD, &CDBTestDlg::OnBnClickedButtonSpLoad)
	ON_BN_CLICKED(IDC_BUTTON_CMD_TABLE, &CDBTestDlg::OnBnClickedButtonCmdTable)
	ON_BN_CLICKED(IDC_BUTTON_CMD_SP, &CDBTestDlg::OnBnClickedButtonCmdSp)
	ON_BN_CLICKED(IDC_BUTTON_SP_SELECT, &CDBTestDlg::OnBnClickedButtonSpSelect)
	ON_BN_CLICKED(IDC_BUTTON_SP_UPDATE, &CDBTestDlg::OnBnClickedButtonSpUpdate)
	ON_BN_CLICKED(IDC_BUTTON_SP_INSERT, &CDBTestDlg::OnBnClickedButtonSpInsert)
	ON_BN_CLICKED(IDC_BUTTON_SP_DELETE, &CDBTestDlg::OnBnClickedButtonSpDelete)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SP, &CDBTestDlg::OnNMDblclkListSp)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_FIELDS, &CDBTestDlg::OnLvnKeydownListFields)
	ON_BN_CLICKED(IDC_BUTTON_SP_GENERATE_SELECT, &CDBTestDlg::OnBnClickedButtonSpGenerateSelect)
	ON_BN_CLICKED(IDC_BUTTON_SP_GENERATE_INSERT, &CDBTestDlg::OnBnClickedButtonSpGenerateInsert)
	ON_BN_CLICKED(IDC_BUTTON_SP_GENERATE_UPDATE, &CDBTestDlg::OnBnClickedButtonSpGenerateUpdate)
	ON_BN_CLICKED(IDC_BUTTON_SP_GENERATE_DELETE, &CDBTestDlg::OnBnClickedButtonSpGenerateDelete)
	ON_BN_CLICKED(IDC_BUTTON_CODE_SPSELECT, &CDBTestDlg::OnBnClickedButtonCodeSpselect)
	ON_BN_CLICKED(IDC_BUTTON_SP_CODE_GENERATE, OnBnClickedButtonSpCodeGenerate)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SP, OnLvnItemchangedListSp)
END_MESSAGE_MAP()


// CDBTestDlg 메시지 처리기

BOOL CDBTestDlg::OnInitDialog()
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
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	ReadXml();

	InitHeaderTableList();
	InitHeaderFieldList();
	InitHeaderSPList();
	InitHeaderSPParameter();

	OnBnClickedButtonLogin();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CDBTestDlg::InitHeaderSPList()
{
	RECT	rect;
	m_listCtrlSP.GetClientRect( &rect );

	int width = rect.right - rect.left;

	m_listCtrlSP.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_SORTASCENDING );
	m_listCtrlSP.InsertColumn( 0, _T("Name"), LVCFMT_CENTER, static_cast<int>( width * 0.95f ) );
}

void CDBTestDlg::InitHeaderSPParameter()
{
	RECT	rect;
	m_listctrlSPParameters.GetClientRect( &rect );

	int width = rect.right - rect.left;

	m_listctrlSPParameters.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_SORTASCENDING );
	m_listctrlSPParameters.InsertColumn( 0, _T("Parameter Name"), LVCFMT_LEFT, static_cast<int>( width * 0.4f ) );
	m_listctrlSPParameters.InsertColumn( 1, _T("Type"), LVCFMT_LEFT, static_cast<int>( width * 0.3f ) );
	m_listctrlSPParameters.InsertColumn( 2, _T("IOType"), LVCFMT_LEFT, static_cast<int>( width * 0.3f ) );
}

void CDBTestDlg::ReadXml()
{
	TiXmlDocument xmldoc("cfg\\DBList.xml");
	xmldoc.LoadFile();

	m_vectorDBInfo.clear();
	const TiXmlElement* eleDBList = xmldoc.FirstChildElement("DBList");
	if( eleDBList )
	{
		const TiXmlElement* eleDBInfo = eleDBList->FirstChildElement("DBInfo");
		while(eleDBInfo)
		{
			SDBInfo cDBInfo;

			cDBInfo.m_strNo		= CHAR_TO_TCHAR(TiAsString(eleDBInfo, "NO"));
			cDBInfo.m_strIP		= CHAR_TO_TCHAR(TiAsString(eleDBInfo, "IP"));
			cDBInfo.m_strDBName	= CHAR_TO_TCHAR(TiAsString(eleDBInfo, "DBNAME"));
			cDBInfo.m_strLogin	= CHAR_TO_TCHAR(TiAsString(eleDBInfo, "LOGIN"));
			cDBInfo.m_strPWD	= CHAR_TO_TCHAR(TiAsString(eleDBInfo, "PWD"));
			cDBInfo.m_strPort	= CHAR_TO_TCHAR(TiAsString(eleDBInfo, "PORT"));

			const TiXmlElement* elePath = eleDBInfo->FirstChildElement("Setting");
			if(elePath)
			{
				cDBInfo.m_strSPNamePrefix		= CHAR_TO_TCHAR(TiAsString(elePath, "SPNamePrefix"));
				cDBInfo.m_strSPNameSuffixSelect	= CHAR_TO_TCHAR(TiAsString(elePath, "SPNameSuffixSelect"));
				cDBInfo.m_strSPNameSuffixUpdate	= CHAR_TO_TCHAR(TiAsString(elePath, "SPNameSuffixUpdate"));
				cDBInfo.m_strSPNameSuffixInsert	= CHAR_TO_TCHAR(TiAsString(elePath, "SPNameSuffixInsert"));
				cDBInfo.m_strSPNameSuffixDelete	= CHAR_TO_TCHAR(TiAsString(elePath, "SPNameSuffixDelete"));

				cDBInfo.m_strPathCmdClassTable = CHAR_TO_TCHAR(TiAsString(elePath, "PathCmdClassTable"));
				Redmoon::CDirectoryUtil::CreateDirectory(cDBInfo.m_strPathCmdClassTable);

				cDBInfo.m_strPathCmdClassProcedure = CHAR_TO_TCHAR(TiAsString(elePath, "PathCmdClassProcedure"));
				Redmoon::CDirectoryUtil::CreateDirectory(cDBInfo.m_strPathCmdClassProcedure);

				cDBInfo.m_strPathSPSelect = CHAR_TO_TCHAR(TiAsString(elePath, "PathSPSelect"));
				Redmoon::CDirectoryUtil::CreateDirectory(cDBInfo.m_strPathSPSelect);

				cDBInfo.m_strPathSPUpdate = CHAR_TO_TCHAR(TiAsString(elePath, "PathSPUpdate"));
				Redmoon::CDirectoryUtil::CreateDirectory(cDBInfo.m_strPathSPUpdate);

				cDBInfo.m_strPathSPInsert = CHAR_TO_TCHAR(TiAsString(elePath, "PathSPInsert"));
				Redmoon::CDirectoryUtil::CreateDirectory(cDBInfo.m_strPathSPInsert);

				cDBInfo.m_strPathSPDelete = CHAR_TO_TCHAR(TiAsString(elePath, "PathSPDelete"));
				Redmoon::CDirectoryUtil::CreateDirectory(cDBInfo.m_strPathSPDelete);
			}

			m_vectorDBInfo.push_back(cDBInfo);
			eleDBInfo = eleDBInfo->NextSiblingElement();
		}
	}
	xmldoc.Clear();
}

void CDBTestDlg::InitHeaderTableList()
{
	RECT	rect;
	m_listctrlTables.GetClientRect( &rect );

	int width = rect.right - rect.left;

	m_listctrlTables.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_SORTASCENDING );
	m_listctrlTables.InsertColumn( 0, _T("Name"), LVCFMT_CENTER, static_cast<int>( width * 0.8f ) );
	m_listctrlTables.InsertColumn( 1, _T("Type"), LVCFMT_LEFT, static_cast<int>( width * 0.14f ) );
}

void CDBTestDlg::InitHeaderFieldList()
{
	RECT	rect;
	m_listctrlFields.GetClientRect( &rect );

	int width = rect.right - rect.left;

	m_listctrlFields.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_SORTASCENDING | LVS_EX_CHECKBOXES );
	m_listctrlFields.InsertColumn( 0, _T("Field Name"), LVCFMT_CENTER, static_cast<int>( width * 0.4f ) );
	m_listctrlFields.InsertColumn( 1, _T("Type"), LVCFMT_LEFT, static_cast<int>( width * 0.2f ) );
	m_listctrlFields.InsertColumn( 2, _T("CodeType"), LVCFMT_LEFT, static_cast<int>( width * 0.1f ) );
	m_listctrlFields.InsertColumn( 3, _T("identity"), LVCFMT_LEFT, static_cast<int>( width * 0.2f ) );
	m_listctrlFields.InsertColumn( 4, _T("CodeDefineIn"), LVCFMT_LEFT, static_cast<int>( width * 0.05f ) );
	m_listctrlFields.InsertColumn( 5, _T("CodeDefineOut"), LVCFMT_LEFT, static_cast<int>( width * 0.05f ) );
}

void CDBTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CDBTestDlg::OnPaint()
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
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CDBTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDBTestDlg::OnBnClickedButtonLogin()
{
	m_pcLoginDlg->SetDBInfo(&m_vectorDBInfo);

	INT_PTR nRet = m_pcLoginDlg->DoModal();
	if(nRet == IDOK)
	{
		CString strNo = m_pcLoginDlg->GetSelectNo();
		std::vector<SDBInfo>::iterator it;
		for(it=m_vectorDBInfo.begin(); it!=m_vectorDBInfo.end(); ++it)
		{
			SDBInfo& info = *it;
			if(strNo.Compare(info.m_strNo.c_str())==0)
			{
				m_psSelectedDBInfo = &(*it);
				::SetWindowText(::GetDlgItem(this->m_hWnd, IDC_EDIT_CMD_TABLE), info.m_strPathCmdClassTable.c_str());
				::SetWindowText(::GetDlgItem(this->m_hWnd, IDC_EDIT_CMD_SP),	info.m_strPathCmdClassProcedure.c_str());
				::SetWindowText(::GetDlgItem(this->m_hWnd, IDC_EDIT_SP_SELECT), info.m_strPathSPSelect.c_str());
				::SetWindowText(::GetDlgItem(this->m_hWnd, IDC_EDIT_SP_UPDATE), info.m_strPathSPUpdate.c_str());
				::SetWindowText(::GetDlgItem(this->m_hWnd, IDC_EDIT_SP_INSERT), info.m_strPathSPInsert.c_str());
				::SetWindowText(::GetDlgItem(this->m_hWnd, IDC_EDIT_SP_DELETE), info.m_strPathSPDelete.c_str());

				break;
			}
		}

		tstring strDBInfo;
		strDBInfo.append(CHAR_TO_TCHAR(m_DB.GetIP().c_str()));
		strDBInfo.append(_T("["));
		strDBInfo.append(CHAR_TO_TCHAR(m_DB.GetDBName().c_str()));
		strDBInfo.append(_T("]"));
		m_LableDBInfo.SetWindowText(strDBInfo.c_str());

		LoadTableList();
		LoadProcedureList();
	}
}

void CDBTestDlg::OnLvnItemchangedListTables(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( pNMLV->iItem > -1 )
	{
		m_listctrlFields.DeleteAllItems();

		tstring strTableName = m_listctrlTables.GetItemText(pNMLV->iItem, 0);
		TCHAR strTemp[300];
		_stprintf(strTemp, _T("* %s"), strTableName.c_str());
		::SetWindowText(::GetDlgItem(this->m_hWnd, IDC_STATIC_FIELD), strTemp);

		int uid = static_cast<int>(m_listctrlTables.GetItemData( pNMLV->iItem ));

		Redmoon::CDBTable * pTable = m_DB.FindTable( uid );
		if( pTable )
		{
			Redmoon::FieldIterator itr = pTable->GetBeginField();
			while( itr != pTable->GetEndField() )
			{
				int count = m_listctrlFields.GetItemCount();
				Redmoon::SFieldInfo *fieldinfo = (*itr);
				if(fieldinfo->GetName())
					m_listctrlFields.InsertItem( count, fieldinfo->GetName() );
				m_listctrlFields.SetItemText( count, 1, Redmoon::GetSQLDBTypeString(fieldinfo->GetDataType(), fieldinfo->GetDataLength(), fieldinfo->GetNumericPrecision(), fieldinfo->GetNumericScale()).c_str());
				m_listctrlFields.SetItemText( count, 2, Redmoon::GetSQLDBTypeString(fieldinfo->GetDataType(), fieldinfo->GetDataLength(), fieldinfo->GetNumericPrecision(), fieldinfo->GetNumericScale()).c_str());
				m_listctrlFields.SetItemText( count, 3, (fieldinfo->IsIdentiry() ? _T("Y"): _T("N")) );
				m_listctrlFields.SetItemText( count, 4, Redmoon::GetSQLDBTypeString(fieldinfo->GetDataType(), fieldinfo->GetDataLength(), fieldinfo->GetName(), fieldinfo->GetNumericPrecision(), fieldinfo->GetNumericScale()).c_str());
				m_listctrlFields.SetItemText( count, 5, Redmoon::GetSQLDBTypeString(fieldinfo->GetDataType(), fieldinfo->GetDataLength(), fieldinfo->GetName(), fieldinfo->GetNumericPrecision(), fieldinfo->GetNumericScale(), _T("in_")).c_str());
				++itr;
			}
		}
	}
	*pResult = 0;
}

void CDBTestDlg::LoadTableList()
{
	if(!m_DB.IsConnected()) 
	{
		AfxMessageBox(_T("Not Connected DB"));
		return;
	}

	m_listctrlTables.DeleteAllItems();
	m_listctrlFields.DeleteAllItems();

	int count = 0;
	Redmoon::CDBTable * pTable = NULL;
	m_DB.LoadAllTable();
	Redmoon::COleDBAccessor::TableIterator itr = m_DB.GetBeginTable();
	while( itr != m_DB.GetEndTable() )
	{
		pTable = itr->second;
		int newItemIndex = m_listctrlTables.InsertItem( count, pTable->GetName() );
		if( pTable->GetType() == Redmoon::IID_TABLE )
		{
			m_listctrlTables.SetItemText( newItemIndex, 1, _T("T") );
			m_listctrlTables.SetItemData( newItemIndex, static_cast<DWORD_PTR>(pTable->GetTableIdx()) );
		}
		else 
		{
			m_listctrlTables.SetItemText( newItemIndex, 1, _T("V") );
			m_listctrlTables.SetItemData( newItemIndex, static_cast<DWORD_PTR>(pTable->GetTableIdx()) );
		}

		++itr;
		++count;
	}

	if (m_listctrlTables.GetItemCount()>0)
	{
		m_listctrlTables.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	}

	m_listctrlTables.SetFocus();
	m_listctrlTables.UpdateData();
}


void CDBTestDlg::OnBnClickedButtonTableCodeGenerate()
{
	if(GetCurSel(&m_listctrlTables) < 0)
	{
		Alert(_T("Not Select Table"));
		return;
	}

	int uid = GetCurSel(&m_listctrlTables) + 1;

	Redmoon::CDBTable * pTable = m_DB.FindTable( uid );
	if( pTable == NULL)
	{
		Alert(_T("%s find table failed"), __TFUNCTION__);
		return;
	}

	tstring strFileName;
	if(m_psSelectedDBInfo && m_psSelectedDBInfo->m_strPathCmdClassTable.size()>0)
		strFileName.append(m_psSelectedDBInfo->m_strPathCmdClassTable);

	strFileName.append(pTable->GetName());
	strFileName.append(_T(".h"));
	FILE *fp = _tfopen(strFileName.c_str(), _T("wt+"));
	if (fp == NULL) 
	{
		Alert(_T("%s %s file open failed"), __TFUNCTION__, strFileName.c_str());
		return;
	}

	_ftprintf(fp, _T("#pragma once\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("class %s\n"), pTable->GetName());
	_ftprintf(fp, _T("{\n"));

	_ftprintf(fp, _T("public:\n"));
	
	Redmoon::FieldIterator itr = pTable->GetBeginField();
	while( itr != pTable->GetEndField())
	{
		Redmoon::SFieldInfo *fieldinfo = *itr;
		if(fieldinfo->GetName())
		{
			_ftprintf(fp, _T("\t%s;\n"), Redmoon::GetSQLDBTypeString(fieldinfo->GetDataType(), fieldinfo->GetDataLength(), fieldinfo->GetName(), fieldinfo->GetNumericPrecision(), fieldinfo->GetNumericScale()).c_str());
		}
		++itr;
	}
	_ftprintf(fp, _T("\n"));

	_ftprintf(fp, _T("\tDEFINE_COMMAND_EX(%s, LPOLESTR(L\"select "), pTable->GetName());

	itr = pTable->GetBeginField();
	while(itr != pTable->GetEndField())
	{
		Redmoon::SFieldInfo *fieldinfo = *itr;
		if(fieldinfo->GetName())
		{
			_ftprintf(fp, _T("%s"), fieldinfo->GetName());
		}
		++itr;

		if(itr != pTable->GetEndField())
		{
			_ftprintf(fp, _T(", "));
		}
	}

	_ftprintf(fp, _T(" from %s\"))\n"), pTable->GetName());

	_ftprintf(fp, _T("\tBEGIN_COLUMN_MAP(%s)\n"), pTable->GetName());
	int nCnt = 1; //1부터 시작
	itr = pTable->GetBeginField();
	while( itr != pTable->GetEndField())
	{
		Redmoon::SFieldInfo *fieldinfo = *itr;
		if(fieldinfo->GetName())
		{
			_ftprintf(fp, _T("\t\tCOLUMN_ENTRY(%d, %s)\n"), nCnt, fieldinfo->GetName());
		}
		++itr;
		++nCnt;
	}
	_ftprintf(fp, _T("\tEND_COLUMN_MAP()\n"));
	
	_ftprintf(fp, _T("};\n"));

	_ftprintf(fp, _T("typedef CCommand< CAccessor<%s>, CRowset, CNoMultipleResults> QUERY_%s;"), pTable->GetName(), pTable->GetName());

	size_t nFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *str = new char[nFileSize + 1];
	ZeroMemory(str, nFileSize + 1);
	fread(str, nFileSize, 1, fp);

	fseek(fp, 0, SEEK_SET);

	CopyToClipBoard(GetSafeHwnd(), str);
	delete []str;

	fclose(fp);
	Alert(_T("Code Generate Success. Result[%s] Copy To ClipBoard."), strFileName.c_str());
}

void CDBTestDlg::OnNMDblclkListTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedButtonTableCodeGenerate();
	*pResult = 0;
}

void CDBTestDlg::OnBnClickedButtonRefreshTablelist()
{
	LoadTableList();
}

void CDBTestDlg::LoadProcedureList()
{
	if(!m_DB.IsConnected()) 
	{
		AfxMessageBox(_T("Not Connected DB"));
		return;
	}

	m_listCtrlSP.DeleteAllItems();
	m_listctrlSPParameters.DeleteAllItems();

	QUERY_SPList qSPList;
	
	HRESULT hr = qSPList.Open(m_DB.GetSession());
	if(S_OK != hr)
	{
		Alert(_T("Load CProcedures List Fail"));
		return;
	}
	
	while(qSPList.MoveNext()==S_OK)
	{
		m_listCtrlSP.InsertItem(m_listCtrlSP.GetItemCount(), qSPList.name);
	}

	if(m_listCtrlSP.GetItemCount()>0)
	{
		m_listCtrlSP.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CDBTestDlg::OnLvnItemchangedListSP(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if( pNMLV->iItem > -1 )
	{
		CString strSPName;
		strSPName = m_listCtrlSP.GetItemText(pNMLV->iItem, 0);

		TCHAR strTemp[300];
		_stprintf(strTemp, _T("* %s"), strSPName);
		::SetWindowText(::GetDlgItem(this->m_hWnd, IDC_STATIC_SPINFO), strTemp);

		LoadProcedureColumnInfo(strSPName);
		LoadProcedureParameterInfo(strSPName);
		UpdateData(TRUE);
	}
}

void CDBTestDlg::LoadProcedureColumnInfo(CString& strSPName)
{
	CProcedureColumns procedurColumn;
	HRESULT hr = procedurColumn.Open(m_DB.GetSession(), NULL, NULL, NULL, strSPName);
	if(S_OK != hr)
	{
		//select 로 반환하는게 없으면. 실패하므로 로그기록 안함.
		//Alert(_T("CProcedureColumns Open Fail"));
		return;
	}

	m_vProcedureColumns.clear();
	while(S_OK == procedurColumn.MoveNext())
	{	
		m_vProcedureColumns.push_back((CProcedureColumnInfo)procedurColumn);
	}
}

void CDBTestDlg::LoadProcedureParameterInfo(CString& strSPName)
{
	CProcedureParameters procedurParameter;
	HRESULT hr = procedurParameter.Open(m_DB.GetSession(), NULL, NULL, strSPName);
	if(S_OK != hr)
	{
		Alert(_T("CProcedureParameters Open Fail"));
		return;
	}

	m_listctrlSPParameters.DeleteAllItems();
	m_vProcedureParameters.clear();

	int i = 0;
	while(S_OK == procedurParameter.MoveNext())
	{	
		tstring strParamName = procedurParameter.m_szParameterName;
		strParamName.erase(strParamName.begin());
		m_listctrlSPParameters.InsertItem(m_listctrlSPParameters.GetItemCount(), strParamName.c_str());
		tstring strIOType = Redmoon::ParamTypeToString(procedurParameter.m_nType);
		tstring strDataType = Redmoon::GetSQLDBTypeString(procedurParameter.m_nDataType, procedurParameter.m_nMaxLength);
		m_listctrlSPParameters.SetItemText(i, 0, strParamName.c_str());
		m_listctrlSPParameters.SetItemText(i, 1, strDataType.c_str());
		m_listctrlSPParameters.SetItemText(i, 2, strIOType.c_str());
		m_vProcedureParameters.push_back((CProcedureParameterInfo)procedurParameter);
		i++;
	}
}

void CDBTestDlg::OnBnClickedButtonSpLoad()
{
	LoadProcedureList();
}

void CDBTestDlg::OnBnClickedButtonCmdTable()
{
	if(m_psSelectedDBInfo)	OpenFolder(m_psSelectedDBInfo->m_strPathCmdClassTable);
}

void CDBTestDlg::OnBnClickedButtonCmdSp()
{
	if(m_psSelectedDBInfo)	OpenFolder(m_psSelectedDBInfo->m_strPathCmdClassProcedure);
}

void CDBTestDlg::OnBnClickedButtonSpSelect()
{
	if(m_psSelectedDBInfo)	OpenFolder(m_psSelectedDBInfo->m_strPathSPSelect);
}

void CDBTestDlg::OnBnClickedButtonSpUpdate()
{
	if(m_psSelectedDBInfo)	OpenFolder(m_psSelectedDBInfo->m_strPathSPUpdate);
}

void CDBTestDlg::OnBnClickedButtonSpInsert()
{
	if(m_psSelectedDBInfo)	OpenFolder(m_psSelectedDBInfo->m_strPathSPInsert);
}

void CDBTestDlg::OnBnClickedButtonSpDelete()
{
	if(m_psSelectedDBInfo)	OpenFolder(m_psSelectedDBInfo->m_strPathSPDelete);
}

void CDBTestDlg::OpenFolder(tstring strPath)
{
	if(strPath.size()>0)
		::ShellExecute(NULL, _T("open"), strPath.c_str(), NULL, NULL, SW_SHOW);
}
void CDBTestDlg::OnNMDblclkListSp(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedButtonSpCodeGenerate();
	*pResult = 0;
}

void CDBTestDlg::GenerateSPSelect(Redmoon::CDBTable * pTable)
{
	//현재 선택된 필드 찾기
	std::set<int> setSelectedIndex;
	GetSelectedItemIndex(setSelectedIndex);

	tstring strSPName;
	tstring strFileName;

	strSPName.append(m_psSelectedDBInfo->m_strSPNamePrefix);
	strSPName.append(pTable->GetName());
	strSPName.append(m_psSelectedDBInfo->m_strSPNameSuffixSelect);

	//경로지정
	if(m_psSelectedDBInfo && m_psSelectedDBInfo->m_strPathSPSelect.size()>0)
		strFileName.append(m_psSelectedDBInfo->m_strPathSPSelect);

	strFileName.append(strSPName);
	strFileName.append(_T(".sql"));
	FILE *fp = _tfopen(strFileName.c_str(), _T("wt+"));
	if (fp == NULL) 
	{
		Alert(_T("%s SP Generate Fail. FileName[%s]"), __TFUNCTION__, strFileName.c_str());
		return;
	}

	_ftprintf(fp, _T("USE [%s]\n"), m_DB.GetDBName().c_str());
	_ftprintf(fp, _T("go\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("--DROP PROCEDURE [dbo].[%s]\n"), strSPName.c_str());
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("/*\n"));
	_ftprintf(fp, _T("declare @ret int\n"));
	_ftprintf(fp, _T("exec @ret = [dbo].[%s]\n"), strSPName.c_str());
	_ftprintf(fp, _T("select @ret\n"));
	_ftprintf(fp, _T("*/\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("CREATE PROCEDURE [dbo].[%s]"), strSPName.c_str());

	//파라미터
	if(setSelectedIndex.size()>0)
	{
		_ftprintf(fp, _T("(\n"));
		std::set<int>::iterator it;
		for(it=setSelectedIndex.begin(); it!=setSelectedIndex.end(); ++it)
		{
			CString strFieldName = m_listctrlFields.GetItemText(*it, 0);
			CString strFieldType = m_listctrlFields.GetItemText(*it, 1);
			if(it!=setSelectedIndex.begin())
				_ftprintf(fp, _T(",\n@p_%s %s"), strFieldName, strFieldType);
			else
				_ftprintf(fp, _T("@p_%s %s"), strFieldName, strFieldType);
		}
		_ftprintf(fp, _T("\n)\n"));
	}
	else
	{
		_ftprintf(fp, _T("\n"));
	}

	_ftprintf(fp, _T("AS\n"));
	_ftprintf(fp, _T("DECLARE\n"));
	_ftprintf(fp, _T("@m_ret int;\n"));
	_ftprintf(fp, _T("set @m_ret = 0;\n"));
	_ftprintf(fp, _T("BEGIN\n"));
	_ftprintf(fp, _T("\tSET NOCOUNT ON;\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("\tBEGIN TRY \n"));
	_ftprintf(fp, _T("\n"));

	_ftprintf(fp, _T("\t\tselect "));

	Redmoon::FieldIterator itr = pTable->GetBeginField();
	while( itr != pTable->GetEndField())
	{
		Redmoon::SFieldInfo *fieldinfo = *itr;
		if(fieldinfo->GetName())
		{
			if(itr!=pTable->GetBeginField())
				_ftprintf(fp, _T(", %s"), fieldinfo->GetName());
			else 
				_ftprintf(fp, _T("%s"), fieldinfo->GetName());
		}
		++itr;
	}
	_ftprintf(fp, _T("\n"));

	_ftprintf(fp, _T("\t\t  from %s\n"), pTable->GetName());

	//where절
	if(setSelectedIndex.size()>0)
	{
		std::set<int>::iterator it;
		for(it=setSelectedIndex.begin(); it!=setSelectedIndex.end(); ++it)
		{
			CString strFieldName = m_listctrlFields.GetItemText(*it, 0);
			CString strFieldType = m_listctrlFields.GetItemText(*it, 1);
			if(it!=setSelectedIndex.begin())
				_ftprintf(fp, _T("\t\t   and %s = @p_%s\n"), strFieldName, strFieldName);
			else
				_ftprintf(fp, _T("\t\t where %s = @p_%s\n"), strFieldName, strFieldName);
		}
		_ftprintf(fp, _T("\n"));
	}

	_ftprintf(fp, _T("\tEND TRY\n"));
	_ftprintf(fp, _T("\tBEGIN CATCH\n"));
	_ftprintf(fp, _T("\tEND CATCH\n"));
	_ftprintf(fp, _T("\tRETURN @m_ret;\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("END\n"));
	_ftprintf(fp, _T("GO\n"));

	size_t nFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *str = new char[nFileSize + 1];
	ZeroMemory(str, nFileSize + 1);
	fread(str, nFileSize, 1, fp);

	fseek(fp, 0, SEEK_SET);

	CopyToClipBoard(GetSafeHwnd(), str);
	delete []str;

	fclose(fp);
	Alert(_T("SP Generate Success. Result[%s] Copy To ClipBoard."), strFileName.c_str());
}

void CDBTestDlg::GenerateSPDelete(Redmoon::CDBTable * pTable)
{
	//현재 선택된 필드 찾기
	std::set<int> setSelectedIndex;
	GetSelectedItemIndex(setSelectedIndex);

	tstring strSPName;
	tstring strFileName;

	strSPName.append(m_psSelectedDBInfo->m_strSPNamePrefix);
	strSPName.append(pTable->GetName());
	strSPName.append(m_psSelectedDBInfo->m_strSPNameSuffixDelete);

	//경로지정
	if(m_psSelectedDBInfo && m_psSelectedDBInfo->m_strPathSPDelete.size()>0)
		strFileName.append(m_psSelectedDBInfo->m_strPathSPDelete);

	strFileName.append(strSPName);
	strFileName.append(_T(".sql"));
	FILE *fp = _tfopen(strFileName.c_str(), _T("wt+"));
	if (fp == NULL) 
	{
		Alert(_T("%s SP Generate Fail. FileName[%s]"), __TFUNCTION__, strFileName.c_str());
		return;
	}

	_ftprintf(fp, _T("USE [%s]\n"), m_DB.GetDBName().c_str());
	_ftprintf(fp, _T("go\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("--DROP PROCEDURE [dbo].[%s]\n"), strSPName.c_str());
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("/*\n"));
	_ftprintf(fp, _T("declare @ret int\n"));
	_ftprintf(fp, _T("exec @ret = [dbo].[%s]\n"), strSPName.c_str());
	_ftprintf(fp, _T("select @ret\n"));
	_ftprintf(fp, _T("*/\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("CREATE PROCEDURE [dbo].[%s]"), strSPName.c_str());

	//파라미터
	if(setSelectedIndex.size()>0)
	{
		_ftprintf(fp, _T("(\n"));
		std::set<int>::iterator it;
		for(it=setSelectedIndex.begin(); it!=setSelectedIndex.end(); ++it)
		{
			CString strFieldName = m_listctrlFields.GetItemText(*it, 0);
			CString strFieldType = m_listctrlFields.GetItemText(*it, 1);
			if(it!=setSelectedIndex.begin())
				_ftprintf(fp, _T(",\n@p_%s %s"), strFieldName, strFieldType);
			else
				_ftprintf(fp, _T("@p_%s %s"), strFieldName, strFieldType);
		}
		_ftprintf(fp, _T("\n)\n"));
	}
	else
	{
		_ftprintf(fp, _T("\n"));
	}

	_ftprintf(fp, _T("AS\n"));
	_ftprintf(fp, _T("DECLARE\n"));
	_ftprintf(fp, _T("@m_ret int;\n"));
	_ftprintf(fp, _T("set @m_ret = 0;\n"));

	_ftprintf(fp, _T("BEGIN\n"));
	_ftprintf(fp, _T("\tSET NOCOUNT ON;\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("\tBEGIN TRY \n"));
	_ftprintf(fp, _T("\t\tBEGIN TRAN \n"));
	_ftprintf(fp, _T("\n"));


	_ftprintf(fp, _T("\t\tdelete from %s\n"), pTable->GetName());
	//where절
	if(setSelectedIndex.size()>0)
	{
		std::set<int>::iterator it;
		for(it=setSelectedIndex.begin(); it!=setSelectedIndex.end(); ++it)
		{
			CString strFieldName = m_listctrlFields.GetItemText(*it, 0);
			CString strFieldType = m_listctrlFields.GetItemText(*it, 1);
			if(it!=setSelectedIndex.begin())
				_ftprintf(fp, _T("\t\t   and %s = @p_%s\n"), strFieldName, strFieldName);
			else
				_ftprintf(fp, _T("\t\t where %s = @p_%s\n"), strFieldName, strFieldName);
		}
		_ftprintf(fp, _T("\n"));
	}

	_ftprintf(fp, _T("\t\tCOMMIT TRAN \n"));
	_ftprintf(fp, _T("\tEND TRY\n"));
	_ftprintf(fp, _T("\tBEGIN CATCH\n"));
	_ftprintf(fp, _T("\t\tROLLBACK TRAN\n"));
	_ftprintf(fp, _T("\tEND CATCH\n"));
	_ftprintf(fp, _T("\tRETURN @m_ret;\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("END\n"));
	_ftprintf(fp, _T("GO\n"));

	size_t nFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *str = new char[nFileSize + 1];
	ZeroMemory(str, nFileSize + 1);
	fread(str, nFileSize, 1, fp);

	fseek(fp, 0, SEEK_SET);

	CopyToClipBoard(GetSafeHwnd(), str);
	delete []str;

	fclose(fp);
	Alert(_T("SP Generate Success. Result[%s] Copy To ClipBoard."), strFileName.c_str());
}

void CDBTestDlg::GenerateSPInsert(Redmoon::CDBTable * pTable)
{
	//현재 선택된 필드 찾기
	std::set<int> setSelectedIndex;
	GetSelectedItemIndex(setSelectedIndex);

	tstring strSPName;
	tstring strFileName;

	strSPName.append(m_psSelectedDBInfo->m_strSPNamePrefix);
	strSPName.append(pTable->GetName());
	strSPName.append(m_psSelectedDBInfo->m_strSPNameSuffixInsert);

	//경로지정
	if(m_psSelectedDBInfo && m_psSelectedDBInfo->m_strPathSPInsert.size()>0)
		strFileName.append(m_psSelectedDBInfo->m_strPathSPInsert);

	strFileName.append(strSPName);
	strFileName.append(_T(".sql"));
	FILE *fp = _tfopen(strFileName.c_str(), _T("wt+"));
	if (fp == NULL) 
	{
		Alert(_T("%s SP Generate Fail. FileName[%s]"), __TFUNCTION__, strFileName.c_str());
		return;
	}

	_ftprintf(fp, _T("USE [%s]\n"), m_DB.GetDBName().c_str());
	_ftprintf(fp, _T("go\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("--DROP PROCEDURE [dbo].[%s]\n"), strSPName.c_str());
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("/*\n"));
	_ftprintf(fp, _T("declare @ret int\n"));
	_ftprintf(fp, _T("exec @ret = [dbo].[%s]\n"), strSPName.c_str());
	_ftprintf(fp, _T("select @ret\n"));
	_ftprintf(fp, _T("*/\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("CREATE PROCEDURE [dbo].[%s]"), strSPName.c_str());

	//파라미터
	_ftprintf(fp, _T("(\n"));
	int nCount = 0;
	for(int itemIdx = 0; itemIdx < m_listctrlFields.GetItemCount(); ++itemIdx)
	{
		CString strFieldName  = m_listctrlFields.GetItemText(itemIdx, 0);
		CString strFieldType  = m_listctrlFields.GetItemText(itemIdx, 1);
		CString strIsIdentity = m_listctrlFields.GetItemText(itemIdx, 3);

		if(strIsIdentity.Compare(_T("N"))==0)
		{
			if(nCount!=0)
				_ftprintf(fp, _T(",\n@p_%s %s"), strFieldName, strFieldType);
			else
				_ftprintf(fp, _T("@p_%s %s"), strFieldName, strFieldType);
			++nCount;
		}
	}
	_ftprintf(fp, _T("\n)\n"));

	_ftprintf(fp, _T("AS\n"));
	_ftprintf(fp, _T("DECLARE\n"));
	_ftprintf(fp, _T("@m_ret int;\n"));
	_ftprintf(fp, _T("set @m_ret = 0;\n"));
	_ftprintf(fp, _T("BEGIN\n"));
	_ftprintf(fp, _T("\tSET NOCOUNT ON;\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("\tBEGIN TRY \n"));
	_ftprintf(fp, _T("\t\tBEGIN TRAN \n"));
	_ftprintf(fp, _T("\n"));

	_ftprintf(fp, _T("\t\tinsert into %s ("), pTable->GetName());
	//column
	nCount = 0;
	for(int itemIdx = 0; itemIdx < m_listctrlFields.GetItemCount(); ++itemIdx)
	{
		CString strFieldName  = m_listctrlFields.GetItemText(itemIdx, 0);
		CString strFieldType  = m_listctrlFields.GetItemText(itemIdx, 1);
		CString strIsIdentity = m_listctrlFields.GetItemText(itemIdx, 3);

		if(strIsIdentity.Compare(_T("N"))==0)
		{
			if(nCount!=0)
				_ftprintf(fp, _T(", %s"), strFieldName);
			else
				_ftprintf(fp, _T("%s"), strFieldName);
			++nCount;
		}
	}
	_ftprintf(fp, _T(")\n"));
	_ftprintf(fp, _T("\t\tvalues("));
	//values
	nCount = 0;
	for(int itemIdx = 0; itemIdx < m_listctrlFields.GetItemCount(); ++itemIdx)
	{
		CString strFieldName  = m_listctrlFields.GetItemText(itemIdx, 0);
		CString strFieldType  = m_listctrlFields.GetItemText(itemIdx, 1);
		CString strIsIdentity = m_listctrlFields.GetItemText(itemIdx, 3);

		if(strIsIdentity.Compare(_T("N"))==0)
		{
			if(nCount!=0)
				_ftprintf(fp, _T(", @p_%s"), strFieldName);
			else
				_ftprintf(fp, _T("@p_%s"), strFieldName);
			++nCount;
		}
	}
	_ftprintf(fp, _T(")\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("\t\tCOMMIT TRAN \n"));
	_ftprintf(fp, _T("\tEND TRY\n"));
	_ftprintf(fp, _T("\tBEGIN CATCH\n"));
	_ftprintf(fp, _T("\t\tROLLBACK TRAN\n"));
	_ftprintf(fp, _T("\tEND CATCH\n"));
	_ftprintf(fp, _T("\tRETURN @m_ret;\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("END\n"));
	_ftprintf(fp, _T("GO\n"));

	size_t nFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *str = new char[nFileSize + 1];
	ZeroMemory(str, nFileSize + 1);
	fread(str, nFileSize, 1, fp);

	fseek(fp, 0, SEEK_SET);

	CopyToClipBoard(GetSafeHwnd(), str);
	delete []str;

	fclose(fp);
	Alert(_T("SP Generate Success. Result[%s] Copy To ClipBoard."), strFileName.c_str());
}

void CDBTestDlg::GenerateSPUpdate(Redmoon::CDBTable * pTable)
{
	//현재 선택된 필드 찾기
	std::set<int> setSelectedIndex;
	GetSelectedItemIndex(setSelectedIndex);

	tstring strSPName;
	tstring strFileName;

	strSPName.append(m_psSelectedDBInfo->m_strSPNamePrefix);
	strSPName.append(pTable->GetName());
	strSPName.append(m_psSelectedDBInfo->m_strSPNameSuffixUpdate);

	//경로지정
	if(m_psSelectedDBInfo && m_psSelectedDBInfo->m_strPathSPUpdate.size()>0)
		strFileName.append(m_psSelectedDBInfo->m_strPathSPUpdate);

	strFileName.append(strSPName);
	strFileName.append(_T(".sql"));
	FILE *fp = _tfopen(strFileName.c_str(), _T("wt+"));
	if (fp == NULL) 
	{
		Alert(_T("%s SP Generate Fail. FileName[%s]"), __TFUNCTION__, strFileName.c_str());
		return;
	}

	_ftprintf(fp, _T("USE [%s]\n"), m_DB.GetDBName().c_str());
	_ftprintf(fp, _T("go\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("--DROP PROCEDURE [dbo].[%s]\n"), strSPName.c_str());
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("/*\n"));
	_ftprintf(fp, _T("declare @ret int\n"));
	_ftprintf(fp, _T("exec @ret = [dbo].[%s]\n"), strSPName.c_str());
	_ftprintf(fp, _T("select @ret\n"));
	_ftprintf(fp, _T("*/\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("CREATE PROCEDURE [dbo].[%s]"), strSPName.c_str());

	//파라미터
	_ftprintf(fp, _T("(\n"));
	int nCount = 0;
	for(int itemIdx = 0; itemIdx < m_listctrlFields.GetItemCount(); ++itemIdx)
	{
		CString strFieldName  = m_listctrlFields.GetItemText(itemIdx, 0);
		CString strFieldType  = m_listctrlFields.GetItemText(itemIdx, 1);
		CString strIsIdentity = m_listctrlFields.GetItemText(itemIdx, 3);

		if(nCount!=0)
			_ftprintf(fp, _T(",\n@p_%s %s"), strFieldName, strFieldType);
		else
			_ftprintf(fp, _T("@p_%s %s"), strFieldName, strFieldType);
		++nCount;
	}
	_ftprintf(fp, _T("\n)\n"));

	_ftprintf(fp, _T("AS\n"));
	_ftprintf(fp, _T("DECLARE\n"));
	_ftprintf(fp, _T("@m_ret int;\n"));
	_ftprintf(fp, _T("set @m_ret = 0;\n"));
	_ftprintf(fp, _T("BEGIN\n"));
	_ftprintf(fp, _T("\tSET NOCOUNT ON;\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("\tBEGIN TRY \n"));
	_ftprintf(fp, _T("\t\tBEGIN TRAN \n"));
	_ftprintf(fp, _T("\n"));

	_ftprintf(fp, _T("\t\tupdate %s set\n"), pTable->GetName());
	//column
	nCount = 0;
	for(int itemIdx = 0; itemIdx < m_listctrlFields.GetItemCount(); ++itemIdx)
	{
		CString strFieldName  = m_listctrlFields.GetItemText(itemIdx, 0);
		CString strFieldType  = m_listctrlFields.GetItemText(itemIdx, 1);
		CString strIsIdentity = m_listctrlFields.GetItemText(itemIdx, 3);

		if(strIsIdentity.Compare(_T("N"))==0 && setSelectedIndex.find(itemIdx)==setSelectedIndex.end()) //이거 굉장히 애매함... Identity만으론 안되지~
		{
			if(nCount!=0)
				_ftprintf(fp, _T(",\n\t\t       %s = @p_%s"), strFieldName, strFieldName);
			else
				_ftprintf(fp, _T("\t\t       %s = @p_%s"), strFieldName, strFieldName);
			++nCount;
		}
	}
	_ftprintf(fp, _T("\n"));
	//where절
	if(setSelectedIndex.size()>0)
	{
		std::set<int>::iterator it;
		for(it=setSelectedIndex.begin(); it!=setSelectedIndex.end(); ++it)
		{
			CString strFieldName = m_listctrlFields.GetItemText(*it, 0);
			CString strFieldType = m_listctrlFields.GetItemText(*it, 1);
			if(it!=setSelectedIndex.begin())
				_ftprintf(fp, _T("\t\t   and %s = @p_%s\n"), strFieldName, strFieldName);
			else
				_ftprintf(fp, _T("\t\t where %s = @p_%s\n"), strFieldName, strFieldName);
		}
	}
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("\t\tCOMMIT TRAN \n"));
	_ftprintf(fp, _T("\tEND TRY\n"));
	_ftprintf(fp, _T("\tBEGIN CATCH\n"));
	_ftprintf(fp, _T("\t\tROLLBACK TRAN\n"));
	_ftprintf(fp, _T("\tEND CATCH\n"));
	_ftprintf(fp, _T("\tRETURN @m_ret;\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("END\n"));
	_ftprintf(fp, _T("GO\n"));

	size_t nFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *str = new char[nFileSize + 1];
	ZeroMemory(str, nFileSize + 1);
	fread(str, nFileSize, 1, fp);

	fseek(fp, 0, SEEK_SET);

	CopyToClipBoard(GetSafeHwnd(), str);
	delete []str;

	fclose(fp);
	Alert(_T("SP Generate Success. Result[%s] Copy To ClipBoard."), strFileName.c_str());
}

void CDBTestDlg::GetSelectedItemIndex(std::set<int>& setSelectedIndex)
{
	setSelectedIndex.clear();
		
	//현재 선택된 필드 찾기
	for(int iIdx = 0; iIdx < m_listctrlFields.GetItemCount(); ++iIdx)
	{
		if(m_listctrlFields.GetCheck(iIdx))
		{
			setSelectedIndex.insert(iIdx);
		}
	}
}

void CDBTestDlg::OnLvnKeydownListFields(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if(pLVKeyDow->wVKey == VK_DELETE)
	{
		POSITION selectedPos = m_listctrlFields.GetFirstSelectedItemPosition();
		while(selectedPos)
		{
			m_listctrlFields.DeleteItem(m_listctrlFields.GetNextSelectedItem(selectedPos));
			selectedPos = m_listctrlFields.GetFirstSelectedItemPosition();
		}
	}
}

void CDBTestDlg::OnBnClickedButtonSpGenerateSelect()
{
	if(GetCurSel(&m_listctrlTables) < 0)
	{
		Alert(_T("Not Select Table"));
		return;
	}

	int uid = GetCurSel(&m_listctrlTables) + 1;

	Redmoon::CDBTable * pTable = m_DB.FindTable( uid );
	if( pTable )
	{
		GenerateSPSelect(pTable);
	}
}

void CDBTestDlg::OnBnClickedButtonSpGenerateInsert()
{
	if(GetCurSel(&m_listctrlTables) < 0)
	{
		Alert(_T("Not Select Table"));
		return;
	}

	int uid = GetCurSel(&m_listctrlTables) + 1;

	Redmoon::CDBTable * pTable = m_DB.FindTable( uid );
	if( pTable )
	{
		GenerateSPInsert(pTable);
	}
}

void CDBTestDlg::OnBnClickedButtonSpGenerateUpdate()
{
	if(GetCurSel(&m_listctrlTables) < 0)
	{
		Alert(_T("Not Select Table"));
		return;
	}

	int uid = GetCurSel(&m_listctrlTables) + 1;

	Redmoon::CDBTable * pTable = m_DB.FindTable( uid );
	if( pTable )
	{
		GenerateSPUpdate(pTable);
	}
}

void CDBTestDlg::OnBnClickedButtonSpGenerateDelete()
{
	if(GetCurSel(&m_listctrlTables) < 0)
	{
		Alert(_T("Not Select Table"));
		return;
	}

	int uid = GetCurSel(&m_listctrlTables) + 1;

	Redmoon::CDBTable * pTable = m_DB.FindTable( uid );
	if( pTable )
	{
		GenerateSPDelete(pTable);
	}
}

void CDBTestDlg::OnBnClickedButtonCodeSpselect()
{
	if(GetCurSel(&m_listctrlTables) < 0)
	{
		Alert(_T("Not Select Table"));
		return;
	}

	int uid = GetCurSel(&m_listctrlTables) + 1;

	Redmoon::CDBTable * pTable = m_DB.FindTable( uid );
	if( pTable == NULL)
	{
		Alert(_T("%s find table failed"), __TFUNCTION__);
		return;
	}

	tstring strSPName;
	strSPName.append(m_psSelectedDBInfo->m_strSPNamePrefix);
	strSPName.append(pTable->GetName());
	strSPName.append(m_psSelectedDBInfo->m_strSPNameSuffixSelect);

	//현재 선택된 필드 찾기
	std::set<int> setSelectedIndex;
	GetSelectedItemIndex(setSelectedIndex);

	tstring strFileName;
	if(m_psSelectedDBInfo && m_psSelectedDBInfo->m_strPathCmdClassProcedure.size()>0)
		strFileName.append(m_psSelectedDBInfo->m_strPathCmdClassProcedure);

	strFileName.append(strSPName.c_str());
	strFileName.append(_T(".h"));
	FILE *fp = _tfopen(strFileName.c_str(), _T("wt+"));
	if (fp == NULL) 
	{
		Alert(_T("%s %s file open failed"), __TFUNCTION__, strFileName.c_str());
		return;
	}
		
	_ftprintf(fp, _T("#pragma once\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("class %s\n"), strSPName.c_str());
	_ftprintf(fp, _T("{\n"));

	_ftprintf(fp, _T("public:\n"));
	_ftprintf(fp, _T("\tLONG nRet;\n"));
	_ftprintf(fp, _T("\n"));

	//입력파라미터 선언
	std::set<int>::iterator it;
	for(it=setSelectedIndex.begin(); it!=setSelectedIndex.end(); ++it)
	{
		CString strFieldName		= m_listctrlFields.GetItemText(*it, 0);
		CString strFieldDefineIN	= m_listctrlFields.GetItemText(*it, 5);

		_ftprintf(fp, _T("\t%s;\n"), strFieldDefineIN);

		if((strFieldDefineIN.Left(4)).CompareNoCase(_T("char"))==0)
		{
			_ftprintf(fp, _T("\tvoid Set%s(char* szIn){strcpy_s(in_%s, _countof(in_%s), szIn);}\n"), strFieldName, strFieldName, strFieldName);
		}
	}
	_ftprintf(fp, _T("\n"));

	//출력파라미터 선언
	for(int iIdx = 0; iIdx<m_listctrlFields.GetItemCount(); ++iIdx)			
	{
		CString strFieldName  = m_listctrlFields.GetItemText(iIdx, 0);
		CString strFieldDefineOut	= m_listctrlFields.GetItemText(iIdx, 4);

		_ftprintf(fp, _T("\t%s;\n"), strFieldDefineOut);
	}
	_ftprintf(fp, _T("\n"));

	_ftprintf(fp, _T("\tDEFINE_COMMAND_EX(%s, LPOLESTR(L\"exec ? = %s "), strSPName.c_str(), strSPName.c_str());
	//파라미터갯수만큼 ?를
	for(it=setSelectedIndex.begin(); it!=setSelectedIndex.end(); ++it)
	{
		if(it!=setSelectedIndex.begin())
			_ftprintf(fp, _T(", ?"));
		else
			_ftprintf(fp, _T("?"));
	}
	_ftprintf(fp, _T("\"))\n"));

	//파라미터bind
	_ftprintf(fp, _T("\tBEGIN_PARAM_MAP(%s)\n"), strSPName.c_str());
	_ftprintf(fp, _T("\t\tSET_PARAM_TYPE(DBPARAMIO_OUTPUT)\n"));
	_ftprintf(fp, _T("\t\tCOLUMN_ENTRY(1, nRet)\n"));
	//입력파라미터 bind
	if(setSelectedIndex.size()>0)
	{
		int nParamIdx = 2;//1은 nRet임
		_ftprintf(fp, _T("\t\tSET_PARAM_TYPE(DBPARAMIO_INPUT)\n"));
		std::set<int>::iterator it;
		for(it=setSelectedIndex.begin(); it!=setSelectedIndex.end(); ++it)
		{
			CString strFieldName  = m_listctrlFields.GetItemText(*it, 0);
			_ftprintf(fp, _T("\t\tCOLUMN_ENTRY(%d, in_%s)\n"), nParamIdx, strFieldName);
			++nParamIdx;
		}
	}
	_ftprintf(fp, _T("\tEND_PARAM_MAP()\n"));

	_ftprintf(fp, _T("\tBEGIN_COLUMN_MAP(%s)\n"), strSPName.c_str());
	//출력파라미터 bind
	int nParamIdx = 1;//1은 nRet임
	for(int iIdx = 0; iIdx<m_listctrlFields.GetItemCount(); ++iIdx)			
	{
		CString strFieldName  = m_listctrlFields.GetItemText(iIdx, 0);
		_ftprintf(fp, _T("\t\tCOLUMN_ENTRY(%d, %s)\n"), nParamIdx, strFieldName);
		++nParamIdx;
	}
	_ftprintf(fp, _T("\tEND_COLUMN_MAP()\n"));

	_ftprintf(fp, _T("};\n"));

	_ftprintf(fp, _T("typedef CCommand< CAccessor<%s>, CRowset, CNoMultipleResults> QUERY_%s;"), strSPName.c_str(), strSPName.c_str());

	size_t nFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *str = new char[nFileSize + 1];
	ZeroMemory(str, nFileSize + 1);
	fread(str, nFileSize, 1, fp);

	fseek(fp, 0, SEEK_SET);

	CopyToClipBoard(GetSafeHwnd(), str);
	delete []str;

	fclose(fp);
	Alert(_T("Code Generate Success. Result[%s] Copy To ClipBoard."), strFileName.c_str());
}

void CDBTestDlg::OnLvnItemchangedListSp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void CDBTestDlg::OnBnClickedButtonSpCodeGenerate()
{
	if(GetCurSel(&m_listCtrlSP) < 0)
	{
		Alert(_T("%s Not Select SP"), __TFUNCTION__);
		return;
	}

	CString strSPName, strFileName;
	if(m_psSelectedDBInfo && m_psSelectedDBInfo->m_strPathCmdClassProcedure.size() > 0)
		strFileName = m_psSelectedDBInfo->m_strPathCmdClassProcedure.c_str();

	strSPName = m_listCtrlSP.GetItemText(GetCurSel(&m_listCtrlSP), 0);
	strFileName += strSPName;
	strFileName += _T(".h");
	FILE *fp = _tfopen(strFileName.GetBuffer(), _T("wt+"));
	if (fp == NULL) 
	{
		Alert(_T("%s %s file open failed"), __TFUNCTION__, strFileName.GetBuffer());
		return;
	}

	_ftprintf(fp, _T("#pragma once\n"));
	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("class %s\n"), strSPName.GetBuffer());
	_ftprintf(fp, _T("{\n"));

	_ftprintf(fp, _T("public:\n"));

	for(STL_Vector_ProcedureParameters it = m_vProcedureParameters.begin(); it != m_vProcedureParameters.end(); ++it)
	{
		CProcedureParameterInfo *procedureparameters = &(*it);
		_ftprintf(fp, _T("\t%s;\n"), Redmoon::GetDBTypeString(procedureparameters->m_nDataType, procedureparameters->m_nMaxLength, procedureparameters->m_szParameterName).c_str());
	}

	_ftprintf(fp, _T("\n"));
	_ftprintf(fp, _T("\tDEFINE_COMMAND_EX(%s, LPOLESTR(L\"exec "), strSPName.GetBuffer());

	for(int i = 0; i < m_listctrlSPParameters.GetItemCount(); i++)
	{
		CString strIOType = m_listctrlSPParameters.GetItemText(i, 2);
		if(strIOType == _T("RETURNVALUE"))
		{
			_ftprintf(fp, _T("? = %s "), strSPName.GetBuffer());
		}
		else
		{
			_ftprintf(fp, _T("?"));
			_ftprintf(fp, _T("%s"), Redmoon::ParamTypePerAddonString(strIOType.GetBuffer()));
			if(i != m_listctrlSPParameters.GetItemCount() -1) //마지막꺼가 아니어야만 ,붙인다.
			{
				_ftprintf(fp, _T(","));
			}
		}
	}
	_ftprintf(fp, _T("\"))\n"));

	_ftprintf(fp, _T("\tBEGIN_PARAM_MAP(%s)\n"), strSPName.GetBuffer());
	int nCnt = 1; //1부터 시작
	for(int i = 0; i < m_listctrlSPParameters.GetItemCount(); i++)
	{
		CString strParam = m_listctrlSPParameters.GetItemText(i, 0);
		CString strIOType = m_listctrlSPParameters.GetItemText(i, 2);
		tstring strParamIO = Redmoon::ParamTypeToParamIOString(strIOType.GetBuffer());
		if(!strParamIO.empty())
		{
			_ftprintf(fp, _T("\t\tSET_PARAM_TYPE(%s)\n"), strParamIO.c_str());
			_ftprintf(fp, _T("\t\tCOLUMN_ENTRY(%d, %s)\n"), nCnt++, strParam.GetBuffer());
		}
	}
	_ftprintf(fp, _T("\tEND_PARAM_MAP()\n"));

	_ftprintf(fp, _T("};\n"));

	_ftprintf(fp, _T("typedef CCommand< CAccessor<%s>, CRowset, CNoMultipleResults> SP_%s;\r\n"), strSPName.GetBuffer(), strSPName.GetBuffer());

	size_t nFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *str = new char[nFileSize + 1];
	ZeroMemory(str, nFileSize + 1);
	fread(str, nFileSize, 1, fp);

	CopyToClipBoard(GetSafeHwnd(), str);
	delete []str;

	fclose(fp);
	Alert(_T("Code Generate Success. Result[%s] Copy To ClipBoard."), strFileName.GetBuffer());
}

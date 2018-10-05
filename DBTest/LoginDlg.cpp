// LoginDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DBTest.h"
#include "LoginDlg.h"
#include "Str.h"
#include "SocketCommon.h"

// CLoginDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLoginDlg, CDialog)

CLoginDlg::CLoginDlg(Redmoon::COleDBAccessor *accessor, CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
{
	m_pcAccessor = accessor;
}

CLoginDlg::~CLoginDlg()
{
}

BOOL CLoginDlg::OnInitDialog()
{
	WSADATA wsaData;
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(0 != nRet)
	{
		Alert(_T("WSAStartup Error [%d, %d]"), nRet, WSAGetLastError());
		return FALSE;
	}

	CDialog::OnInitDialog();
	InitListBox();

	WSACleanup();
	return TRUE;
}

void CLoginDlg::InitListBox()
{
	RECT	rect;
	m_ctrlDBList.GetClientRect( &rect );

	int width = rect.right - rect.left;

	m_ctrlDBList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_SORTASCENDING );
	m_ctrlDBList.InsertColumn( 0, _T("IP"),		LVCFMT_CENTER, static_cast<int>( width * 0.4f ) );
	m_ctrlDBList.InsertColumn( 1, _T("DBName"),	LVCFMT_CENTER, static_cast<int>( width * 0.23f ) );
	m_ctrlDBList.InsertColumn( 2, _T("Login"),	LVCFMT_CENTER, static_cast<int>( width * 0.23f ) );
	m_ctrlDBList.InsertColumn( 3, _T("PWD"),	LVCFMT_CENTER, static_cast<int>( width * 0.0f ) );
	m_ctrlDBList.InsertColumn( 4, _T("Port"),	LVCFMT_CENTER, static_cast<int>( width * 0.1f ) );
	m_ctrlDBList.InsertColumn( 5, _T("No"),		LVCFMT_CENTER, static_cast<int>( width * 0.0f ) );

	std::vector<SDBInfo>::iterator it;
	for(it=m_vectorDBInfoTemp.begin(); it!=m_vectorDBInfoTemp.end(); ++it)
	{
		SDBInfo& info = (*it);
		
		int iNewItemIndex = m_ctrlDBList.InsertItem(m_ctrlDBList.GetItemCount(), info.m_strIP.c_str());
		
		m_ctrlDBList.SetItemText(iNewItemIndex, 1, info.m_strDBName.c_str());
		m_ctrlDBList.SetItemText(iNewItemIndex, 2, info.m_strLogin.c_str());
		m_ctrlDBList.SetItemText(iNewItemIndex, 3, info.m_strPWD.c_str());
		m_ctrlDBList.SetItemText(iNewItemIndex, 4, info.m_strPort.c_str());
		m_ctrlDBList.SetItemText(iNewItemIndex, 5, info.m_strNo.c_str());
	}

	if(m_ctrlDBList.GetItemCount()>0)
	{
		m_ctrlDBList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DBNAME, m_editDBName);
	DDX_Control(pDX, IDC_EDIT_ID, m_editID);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_EDIT_PORT, m_editPort);
	DDX_Control(pDX, IDC_EDIT_NO,	m_editNo);

	DDX_Control(pDX, IDC_IPADDRESS, m_IPAddressCtrl);

	DDX_Control(pDX, IDC_LIST_DB, m_ctrlDBList);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_ctrlConnect);
}

BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CLoginDlg::OnBnClickedButtonConnect)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DB, &CLoginDlg::OnLvnItemchangedListDb)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DB, &CLoginDlg::OnNMDblclkListDb)
END_MESSAGE_MAP()


// CLoginDlg 메시지 처리기입니다.

void CLoginDlg::OnBnClickedButtonConnect()
{
	CString strPort, strID, strPassword, strDBName;
	m_editDBName.GetWindowText(strDBName);
	m_editID.GetWindowText(strID);
	m_editPassword.GetWindowText(strPassword);
	m_editPort.GetWindowText(strPort);
	m_editNo.GetWindowText(m_strNo);

	TCHAR strIp[20];
	DWORD dwIP=0;
	m_IPAddressCtrl.GetAddress( dwIP );
	dwIP = ntohl(dwIP);
	Redmoon::IPToTCHAR(dwIP, strIp);

	this->EnableWindow(FALSE);

	m_ctrlConnect.SetWindowText(_T("Connecting..."));

	if(Redmoon::ERROR_CODE_SUCCESS == m_pcAccessor->Connect(std::string(TCHAR_TO_CHAR(strDBName.GetBuffer())), std::string(TCHAR_TO_CHAR(strIp)), std::string(TCHAR_TO_CHAR(strID.GetBuffer())), 
		std::string(TCHAR_TO_CHAR(strPassword.GetBuffer())),  _ttoi(strPort.GetBuffer())))
	{
		OnOK();
	}
	else
	{
		AfxMessageBox(_T("Connect Fail"));
		OnCancel();
	}
	m_ctrlConnect.SetWindowText(_T("&Connect"));
	this->EnableWindow(TRUE);
}

void CLoginDlg::OnLvnItemchangedListDb(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if(pNMLV->iItem >-1 )
	{
		tstring strIP		= m_ctrlDBList.GetItemText(pNMLV->iItem, 0);
		tstring strDBName	= m_ctrlDBList.GetItemText(pNMLV->iItem, 1);
		tstring strLogin	= m_ctrlDBList.GetItemText(pNMLV->iItem, 2);
		tstring strPWD		= m_ctrlDBList.GetItemText(pNMLV->iItem, 3);
		tstring strPort		= m_ctrlDBList.GetItemText(pNMLV->iItem, 4);
		tstring strNo		= m_ctrlDBList.GetItemText(pNMLV->iItem, 5);

		m_IPAddressCtrl.SetWindowText(strIP.c_str());
		m_editDBName.SetWindowText(strDBName.c_str());
		m_editID.SetWindowText(strLogin.c_str());
		m_editPassword.SetWindowText(strPWD.c_str());
		m_editPort.SetWindowText(strPort.c_str());
		m_editNo.SetWindowText(strNo.c_str());
	}
	*pResult = 0;
}

void CLoginDlg::OnNMDblclkListDb(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedButtonConnect();
	*pResult = 0;
}

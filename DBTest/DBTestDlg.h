// DBTestDlg.h : 헤더 파일
//

#pragma once

#include "LoginDlg.h"
#include "OleDBAccessor.h"
#include <memory>
#include <vector>
#include "afxcmn.h"
#include "afxwin.h"
#include "Common.h"
#include <set>
#include "AutoPtr.h"

// CDBTestDlg 대화 상자
class CDBTestDlg : public CDialog
{
	std::vector<SDBInfo> m_vectorDBInfo;

	SDBInfo* m_psSelectedDBInfo;

	typedef std::vector<CProcedureParameterInfo>::iterator STL_Vector_ProcedureParameters;
	std::vector<CProcedureParameterInfo> m_vProcedureParameters;

	std::vector<CProcedureColumnInfo> m_vProcedureColumns;

// 생성입니다.
public:
	CDBTestDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	virtual ~CDBTestDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EDTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	Redmoon::CAutoPtr<CLoginDlg> m_pcLoginDlg;
	Redmoon::COleDBAccessor m_DB;

	void InitHeaderTableList();
	void InitHeaderFieldList();
	void InitHeaderSPList();
	void InitHeaderSPParameter();
	void ReadXml();

	void LoadTableList();
	void LoadProcedureList();
	void LoadProcedureParameterInfo(CString& strSPName);
	void LoadProcedureColumnInfo(CString& strSPName);

	void OpenFolder(tstring strPath);

	void GenerateSPSelect(Redmoon::CDBTable * pTable);
	void GenerateSPDelete(Redmoon::CDBTable * pTable);
	void GenerateSPInsert(Redmoon::CDBTable * pTable);
	void GenerateSPUpdate(Redmoon::CDBTable * pTable);

	void GetSelectedItemIndex(std::set<int>& setSelectedIndex);

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	//afx_msg void OnSize(
	//	UINT nType,
	//	int cx,
	//	int cy );

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	CListCtrl m_listctrlTables;
	CListCtrl m_listctrlFields;
	CListBox  m_listboxLog;

	CListCtrl m_listCtrlSP;
	CListCtrl m_listctrlSPParameters;

	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnNMDblclkListTable(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListTables(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonTableCodeGenerate();

	CStatic m_LableDBInfo;
	afx_msg void OnBnClickedButtonRefreshTablelist();
	afx_msg void OnLvnItemchangedListSP(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSpLoad();
	afx_msg void OnBnClickedButtonCmdTable();
	afx_msg void OnBnClickedButtonCmdSp();
	afx_msg void OnBnClickedButtonSpSelect();
	afx_msg void OnBnClickedButtonSpUpdate();
	afx_msg void OnBnClickedButtonSpInsert();
	afx_msg void OnBnClickedButtonSpDelete();
	afx_msg void OnNMDblclkListSp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownListFields(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSpGenerateSelect();
	afx_msg void OnBnClickedButtonSpGenerateInsert();
	afx_msg void OnBnClickedButtonSpGenerateUpdate();
	afx_msg void OnBnClickedButtonSpGenerateDelete();
	afx_msg void OnBnClickedButtonCodeSpselect();
	afx_msg void OnBnClickedButtonSpCodeGenerate();
	afx_msg void OnLvnItemchangedListSp(NMHDR *pNMHDR, LRESULT *pResult);
};

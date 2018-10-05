#pragma once
#include "afxwin.h"

#include "OleDBAccessor.h"
#include "afxcmn.h"
#include "DBCommon.h"
#include <vector>

// CLoginDlg 대화 상자입니다.

class CLoginDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoginDlg)

	Redmoon::COleDBAccessor* m_pcAccessor;

	CListCtrl m_ctrlDBList;
	CButton   m_ctrlConnect;

	std::vector<SDBInfo> m_vectorDBInfoTemp;//원본은 CDBTestDlg에 있음.

public:
	CLoginDlg(Redmoon::COleDBAccessor *accessor, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLoginDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LOGIN };

private:
	void InitListBox();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editDBName;
	CEdit m_editID;
	CEdit m_editPassword;
	CEdit m_editPort;
	CEdit m_editNo;

	CString m_strNo;

	CIPAddressCtrl m_IPAddressCtrl;

	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnLvnItemchangedListDb(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListDb(NMHDR *pNMHDR, LRESULT *pResult);

	void SetDBInfo(std::vector<SDBInfo> *pVector)
	{
		m_vectorDBInfoTemp.clear();
		copy(pVector->begin(), pVector->end(), back_inserter(m_vectorDBInfoTemp));
	}

	CString GetSelectNo(){return m_strNo;}
};

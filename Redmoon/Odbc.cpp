#include "Odbc.h"
#include "DBConnection.h"


namespace Redmoon
{

// COdbc
COdbc::COdbc(TCHAR* szName) : m_hStmt(SQL_NULL_HANDLE), m_dwTimeOut(-1), m_pcDBConnection(NULL), m_nResult(0)
{
	::memset(m_szQuery, 0, sizeof(m_szQuery));

	::memset(m_szErrMsg, 0, sizeof(m_szErrMsg));
	::memset(m_szSqlState, 0, sizeof(m_szSqlState));

	m_nColNum = 0;
	::memset(m_ColName, 0, sizeof(m_ColName));

	::_tcscpy(m_szName, szName);
}

COdbc::~COdbc()
{
	if( SQL_NULL_HANDLE != m_hStmt )
	{
		::SQLFreeStmt(m_hStmt, SQL_CLOSE);
		::SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);

		m_hStmt = SQL_NULL_HANDLE;
	}
}

void COdbc::Cleanup()
{
	if( SQL_NULL_HANDLE != m_hStmt )
	{
		::SQLFreeStmt(m_hStmt, SQL_CLOSE);
	}
}

void COdbc::SetSTMT(SQLHSTMT hStmt)
{
	if( SQL_NULL_HANDLE != m_hStmt )
	{
		::SQLFreeStmt(m_hStmt, SQL_CLOSE);
		::SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
	}

	m_hStmt = hStmt;
}

void COdbc::SetDBConnector(CDBConnection* pDBConnector)
{
	if( NULL != pDBConnector )
		m_pcDBConnection = pDBConnector;
}

void COdbc::SetTimeOut()
{
	SQLRETURN nRet;
	if( m_dwTimeOut != -1 )
	{		
		nRet = ::SQLSetStmtOption(m_hStmt, SQL_QUERY_TIMEOUT, m_dwTimeOut);

		if( !SQL_SUCCEEDED(nRet) )
		{
			m_dwTimeOut = (DWORD)-1;
		}
	}

	//Ret = (SQLSetStmtAttr(m_hStmt, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)m_dwTimeOut, SQL_IS_INTEGER));
	//if(Ret != SQL_SUCCESS)
	//{
	//	char szErrMsg[300];
	//	GetErrMsg2(szErrMsg, 300);
	//	Debug(_T("CDBConnector::Prepare() SQLSetStmtAttr() SQL_ATTR_QUERY_TIMEOUT error (%d) %s"), Ret, szErrMsg);
	//	m_bDBConnect = TRUE;
	//	return FALSE;
	//}
}

int COdbc::FindCol(const char* szColName)
{
	for(int i = 1; i < m_nColNum; ++i)
	{
		if (0 == ::_stricmp(szColName, (const char*)m_ColName[i]))
		{
			return i;
		}
	}
	return 0;
}

void COdbc::DescribeCol()
{
	SQLRETURN SqlRtn = ::SQLNumResultCols(m_hStmt, &m_nColNum);
	if (SQL_SUCCESS != SqlRtn)
	{
		GetErrMsg();

		LOG_INFO(_TEXT("SQLNumResultCols error (%d)"), SqlRtn);
		return;
	}

	if (0 == m_nColNum)
		return;

	if (m_nColNum > SQL_MAX_COLUMNS_IN_SELECT)
		return;

	for(int i = 1; i < m_nColNum; ++i)
		::SQLDescribeCol(m_hStmt, i, m_ColName[i], SQL_MAX_COLUMN_NAME_LEN, NULL, NULL, NULL, NULL, NULL);
}

void COdbc::Reconnect()
{
	if( NULL == m_pcDBConnection ) return;

	// DB컨넥터가 끊어졌을 경우 다시 연결한다.
	m_pcDBConnection->Reconnect();
}

void COdbc::GetErrMsg()
{
	SQLINTEGER nNativeError;
	SQLSMALLINT sLen;

	::memset(m_szErrMsg, 0, sizeof(m_szErrMsg));

	USHORT plm_SS_Line = 0;
	SDWORD plm_SS_Severity = 0;
	SQLSMALLINT  plm_cbSS_Procname;
	SQLCHAR  plm_SS_Procname [SQL_MAX_PROCEDURE_NAME_LEN];

	if( SQL_NO_DATA != ::SQLGetDiagRec(SQL_HANDLE_STMT, m_hStmt, 1, m_szSqlState, &nNativeError, m_szErrMsg, sizeof(m_szErrMsg), &sLen) )
	{
		::SQLGetDiagField(SQL_HANDLE_STMT, m_hStmt, 1, SQL_DIAG_SS_LINE, &plm_SS_Line, SQL_IS_INTEGER, NULL);
		::SQLGetDiagField(SQL_HANDLE_STMT, m_hStmt, 1, SQL_DIAG_SS_SEVERITY, &plm_SS_Severity, SQL_IS_INTEGER, NULL);
		::SQLGetDiagField(SQL_HANDLE_STMT, m_hStmt, 1, SQL_DIAG_SS_PROCNAME, &plm_SS_Procname, sizeof(plm_SS_Procname), &plm_cbSS_Procname);
	}
	if(0 == ::_tcscmp((const TCHAR*)m_szSqlState, _T("08S01")) || 10054 == nNativeError )
	{
		Reconnect();
	}
	else
	{
		Cleanup();
	}
}

BOOL COdbc::BindParameter(SQLUSMALLINT ParameterNumber, SQLSMALLINT InputOutputType,
								SQLSMALLINT ValueType, SQLSMALLINT ParameterType,
								SQLUINTEGER ColumnSize, SQLSMALLINT DecimalDigits,
								SQLPOINTER ParameterValuePtr, SQLINTEGER BufferLength, SQLINTEGER* StrLen_or_IndPtr)
{
	SQLRETURN nRet = ::SQLBindParameter(m_hStmt, ParameterNumber, InputOutputType, 
					 ValueType, ParameterType, 
					 ColumnSize, DecimalDigits,
					 ParameterValuePtr, BufferLength, StrLen_or_IndPtr);
	if( !SQL_SUCCEEDED(nRet) )
	{
		GetErrMsg();
		LOG_INFO(_TEXT("BindParameter error (%d) %s"), nRet, m_szErrMsg);
		return FALSE;
	}
	return TRUE;
}

int COdbcQuery::GetRowCount()
{
	SQLINTEGER iCount;
	SQLRETURN nRet = ::SQLRowCount(m_hStmt, &iCount);
	if( !SQL_SUCCEEDED(nRet) )
	{
		GetErrMsg();
		LOG_INFO(_TEXT("SQLRowCount error (%d)"), nRet);
		return 0;
	}
	return iCount;
}

BOOL COdbcQuery::Open()
{
	::SQLFreeStmt(m_hStmt, SQL_CLOSE);

	_BindParameters();

	SetTimeOut();

	SQLRETURN nRet = ::SQLExecute(m_hStmt);
	if( !SQL_SUCCEEDED(nRet) && SQL_NO_DATA != nRet )  // select할때 SQL_NO_DATA가 나오면 FALSE로 보자...
	{
		GetErrMsg();
		return FALSE;
	}

	return TRUE;
}

BOOL COdbcQuery::Fetch()
{
	SQLRETURN nRet = ::SQLFetch(m_hStmt);
	if( !SQL_SUCCEEDED(nRet) )
	{
		if( nRet != SQL_NO_DATA)
		{
			GetErrMsg();
		}
		return FALSE;
	}
	return TRUE;
}

/**
* 바인드 되는 컬럼값은 NULL이 되면 안된다.
* ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/odbc/htm/odbcusing_sqlbindcol.htm
*/
BOOL COdbcQuery::BindCol(SQLUSMALLINT colno, SQLSMALLINT TargetType, SQLPOINTER TargetValue, SQLINTEGER BufferLength, SQLINTEGER *StrLen_or_Ind)
{
	//결과를 돌려 받기 위해 바인딩 한다.
	SQLRETURN nRet = ::SQLBindCol(m_hStmt, colno, TargetType, TargetValue, BufferLength, StrLen_or_Ind);
	if( !SQL_SUCCEEDED(nRet) )
	{
		GetErrMsg();
		LOG_INFO(_TEXT("BindCol error (%d)"), nRet);
		return FALSE;
	}
	return TRUE;
}


BOOL COdbcSp::Open()
{
	::SQLFreeStmt(m_hStmt, SQL_CLOSE);

	_BindParameters();

	SetTimeOut();

	SQLRETURN nRet = ::SQLExecute(m_hStmt);
	if( !SQL_SUCCEEDED(nRet) && SQL_NO_DATA != nRet )
	{
		GetErrMsg();
		return FALSE;
	}

	do
	{
		nRet = ::SQLMoreResults(m_hStmt);
		if( !SQL_SUCCEEDED(nRet) && SQL_NO_DATA != nRet )
		{
			GetErrMsg();
			return FALSE;
		}

	} while( nRet != SQL_NO_DATA );

	Cleanup();

	return TRUE;
}

} //namespace Redmoon
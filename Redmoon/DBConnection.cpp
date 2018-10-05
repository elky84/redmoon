#include "DBConnection.h"

#include "Odbc.h"

namespace Redmoon
{

CDBConnection::CDBConnection(DWORD dwConnectionID) : m_dwConnectionID(dwConnectionID)
{
	m_hEnv = NULL;
	m_hDbc = NULL;
	
	m_bDBConnect = FALSE;
	m_bTransaction = FALSE;

	memset(m_szErrMsg, 0, sizeof(m_szErrMsg));

	OpenEnvironment();
}

CDBConnection::~CDBConnection()
{
	DisConnect();
	CloseEnvironment();
}

BOOL CDBConnection::OpenEnvironment()
{
	SQLRETURN nRet;
	// 환경 핸들을 할당하고 버전 속성을 설정한다.
	nRet = ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
	if( !SQL_SUCCEEDED(nRet) )
	{
		m_hEnv = SQL_NULL_HANDLE;
		return FALSE;
	}

	nRet = ::SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	if( !SQL_SUCCEEDED(nRet) )
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
		m_hEnv = SQL_NULL_HANDLE;
		return FALSE;
	}
	return TRUE;
}

void CDBConnection::CloseEnvironment()
{
	if( SQL_NULL_HANDLE != m_hEnv )
		::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
}

BOOL CDBConnection::Connect(const TCHAR* dsn, const TCHAR* uid, const TCHAR* pwd, const TCHAR* databasename, int port, const TCHAR* szLogFileName)
{
	TCSCPY(m_szDsn, _countof(m_szDsn), dsn);
	TCSCPY(m_szUID, _countof(m_szUID), uid);
	TCSCPY(m_szPWD, _countof(m_szPWD), pwd);
	TCSCPY(m_szDatabaseName, _countof(m_szDatabaseName), databasename);
	m_nPort = port;

	if( szLogFileName )
		TCSCPY(m_szLogFileName, _countof(m_szLogFileName), szLogFileName);
	else
		TCSCPY(m_szLogFileName, _countof(m_szLogFileName), _TEXT("c:\\log\\odbcquery.log"));

	m_bDBConnect = FALSE;

	if( SQL_NULL_HANDLE != m_hDbc )
		DisConnect();

	SQLRETURN nRet;
	// 연결 핸들을 할당하고 연결한다.
	if((nRet = ::SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc)) != SQL_SUCCESS)
	{
		LOG_INFO(_T("SQLAllocHandle SQL_HANDLE_DBC error (%d)"), nRet);
		return FALSE;
	}

	{
		// Set options to log long-running queries, including the file to use for the log.
		nRet = ::SQLSetConnectAttr
						 (m_hDbc,
						 SQL_COPT_SS_PERF_QUERY_LOG,
						 m_szLogFileName,
						 SQL_NTS);
	   // Set the long-running query interval (in milliseconds).  Note that for version 2.50 and 2.65
	   // drivers, this value is specified in seconds, not milliseconds.
	   nRet = ::SQLSetConnectAttr
						 (m_hDbc,
						 SQL_COPT_SS_PERF_QUERY_INTERVAL,
						 (SQLPOINTER)3000,
						 SQL_IS_UINTEGER);
	   // Start the long-running query log.
	   nRet = ::SQLSetConnectAttr
						 (m_hDbc,
						 SQL_COPT_SS_PERF_QUERY,
						 (SQLPOINTER)SQL_PERF_START,
						 SQL_IS_UINTEGER);
	}

	TCHAR szConnIn[1024];
	TCHAR szConnOut[1024];
	SQLSMALLINT nNumOut;

	//_stprintf_s(szConnect, _countof(szConnect), _T("DSN=%s;UID=%s;PWD=%s;PORT_NO=%d"), dsn, uid, pwd, port);
	// dsn에 직접 ip를 적어서 ODBC에 등록되어 있지 않아도 된다.
	STPRINTF(szConnIn, _T("DRIVER={SQL Server};SERVER=%s,%d;UID=%s;PWD=%s;Database=%s"), dsn, port, uid, pwd, databasename);
	nRet = ::SQLDriverConnect(m_hDbc,
								NULL,
								(SQLTCHAR*)LPCTSTR(szConnIn),
								SQL_NTS,
								(SQLTCHAR*)szConnOut,
								1024,
								&nNumOut,
								SQL_DRIVER_NOPROMPT);
	if( !SQL_SUCCEEDED(nRet) )
	{
		TCHAR szError[32];

		SQLTCHAR szSqlState[6];
		SQLINTEGER nNativeError;
		SQLSMALLINT sLen;

		::SQLGetDiagRec(SQL_HANDLE_DBC, m_hDbc, 1, szSqlState, &nNativeError, m_szErrMsg, sizeof(m_szErrMsg), &sLen);
		
		STPRINTF(szError, _T("%s"), szSqlState);
		if(0 == _tcscmp((const TCHAR*)szError, _T("08S01")) || 10054 == nNativeError )
			m_bDBConnect = FALSE;

		LOG_INFO(_TEXT("SQLConnect error (%d) (%s, %s, %s)"), nRet, dsn, uid, m_szErrMsg);

		return FALSE;
	}

	m_bDBConnect = TRUE;
	m_bTransaction	= TRUE;
	
	SQLSMALLINT nResult;
	SWORD nTxnCapable = 0;
	SQLGetInfo(m_hDbc, SQL_TXN_CAPABLE, &nTxnCapable, sizeof(nTxnCapable), &nResult);

	if( !SQL_SUCCEEDED(nRet) )
		m_bTransaction = FALSE;

	LOG_INFO(_TEXT("SQLConnect Success %s %s %s"), m_szDsn, m_szUID, m_szDatabaseName);

	return TRUE;	
}

/*
DSN : 호스트 IP 또는 호스트 이름 
UID : 권한 부여 이름 (사용자 ID) 
PWD : 권한 부여 이름에 대응하는 암호. 사용자 ID에 대한 암호가 없으면 아무것도 지정되지 않는다. 
CONNTYPE : 연결 방법 (1 : TCP/IP, 2 : UNIX DOMAIN, 3 : IPC) 
PORT_NO : 연결 포트 번호 
NLS_USE : 사용언어 지정 (US7ASCII : 영어, KO16KSC5601 : 한국어) 
BATCH : 일괄 처리 (batch processing) 모드로 연결을 생성 
TIMEOUT : 서버 연결 시도 시 기다리는 시간. 기본값은 3초이다. 
CONNECTION_TIMEOUT : 네트워크 불안정 시 select() 또는 poll() 에서 발생할 수 있는 blocking을 방지하기 위한 속성이다. 
DATE_FORMAT : 날짜 형식. 기본 날짜 형식은 YYYY/MM/DD HH:MI:SS이다. 
InConnectionString : DSN=192.168.1.11;UID=SYS;PWD=MANAGER;CONNTYPE=1;NLS_USE=KO16KSC5601;PORT_NO=20202;BATCH=ON;TIMEOUT=5;CONNECTION_TIMEOUT=10 
* Batch Processing 
연결이 생성된 후에 연결 속성을 변경하여 batch processing을 작동시키거나 정지 시킬 수 있다. 
(e.g.) SQLSetConnectAttr(dbc, SQL_ATTR_BATCH, (void*)SQL_BATCH_ON, 0); 
SQLSetConnectAttr(dbc, SQL_ATTR_BATCH, (void*)SQL_BATCH_OFF, 0); 
*/
void CDBConnection::DisConnect()
{
	if( m_bDBConnect && SQL_NULL_HANDLE != m_hDbc )
	{
		SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
		m_hDbc = SQL_NULL_HANDLE;
	
		m_bDBConnect = FALSE;
	}
}
	
void CDBConnection::Reconnect()
{
	DisConnect();

	if( Connect(m_szDsn, m_szUID, m_szPWD, m_szDatabaseName, m_nPort, m_szLogFileName) )
	{
		// 재접속에 성공했으면 다시 예약을 만든다.
		Reprepare();
	}

    static int count = 0;
	LOG_INFO(_TEXT("CDBConnection::Reconnect() (%d)"), count++);
}

BOOL CDBConnection::BeginTrans()
{
	if( !m_bTransaction ) return TRUE;	
	
	// AutoCommit를 끈다.
	SQLRETURN nRet;	
	nRet = ::SQLSetConnectAttr(m_hDbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0);

	if( !SQL_SUCCEEDED(nRet) )
		return FALSE;
	
	return TRUE;
}

void CDBConnection::EndTrans(BOOL bCommit)
{
	if( !m_bTransaction ) return;
	
	//SQLSMALLINT nTransaction;
	//nTransaction = bCommit ? SQL_COMMIT : SQL_ROLLBACK;	

	SQLEndTran(SQL_HANDLE_DBC, m_hDbc, bCommit ? SQL_COMMIT : SQL_ROLLBACK);

	// AutoCommit를 켠다.
	SQLSetConnectAttr(m_hDbc,SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_ON, 0);
}


SQLHSTMT CDBConnection::Prepare(TCHAR* query)
{
	SQLHSTMT hStmt;
	//명령 핸들을 할당한다.
	SQLRETURN nRet = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &hStmt);
	if( !SQL_SUCCEEDED(nRet) )
	{
		SQLTCHAR szSqlState[6];
		SQLINTEGER nNativeError;
		SQLSMALLINT sLen;

		nRet = SQLGetDiagRec(SQL_HANDLE_DBC, m_hDbc, 1, szSqlState, &nNativeError, m_szErrMsg, sizeof(m_szErrMsg), &sLen);
		
		LOG_INFO(_TEXT("CDBConnection::Prepare() SQLAllocHandle() error (%d) %s"), nRet, m_szErrMsg);
		return SQL_NULL_HANDLE;
	}

	nRet = SQLPrepare(hStmt, (SQLTCHAR* )query, SQL_NTS);
	if( !SQL_SUCCEEDED(nRet) )
	{
		SQLTCHAR szSqlState[6];
		SQLINTEGER nNativeError;
		SQLSMALLINT sLen;

		nRet = SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, szSqlState, &nNativeError, m_szErrMsg, sizeof(m_szErrMsg), &sLen);

		LOG_INFO(_TEXT("CDBConnection::Prepare() SQLPrepare() error (%d) %s"), nRet, m_szErrMsg);

		SQLFreeStmt(hStmt, SQL_CLOSE);
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return SQL_NULL_HANDLE;
	}

	return hStmt;
}

void CDBConnection::AddPrepare(COdbc* pcOdbc)
{
	if( NULL == pcOdbc ) return;

	pcOdbc->PrepareStatement();
	pcOdbc->SetDBConnector(this);
	SQLHSTMT hStmt = Prepare(pcOdbc->GetQuery());
	pcOdbc->SetSTMT( hStmt );

	pcOdbc->DescribeCol();

	m_stl_list_ODBC.push_back(pcOdbc);
}

void CDBConnection::Reprepare()
{
	STL_LIST_ODBC::iterator itr = m_stl_list_ODBC.begin();
	for(; m_stl_list_ODBC.end()!=itr; ++itr)
	{
		COdbc* pcOdbc = (*itr);
		if( NULL == pcOdbc ) continue;
		
		pcOdbc->PrepareStatement();
		pcOdbc->SetDBConnector(this);
		SQLHSTMT hStmt = Prepare(pcOdbc->GetQuery());
		pcOdbc->SetSTMT( hStmt );
		pcOdbc->DescribeCol();
	}
}

COdbc* CDBConnection::getOdbc(TCHAR* szName)
{
	STL_LIST_ODBC::iterator itr = m_stl_list_ODBC.begin();
	for(; m_stl_list_ODBC.end()!=itr; ++itr)
	{
		if( 0 == ::_tcscmp((*itr)->GetName(), szName) )
			return (*itr);
	}
	return NULL;
}

} //namespace Redmoon

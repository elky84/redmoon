#pragma once

#include "Common.h"

#pragma comment( lib, "odbc32.lib" )

#include <sql.h>
#include <sqlext.h>
#include <ODBCSS.H>

#include <tchar.h>

#include <list>

namespace Redmoon
{

class COdbc;

const int MAX_QUERY_BUF = 100000;
const int QUERY_TIMEOUT = 30;		//30초

class CDBConnection
{
public:
	SQLTCHAR m_szErrMsg[1024];

	SQLHENV m_hEnv;
	SQLHDBC m_hDbc;

	TCHAR m_szLogFileName[MAX_PATH];
	TCHAR m_szDatabaseName[128];
	TCHAR m_szDsn[128];
	TCHAR m_szUID[128];
	TCHAR m_szPWD[128];
	int m_nPort;

	DWORD m_dwConnectionID;

	BOOL m_bDBConnect;
	BOOL m_bTransaction;

	DWORD m_dwTimeOut;
	DWORD m_dwThreadID;

protected:
	typedef std::list<COdbc*>						STL_LIST_ODBC;
	STL_LIST_ODBC									m_stl_list_ODBC;

public:
	CDBConnection(DWORD dwConnectionID);
	virtual ~CDBConnection();

	BOOL OpenEnvironment();
	void CloseEnvironment();

	BOOL Connect(const TCHAR* dsn, const TCHAR* uid, const TCHAR* pwd, const TCHAR* databasename, int port, const TCHAR* szLogFileName);
	void Reconnect();
	void DisConnect();

	BOOL BeginTrans();
	void EndTrans(BOOL bCommit);

	SQLHSTMT Prepare(TCHAR* query);
	void AddPrepare(COdbc* pcOdbc);
	void Reprepare();
	COdbc* getOdbc(TCHAR* szName);

};

} //namespace Redmoon
#pragma once

#include "Common.h"

#include <sql.h>
#include <sqlext.h>
#include <ODBCSS.H>

namespace Redmoon
{

class CDBConnection;

class COdbc
{
protected:
	SQLHSTMT m_hStmt;
	TCHAR m_szName[32];
	TCHAR m_szQuery[2048];

	SQLTCHAR m_szSqlState[6];
	SQLTCHAR m_szErrMsg[1024];

	DWORD m_dwTimeOut;

	CDBConnection* m_pcDBConnection;

	int m_nResult;

	SQLSMALLINT m_nColNum;
	SQLTCHAR m_ColName[SQL_MAX_COLUMNS_IN_SELECT][SQL_MAX_COLUMN_NAME_LEN];	//컬럼의 이름들

public:
	COdbc(TCHAR* szName);
	virtual ~COdbc();

public:
	void SetSTMT(SQLHSTMT hStmt);
	void SetDBConnector(CDBConnection* pDBConnector);
	void SetQuery(TCHAR* query) {TCSCPY(m_szQuery, _countof(m_szQuery), query); }

	void SetTimeOut(DWORD dwTimeOut) { m_dwTimeOut = dwTimeOut; }
	void SetTimeOut();

	TCHAR* GetName() { return m_szName; }
	int GetResult() { return m_nResult; }
	TCHAR* GetQuery() { return m_szQuery; }
	SQLTCHAR* GetSqlState() { return m_szSqlState; }
	//SQLCHAR* GetErrMsg() { return m_szErrMsg; }

protected:
	void Reconnect();
	void GetErrMsg();
	BOOL BindParameter(SQLUSMALLINT ParameterNumber, SQLSMALLINT InputOutputType,
						SQLSMALLINT ValueType, SQLSMALLINT ParameterType,
						SQLUINTEGER ColumnSize, SQLSMALLINT DecimalDigits,
						SQLPOINTER ParameterValuePtr, SQLINTEGER BufferLength, SQLINTEGER* StrLen_or_IndPtr);

	// Query중에서 ? 인자에 매칭시킨다.
	template <typename T>
	BOOL BindParam(int pos, int inout, T& value)
	{
		return BindParameter(pos, inout, SQL_C_LONG, SQL_INTEGER, 10, 0, &value, sizeof(T), NULL);
	}

	BOOL BindParam(int pos, int inout, short& value)
	{
		return BindParameter(pos, inout, SQL_C_SHORT, SQL_SMALLINT, 5, 0, &value, sizeof(short), NULL);
	}

	BOOL BindParam(int pos, int inout, int& value)
	{
		return BindParameter(pos, inout, SQL_C_SLONG, SQL_INTEGER, 10, 0, &value, sizeof(int),  NULL);
	}

	BOOL BindParam(int pos, int inout, __int64& value)
	{
		return BindParameter(pos, inout, SQL_C_SBIGINT, SQL_BIGINT, 10, 0, &value, sizeof(__int64),  NULL);
	}

	BOOL BindParam(int pos, int inout, float& value)
	{
		return BindParameter(pos, inout, SQL_C_FLOAT, SQL_FLOAT, 7, 0, &value, sizeof(float), NULL);
	}

	BOOL BindParam(int pos, int inout, double& value)
	{
		return BindParameter(pos, inout, SQL_C_DOUBLE, SQL_DOUBLE, 15, 0, &value, sizeof(double), NULL);
	}

	BOOL BindParam(int pos, int inout, BYTE& value)
	{
		return BindParameter(pos, inout, SQL_C_UTINYINT, SQL_TINYINT, 3, 0, &value, sizeof(BYTE), NULL);
	}

	BOOL BindParam(int pos, int inout, WORD& value)
	{
		return BindParameter(pos, inout, SQL_C_USHORT, SQL_SMALLINT, 5, 0, &value, sizeof(WORD), NULL);
	}

	BOOL BindParam(int pos, int inout, TCHAR* value, int size)
	{
		return BindParameter(pos, inout, SQL_WCHAR, SQL_VARCHAR, size, 0, value,  size, NULL);
	}

public:
	int FindCol(const char* szColName);
	void DescribeCol();
	void Cleanup();

	CDBConnection* GetDB() { return m_pcDBConnection; }

	virtual BOOL Open() { return FALSE; }
	virtual BOOL Fetch() { return FALSE; }

public:
	virtual void PrepareStatement() = 0;
	virtual void _BindParameters() = 0;
};


// Select 쿼리문으로 연결할때 상속받아서 사용한다.
class COdbcQuery : public COdbc
{
	SQLINTEGER m_nValueLength[SQL_MAX_COLUMNS_IN_SELECT];
public:
	COdbcQuery(TCHAR* szName) : COdbc(szName) {}
	virtual ~COdbcQuery() {}

	virtual BOOL Open();
	virtual BOOL Fetch();

	int GetRowCount();

protected:
	BOOL BindCol(SQLUSMALLINT colno, SQLSMALLINT TargetType, SQLPOINTER TargetValue,
		SQLINTEGER BufferLength, SQLINTEGER *StrLen_or_Ind);

	// 결과값을 가져오는 Select문에서만 사용하므로 TBLBase에만 두었다.
	template <typename T>
	BOOL BindCol(int pos, T& store)
	{
		return BindCol(pos, SQL_C_LONG, &store, sizeof(T), &m_nValueLength[pos]);
	}

	BOOL BindCol(int pos, int& store)
	{
		return BindCol(pos, SQL_C_LONG, &store, sizeof(int), &m_nValueLength[pos]);
	}

	BOOL BindCol(int pos, __int64& store)
	{
		return BindCol(pos, SQL_C_SBIGINT, &store, sizeof(__int64), &m_nValueLength[pos]);
	}

	BOOL BindCol(int pos, float& store)
	{
		return BindCol(pos, SQL_C_FLOAT, &store, sizeof(int), &m_nValueLength[pos]);
	}

	BOOL BindCol(int pos, double& store)
	{
		return BindCol(pos, SQL_C_DOUBLE, &store, sizeof(int), &m_nValueLength[pos]);
	}

	BOOL BindCol(int pos, BYTE& store)
	{
		return BindCol(pos, SQL_C_UTINYINT, &store, sizeof(BYTE), &m_nValueLength[pos]);
	}

	BOOL BindCol(int pos, WORD& store)
	{
		return BindCol(pos, SQL_C_USHORT, &store, sizeof(WORD), &m_nValueLength[pos]);
	}

	BOOL BindCol(int pos, TCHAR* store, int size)
	{
		return BindCol(pos, SQL_WCHAR, store, size, &m_nValueLength[pos]);
	}
};


// COdbcSp(Store Procedure로 연결할때 상속받아서 사용한다.
class COdbcSp : public COdbc
{
protected:

public:
	COdbcSp(TCHAR* szName) : COdbc(szName) {}
	virtual ~COdbcSp() {}

	virtual BOOL Open();
};

} //namespace Redmoon
#pragma once

#include "Str.h"
#include "DBCommon.h"

namespace Redmoon
{

enum SQLDBTYPE{
	SQLDBTYPE_IMAGE = 34, 
	SQLDBTYPE_TEXT = 35, 
	SQLDBTYPE_UNIQUEIDENTIFIER = 36, 
	SQLDBTYPE_TINYINT = 48, 
	SQLDBTYPE_SMALLINT = 52, 
	SQLDBTYPE_INT = 56, 
	SQLDBTYPE_SMALLDATETIME = 58, 
	SQLDBTYPE_REAL = 59, 
	SQLDBTYPE_MONEY = 60, 
	SQLDBTYPE_DATETIME = 61, 
	SQLDBTYPE_FLOAT = 62, 
	SQLDBTYPE_SQL_VARIANT = 98, 
	SQLDBTYPE_NTEXT = 99, 
	SQLDBTYPE_BIT = 104, 
	SQLDBTYPE_DECIMAL = 106, 
	SQLDBTYPE_NUMERIC = 108, 
	SQLDBTYPE_SMALLMONEY = 122, 
	SQLDBTYPE_BIGINT = 127, 
	SQLDBTYPE_VARBINARY = 165, 
	SQLDBTYPE_VARCHAR = 167, 
	SQLDBTYPE_BINARY = 173, 
	SQLDBTYPE_CHAR = 175, 
	SQLDBTYPE_TIMESTAMP = 189, 
	SQLDBTYPE_NVARCHAR = 231, 
	SQLDBTYPE_NCHAR = 239, 
	SQLDBTYPE_XML = 241, 
};

/**
@brief DBType을 문자열로 변환해주는 유틸 클래스
*/
class CSQLDBTypeText
{
protected:
	/// DBType integer 값을 TCHAR형 문자열로 저장해두는 맵
	std::map<INT, TCHAR *>	m_stl_map_SQLDBTypeText;//MSSQL의 타입

private:

	/**
	@brief DBType Integer 값에 해당하는 SQLServerDB 타입 문자열을 맵에 저장하는 함수
	*/
	void InsertDBType(int nDBType, TCHAR* szStr)
	{
		m_stl_map_SQLDBTypeText[nDBType] = szStr;
	}

	/**
	@brief 생성자. DBType에 해당하는 문자열을 맵에 저장하는 작업을 한다.
	*/
	CSQLDBTypeText()
	{
		InsertDBType(34, _T("image"));
		InsertDBType(35, _T("text"));
		InsertDBType(36, _T("uniqueidentifier"));
		InsertDBType(48, _T("tinyint"));
		InsertDBType(52, _T("smallint"));
		InsertDBType(56, _T("int"));
		InsertDBType(58, _T("smalldatetime"));
		InsertDBType(59, _T("real"));
		InsertDBType(60, _T("money"));
		InsertDBType(61, _T("datetime"));
		InsertDBType(62, _T("float"));
		InsertDBType(99, _T("ntext"));
		InsertDBType(104, _T("bit"));
		InsertDBType(106, _T("decimal"));
		InsertDBType(108, _T("numeric"));
		InsertDBType(122, _T("smallmoney"));
		InsertDBType(127, _T("bigint"));
		InsertDBType(165, _T("varbinary"));
		InsertDBType(167, _T("varchar"));
		InsertDBType(173, _T("binary"));
		InsertDBType(175, _T("char"));
		InsertDBType(189, _T("timestamp"));
		InsertDBType(231, _T("nvarchar"));
		InsertDBType(239, _T("nchar"));
		InsertDBType(241, _T("xml"));
		InsertDBType(48, _T("BYTE"));
		InsertDBType(52, _T("short"));
		InsertDBType(56, _T("int"));
		InsertDBType(62, _T("float"));
		InsertDBType(104, _T("VARIANT_BOOL"));
		InsertDBType(106, _T("float"));
		InsertDBType(108, _T("float"));
		InsertDBType(127, _T("__int64"));
		InsertDBType(167, _T("char"));
		InsertDBType(175, _T("char"));
	}

public:

	///정적 변수로 만든 싱글턴 메소드다.
	STATIC_SINGLETON_PTR(CSQLDBTypeText);

	/**
	@brief DBType Integer 값에 해당하는 문자열을 맵에서 찾아 반환하는 함수
	*/
	TCHAR* GetDBTypeName(int nDBType)
	{
		std::map<INT,TCHAR*>::iterator itr = m_stl_map_SQLDBTypeText.find(nDBType);
		if(itr != m_stl_map_SQLDBTypeText.end())
		{
			return itr->second;
		}
		else
		{
			return _T("None");
		}
	}
};

class CDBTypeText
{
public:
	CDBTypeText()
	{
		TCHAR * pText;
#define INSERT_TEXT(a,b) pText = new TCHAR[_tcslen(_T(b))+1];\
		TCSCPY( pText, _tcslen(_T(b))+1, _T(b) );		\
		m_DBTypeText[a] = pText;

		INSERT_TEXT(DBTYPE_EMPTY,"EMPTY")
		INSERT_TEXT(DBTYPE_NULL,"NULL")	
		INSERT_TEXT(DBTYPE_I2,"SHORT")
		INSERT_TEXT(DBTYPE_I4,"LONG")
		INSERT_TEXT(DBTYPE_R4,"FLOAT")
		INSERT_TEXT(DBTYPE_R8,"DOUBLE")
		INSERT_TEXT(DBTYPE_CY,"LARGE_INTEGER")
		INSERT_TEXT(DBTYPE_DATE,"DATE")
		INSERT_TEXT(DBTYPE_BSTR,"BSTR")
		INSERT_TEXT(DBTYPE_IDISPATCH,"IDispatch")
		INSERT_TEXT(DBTYPE_ERROR,"SCODE")
		INSERT_TEXT(DBTYPE_BOOL,"VARIANT_BOOL")
		INSERT_TEXT(DBTYPE_VARIANT,"VARIANT")

		INSERT_TEXT(DBTYPE_IUNKNOWN,"IUnknown*")
		INSERT_TEXT(DBTYPE_DECIMAL,"DECIMAL")
		INSERT_TEXT(DBTYPE_UI1,"BYTE")
		INSERT_TEXT(DBTYPE_ARRAY,"SAFEARRAY *")
		INSERT_TEXT(DBTYPE_BYREF,"void *")
		INSERT_TEXT(DBTYPE_I1,"signed char")
		INSERT_TEXT(DBTYPE_UI2,"USHORT")
		INSERT_TEXT(DBTYPE_UI4,"ULONG")
		INSERT_TEXT(DBTYPE_I8,"LARGE_INTEGER")
		INSERT_TEXT(DBTYPE_UI8,"ULARGE_INTEGER")
		INSERT_TEXT(DBTYPE_GUID,"GUID")
		INSERT_TEXT(DBTYPE_VECTOR,"DBVECTOR")
		INSERT_TEXT(DBTYPE_RESERVED,"Reserved")
		INSERT_TEXT(DBTYPE_BYTES,"BYTE")
		INSERT_TEXT(DBTYPE_STR,"CHAR")
		INSERT_TEXT(DBTYPE_WSTR,"WCHAR")
		INSERT_TEXT(DBTYPE_UDT,"User Define")
		INSERT_TEXT(DBTYPE_DBDATE,"DBDATE")
		INSERT_TEXT(DBTYPE_DBTIME,"DBTIME")
		INSERT_TEXT(DBTYPE_DBTIMESTAMP,"DBTIMESTAMP")
		INSERT_TEXT(DBTYPE_HCHAPTER,"CHAPTER")
		INSERT_TEXT(DBTYPE_FILETIME,"FILETIME")
		INSERT_TEXT(DBTYPE_PROPVARIANT,"PROPVARIANT")
		INSERT_TEXT(DBTYPE_VARNUMERIC,"DB_VARNUMERIC")
	}

	~CDBTypeText()
	{
		for(std::map<INT,TCHAR*>::iterator it = m_DBTypeText.begin(); it != m_DBTypeText.end(); ++it)
		{
			SAFE_DELETE_ARRAY(it->second);
		}
	}
	
	TCHAR * GetTypeName( INT db_type )
	{
		std::map<INT,TCHAR*>::iterator it = m_DBTypeText.find( db_type );
		if( it != m_DBTypeText.end() )
			return it->second;
		else
			return _T("None");
	}

	///정적 변수로 만든 싱글턴 메소드다.
	STATIC_SINGLETON_PTR(CDBTypeText);

protected:
	std::map<INT, TCHAR *>	m_DBTypeText;
};

/**
@brief DBType Integer 값에 해당하는 문자열을 맵에서 찾아, 코드에서 사용할 문자열로 변환해 반환하는 함수.
@param nDBType 디비 타입
@param nDataLength 데이터 길이
@param nNumericPrecision 실수의 정밀도 (가수부. 비트 수를 의미한다)
@param nNumericScale 실수의 자리수
@return 변수의 데이터형을 반환
*/
inline tstring GetSQLDBTypeString(int nDBType, int nDataLength, INT32 nNumericPrecision = 0, INT32 nNumericScale = 0)	
{
	tstring str;
	switch(nDBType)
	{
	case SQLDBTYPE_VARCHAR:
	case SQLDBTYPE_CHAR:
		{
			tstringstream strStream;
			strStream << CSQLDBTypeText::InstancePtr()->GetDBTypeName(nDBType) << _T("[") << nDataLength+1 << _T("]");
			str = strStream.str().c_str();
		}
		break;
	default:
		str = CSQLDBTypeText::InstancePtr()->GetDBTypeName(nDBType);
		break;
	}
	return str;
}

/**
@brief DBType Integer 값에 해당하는 문자열을 맵에서 찾아, 코드에서 사용할 문자열로 변환해 변수 선언시 사용가능한 문자열로 반환하는 함수.
@param nDBType 디비 타입
@param nDataLength 데이터 길이
@param strName 변수명
@param nNumericPrecision 실수의 정밀도 (가수부. 비트 수를 의미한다)
@param nNumericScale 실수의 자리수
@param szPreFix 접두사
@return 변수 선언시 사용할 문자열
*/
inline tstring GetSQLDBTypeString(int nDBType, int nDataLength, tstring strName, INT32 nNumericPrecision = 0, INT32 nNumericScale = 0, TCHAR * szPreFix= _T(""))
{
	tstring str = CSQLDBTypeText::InstancePtr()->GetDBTypeName(nDBType);
	str.append(_T(" "));

	str.append(szPreFix);

	if(_tcsnicmp(strName.c_str(), _T("@"), 1) == 0)
		strName.erase(strName.begin());

	str.append(strName);

	switch(nDBType)
	{
	case SQLDBTYPE_VARCHAR:
	case SQLDBTYPE_CHAR:
		{
			tstringstream strStream;
			strStream << str.c_str() << _T("[") << nDataLength+1 << _T("]");
			return strStream.str().c_str();
		}
	default:
		return str;
	}
}

/**
@brief DBType Integer 값에 해당하는 문자열을 맵에서 찾아, DB타입을 선언가능한 문자열로 표현한다.
@param nDBType 디비 타입
@param nDataLength 데이터 길이
@param strName 변수명
@param nNumericPrecision 실수의 정밀도 (가수부. 비트 수를 의미한다)
@param nNumericScale 실수의 자리수
@return 변수 선언시 사용할 문자열
*/
inline tstring GetDBTypeString(int nDBType, int nDataLength, tstring strName, INT32 nNumericPrecision = 0, INT32 nNumericScale = 0)
{
	tstring str = CDBTypeText::InstancePtr()->GetTypeName(nDBType);
	str.append(_T(" "));

	if(_tcsnicmp(strName.c_str(), _T("@"), 1) == 0)
		strName.erase(strName.begin());

	str.append(strName);
	switch(nDBType)
	{
	case DBTYPE_STR:
	case DBTYPE_WSTR:
		{
			tstringstream strStream;
			strStream << str.c_str() << _T("(") << nDataLength << _T(")");
			str += strStream.str();
		}
	case DBTYPE_NUMERIC:
	case DBTYPE_DECIMAL:
		{
			tstringstream strStream;
			strStream << str.c_str() << _T("(") << nNumericPrecision<<_T(", ") << nNumericScale << _T(")");
			str += strStream.str();
		}
	}
	return str;
}

} //namespace Redmoon
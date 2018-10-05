#include "Common.h"
#include "OleDBAccessor.h"
#include "Str.h"
#include "AutoPtr.h"

namespace Redmoon
{

/**
@brief 생성자. Ole초기화를 해준다.
*/
COleDBAccessor::COleDBAccessor() : m_eDBConState(EN_CONSTATE_NONE)
{
	HRESULT hr = ::OleInitialize(NULL);
	if(S_OK != hr)
	{
		LOG_ERROR(_T("%s OleInitialize() Fail [%u]"), __TFUNCTION__, hr);
	}
}

/**
@brief 소멸자. Ole 릴리즈 작업과, 소유하고 있던 테이블 정보등을 삭제해준다.
*/
COleDBAccessor::~COleDBAccessor()
{
	_Release();
	::OleUninitialize();
}

/**
@brief 테이블 정보와 세션 릴리즈
*/
void COleDBAccessor::_Release()
{
	for(TableIterator it = m_stlext_hashmap_Tables.begin(); it != m_stlext_hashmap_Tables.end();)
	{
		CDBTable* table = it->second;
		it = m_stlext_hashmap_Tables.erase(it);
		delete table;
	}
	m_cOleDBSession.Close();
}

/**
@brief 해당 디비 전체 테이블 정보 불러오기.
*/
bool COleDBAccessor::LoadAllTable()
{
	CAutoPtr<CTables> pTableSet(new CTables);
	TCHAR	query[64];
	TCSCPY(query, strlen(query), _T("TABLE"));
	HRESULT hr = pTableSet->Open(GetSession(), NULL, NULL, NULL, query);
	if(S_OK != hr )
	{
		_ErrorTrace(hr );
		LOG_INFO(_T("Open Tables : Fail"));
		return false;
	}

	INT32 nIdx = 1;
	while(S_OK == pTableSet->MoveNext())
	{
		_LoadTable(nIdx++, IID_TABLE, pTableSet.Get());
	}

	pTableSet->Close();

	TCSCPY(query, _countof(query), _T("VIEW"));
	hr = pTableSet->Open(GetSession(), NULL, NULL, NULL, query);
	if(S_OK != hr )
	{
		_ErrorTrace(hr );
		LOG_INFO(_T("Open VIEW : Fail"));
		return false;
	}

	while(S_OK == pTableSet->MoveNext() )
	{
		_LoadTable(nIdx++, IID_VIEW, pTableSet.Get());
	}

	return true;
}

/**
@brief 디비 연결 함수
@param dbname 디비이름
@param ip 디비 서버 아이피
@param account 디비 계정
@param password 디비 계정 비밀 번호
@param port 디비 접속 포트
@return 디비 접속 에러 코드
*/
CONNECT_DB_ERROR_CODE COleDBAccessor::Connect(const std::string& dbname, const std::string& ip, const std::string& account, const std::string& password, INT16 port /* = DEFAULT_DB_PORT */)
{
	m_strConnectString << _T("PROVIDER=SQLOLEDB;SERVER=") << ip << _T(",") << port << _T(";Persist Security Info=True;UID=") << account << _T(";PWD=") << password << _T(";DATABASE=") << dbname;

	std::wstring strConnectString = TCHAR_TO_WCHAR(m_strConnectString.str().c_str());		

	CDataSource	src;
	// DB 연결
	HRESULT hr = src.OpenFromInitializationString(strConnectString.c_str());
	if(S_OK == hr )
	{
		if(IsConnected())
		{
			_Release();
		}

		// 세션 만들기
		hr = m_cOleDBSession.Open(src);
		if(S_OK == hr )
		{
			LOG_INFO(_T("DB Connect Success"));

			m_eDBConState = EN_CONSTATE_NORMAL;

			m_strDBName = dbname;
			m_strIP = ip;
			m_strAccount = account;
			m_strPassword = password;
			m_nPort = port;

			src.Close();
			return ERROR_CODE_SUCCESS;
		}
		else
		{
			_ErrorTrace(hr );
			LOG_INFO(_T("Fail : cannot create session"));
			return ERROR_CODE_DB_CANNOT_CREATE_SESSION;
		}
	}
	else
	{
		_ErrorTrace(hr);
		LOG_INFO(_T("OpenDB Connect Fail [%d] [%s, %s]"), hr, ip.c_str(), account.c_str());
		return ERROR_CODE_DB_CANNOT_CONNECT_DB;
	}
}

/**
@brief 에러 원인 추적 메소드
@param errhr 에러로 쓰인 result를 넘겨 받아, 그 result를 바탕으로 
*/
void COleDBAccessor::_ErrorTrace(HRESULT errhr )
{
	CDBErrorInfo ErrorInfo;
	ULONG        cRecords;
	HRESULT      hr;
	ULONG        i;

	CComBSTR     bstrDesc, bstrHelpFile, bstrSource;
	GUID         guid;
	DWORD        dwHelpContext;
	WCHAR        wszTableIdx[40];
	USES_CONVERSION;

	// If the user passed in an HRESULT then trace it
	if (errhr != S_OK)
	{
		LOG_INFO(_T("OLE DB Error Record dump for hr = 0x%x"), errhr );
	}

	LCID lcLocale = GetSystemDefaultLCID();

	hr = ErrorInfo.GetErrorRecords(&cRecords);
	if (FAILED(hr) && ErrorInfo.m_spErrorInfo == NULL)
	{
		LOG_INFO(_T("No OLE DB Error Information found: hr = 0x%x"), hr ) ;
	}
	else
	{
		for (i = 0; i < cRecords; i++)
		{
			hr = ErrorInfo.GetAllErrorInfo(i, lcLocale, &bstrDesc, &bstrSource, &guid,
				&dwHelpContext, &bstrHelpFile);
			if (FAILED(hr))
			{
				LOG_INFO(_T("OLE DB Error Record dump retrieval failed: hr = 0x%x"), hr);
				return;
			}
			StringFromGUID2(guid, wszTableIdx, sizeof(wszTableIdx) / sizeof(WCHAR));

			LOG_INFO(_T("Row #: %4d Source: \"%s\" Description: \"%s\" Help File: \"%s\" Help Context: %4d guid: %s"),
				i, OLE2T(bstrSource), OLE2T(bstrDesc), OLE2T(bstrHelpFile), dwHelpContext, OLE2T(wszTableIdx));

			bstrSource.Empty();
			bstrDesc.Empty();
			bstrHelpFile.Empty();
		}
	}
}

/**
@brief 테이블 불러오기 메소드
@param nTableIdx 테이블 인덱스
@param eIIDType 테이블 타입
@param pTable 테이블 클래스
*/
void COleDBAccessor::_LoadTable(INT32 nTableIdx, IID_TYPE eIIDType, CTables * pTable)
{
	CDBTable* pcDBTable = new CDBTable(nTableIdx, eIIDType, pTable->m_szName, pTable->m_szDescription);
	if(pcDBTable)
	{
		//CColumns * pCol = new CColumns;
		//HRESULT hr = pCol->Open(GetSession(), NULL, NULL, pTable->m_szName);
		QUERY_Columns * pCol = new QUERY_Columns;
		STRCPY(pCol->tableName, 256, TCHAR_TO_CHAR(pTable->m_szName));
		HRESULT hr = pCol->Open(GetSession());
		if(FAILED(hr) )
		{
			_ErrorTrace(hr);
		}
		else
		{
			while(S_OK == pCol->MoveNext())
			{
				pcDBTable->AddField(pCol);
			}
		}

		delete pCol;
		m_stlext_hashmap_Tables[nTableIdx] = pcDBTable;
	}
}

/**
@brief 테이블 찾기 메소드
@param nTableIdx 테이블 인덱스
@return 찾은 CDBTable 클래스
*/
CDBTable* COleDBAccessor::FindTable(INT32 nTableIdx)
{
	TableIterator itr = m_stlext_hashmap_Tables.find(nTableIdx);
	if(itr != m_stlext_hashmap_Tables.end() )
		return itr->second;
	else
		return NULL;
}

/**
@brief 쿼리 정상 실행 여부 확인 메소드
@param hr 쿼리 실행 결과 값을 통해 쿼리가 제대로 수행됐는지 확인하는 용도로 쓰인다
@return 쿼리 정상 실행 여부
*/
BOOL COleDBAccessor::ExecuteResult(HRESULT hr)
{
	if(FAILED(hr))
	{
		// ERROR: 쿼리 실행 실패
		LOG_ERROR(_T("Query Fail [%d]"), hr);
		_ErrorTrace(hr);
		Connect(m_strDBName, m_strIP, m_strAccount, m_strPassword, m_nPort);
		return FALSE;
	}
	return TRUE;
}

/**
@brief SP 정상 실행 여부 확인 메소드
@param ret SP 실행 결과 값을 통해 쿼리가 제대로 수행됐는지 확인하는 용도로 쓰인다
@return SP 정상 실행 여부
*/
BOOL COleDBAccessor::ExecuteSpResult(LONG ret)
{
	if( 0 <= ret )
	{
		return true;
	}
	return false;
}

} //namespace Redmoon
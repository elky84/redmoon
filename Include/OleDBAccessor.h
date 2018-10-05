#pragma once

#include "DBCommon.h"
#include "DBTable.h"
#include "SQLDBTypeText.h"

#include <hash_map>
#include <sstream>

namespace Redmoon
{

/**
@brief DB 연결과 모든 테이블/뷰의 정보 유지.
*/
class COleDBAccessor
{
protected:	
	/// DB 연결 상태정보.
	EN_CONSTATE	m_eDBConState;	

	///DB 세션 클래스
	ATL::CSession m_cOleDBSession;

	/// DB 이름
	std::string m_strDBName;

	/// DB 아이피
	std::string m_strIP;

	/// DB 접속 포트
	INT16 m_nPort;

	/// DB 계정
	std::string m_strAccount;

	/// DB 비밀번호
	std::string m_strPassword;

	/// DB 접속시 사용하는 문자열
	std::stringstream m_strConnectString;

	///key : Sequencial 한 식별용 idx, Value : 디비 테이블 정보
	typedef stdext::hash_map<INT32,CDBTable*> STLEXT_HASH_MAP_DBTABLE;

	///해당 디비에 존재하는 테이블들의 hash_map
	STLEXT_HASH_MAP_DBTABLE m_stlext_hashmap_Tables;

public:
	///테이블 Hash_map의 반복자 typedef 해놓은 것
	typedef STLEXT_HASH_MAP_DBTABLE::iterator TableIterator;

public:
	///생성자
	COleDBAccessor();

	///소멸자
	~COleDBAccessor();

	/// DB 연결함수.
	CONNECT_DB_ERROR_CODE Connect(const std::string& dbname, const std::string& ip, const std::string& account, const std::string& password, INT16 port = DEFAULT_DB_PORT);

	/// 테이블 로드 함수
	bool LoadAllTable();

	/**
	@brief 세션 얻어오는 메소드
	@return 현재 DBMS 제어에 사용된 세션
	*/
	const CSession&	GetSession() {return m_cOleDBSession;}

	/// 쿼리 정상 실행 여부 확인 메소드
	BOOL ExecuteResult(HRESULT hr);

	/// SP 정상 실행 여부 확인 메소드
	BOOL ExecuteSpResult(LONG ret);

	/// 테이블 찾기 메소드
	CDBTable* FindTable(INT32 nTableIdx);

	/**
	@brief 테이블 hash_map의 begin Iterator 반환 메소드
	@return begin iterator
	*/
	inline TableIterator GetBeginTable() {return m_stlext_hashmap_Tables.begin();}

	/**
	@brief 테이블 hash_map의 end Iterator 반환 메소드
	@return end iterator
	*/
	inline TableIterator GetEndTable() {return m_stlext_hashmap_Tables.end();}

	/**
	@brief DBMS와의 접속 상태 알아오는 메소드
	@return DBMS와의 접속 여부
	*/
	inline BOOL IsConnected(){return m_eDBConState == EN_CONSTATE_NORMAL ? TRUE : FALSE;}

	/**
	@brief 현재 접속된 디비의 IP 반환 메소드
	@return DBIP
	*/
	std::string GetIP(){return m_strIP;}

	/**
	@brief 현재 접속된 디비명 반환 메소드
	@return DBName
	*/
	std::string GetDBName(){return m_strDBName;}

private:

	/// 에러 원인 추적 메소드
	void _ErrorTrace(HRESULT errhr);

	/// 테이블 불러오기 메소드
	void _LoadTable(INT32 nTableIdx, IID_TYPE eIIDType, CTables* pTable);

	/// 테이블 정보와 세션 릴리즈
	void _Release();
};

} //namespace Redmoon
#pragma once

#include <list>
#include <map>
#include <atldbcli.h>
#include <atldbsch.h>

namespace Redmoon
{

#define	DEFAULT_DB_PORT 1433

///기본 쿼리에서 사용되는 CCommand 객체의 typedef형
typedef CCommand<CNoAccessor, CNoRowset, CNoMultipleResults> SimpleQuery;

enum CONNECT_DB_ERROR_CODE
{
	ERROR_CODE_SUCCESS = 0, //성공
	ERROR_CODE_DB_CANNOT_CONNECT_DB, //접속 실패	
	ERROR_CODE_DB_CANNOT_CREATE_SESSION, //세션 생성 실패
	ERROR_CODE_DB_CANNOT_READ_TABLE_INFO, //테이블 읽기 실패
};

enum EN_CONSTATE
{ 
	EN_CONSTATE_NONE = 0,		// 초기 상태
	EN_CONSTATE_NORMAL,		// 연결되었다.
};

enum IID_TYPE
{
	IID_NONE = -1,
	IID_TABLE = 0, //테이블
	IID_VIEW, //뷰
	IID_PROCEDURE, //프로시져
};

} //namespace Redmoon
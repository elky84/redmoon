#pragma once

//컴파일러 호환용 매크로 함수 헤더 파일
#include "Compatible.h"

namespace Redmoon
{

///절대 값 매크로
#define ABS(a)                (((a) < (0))?-(a):(a))

///값 교환 매크로
#define SWAP(a, b)            (a)^=(b)^=(a)^=(b)

///최대값 리턴 매크로
#define MAX(a,b)			  (((a) > (b)) ? (a) : (b))

///최소값 리턴 매크로
#define MIN(a,b)			  (((a) < (b)) ? (a) : (b))

///안전하게 변수 delete 하는 매크로
#define SAFE_DELETE(x)				if(x) delete x; x = NULL;

///안전하게 배열 delete 하는 매크로
#define SAFE_DELETE_ARRAY(x)		if(x) delete []x; x = NULL;

///안전하게 싱글턴 delete하는 매크로
#define SAFE_DELETE_SINGLETON(x)	if(x) delete x;

///안전하게 CThread 상속 받은 클래스를 delete하는 매크로
#define SAFE_DELETE_THREAD(x)	if(x) {x->Release(); delete x;}

///안전하게 릴리즈 하는 매크로
#define SAFE_RELEASE(x)				if(x) x->Release(); x = NULL;

///안전하게 DeleteObject하는 매크로
#define SAFE_DELTE_OBJECT(x)		if(x) DeleteObject(x);

///안전하게 CloseHandle하는 매크로
#define SAFE_CLOSE_HANDLE(x)		if(x != INVALID_HANDLE_VALUE) {CloseHandle(x);x = INVALID_HANDLE_VALUE;}

///배열 범위 검사용 매크로. unsigned 용
#define IS_ARRAY_ACCESS(var, maxval) (0 <= var && var < maxval) ? true : false

///유효한 값인지 검사해서 true나 false를 리턴
#define IS_VALID_VALUE(var, minval, maxval) (minval <= var && var < maxval ) ? true : false

///문자열이 있으면 문자열 포인터, 내용이 비어 있으면 NULL 반환하는 매크로
#define GET_LPSTR_OR_NULL(str) (_tcslen(str) ? str : NULL)

///함수 포인터 호출 매크로
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

///배열 갯수 계산 매크로
#ifndef _countof
	#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

/// Get 메소드 생성 매크로
#define GET_METHOD(ret,fname,var) \
public: \
	ret Get##fname( ) { return var; }

/// Get, Set 메소드 생성 매크로
#define GET_SET_METHOD(ret,fname,var) \
public: \
	void Set##fname( ret tmp ) { var = tmp; } \
	ret Get##fname( ) { return var; }

/// 문자열 Get 메소드 생성 매크로
#define GET_METHOD_STRING(ret,fname,var) \
public: \
	ret Get##fname( ) { return var; }

/// 문자열 Get, Set 메소드 생성 매크로
#define GET_SET_METHOD_STRING(ret,fname,var) \
public: \
	void Set##fname( ret tmp ) { ::_tcscpy_s(var, _countof(var), tmp); } \
	ret Get##fname( ) { return var; }


///is메소드 생성 매크로
#define IS_METHOD(ret,fname,var) \
public: \
	ret is##fname( ) { return var; }

///is,Set 메소드 생성 매크로
#define IS_SET_METHOD(ret,fname,var) \
public: \
	void Set##fname( ret tmp ) { var = tmp; } \
	ret is##fname( ) { return var; }


///Copy Constructor && Assignment Prevention
#define COPY_ASSIGNMENT_PREVENTION( cls ) \
private: \
	cls(const cls &); \
	cls &operator=(const cls &);

} //namespace Redmoon
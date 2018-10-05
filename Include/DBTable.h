#pragma once

#include "DBCommon.h"
#include "FieldInfo.h"
#include "OleDBColumns.h"

namespace Redmoon
{

typedef std::list<SFieldInfo*>::iterator FieldIterator;

/**
@brief 디비 테이블 정보를 담고 있는 클래스
*/
class CDBTable
{
public:
	/**
	@brief 생성자. 멤버들을 초기화 해준다.
	@param nTableIdx 테이블 고유 인덱스
	@param eIIDType 테이블 타입
	@param szTableName 테이블 이름
	@param szDescription 테이블 설명
	*/
	CDBTable(INT32 nTableIdx, IID_TYPE eIIDType, TCHAR* szTableName, TCHAR* szDescription) : m_nTableIdx(nTableIdx), m_eIIDType(eIIDType), m_szTableName(0), m_szDesciption(0)
	{
		size_t len = _tcslen(szTableName);
		m_szTableName = new TCHAR[len+1];
		TCSCPY(m_szTableName, sizeof(TCHAR) * (len + 1), szTableName);

		len = _tcslen(szDescription);
		m_szDesciption = new TCHAR[len+1];
		TCSCPY(m_szDesciption, sizeof(TCHAR) * (len + 1), szDescription);
	}

	/*
	@brief 소멸자.
	*/
	~CDBTable()
	{
		SAFE_DELETE_ARRAY(m_szTableName);
		SAFE_DELETE_ARRAY(m_szDesciption);

		std::list<SFieldInfo*>::iterator itr = m_Field.begin();
		while(itr != m_Field.end() )
		{
			delete (*itr);
			++itr;
		}

		m_Field.clear();
	}

	/*
	@brief 필드 갯수 알아오기
	@return 필드 갯수
	*/
	inline size_t GetFieldCount() {return m_Field.size();}

	/*
	@brief 테이블 인덱스 알아오기
	@return 테이블 식별값
	*/
	inline INT32 GetTableIdx() {return m_nTableIdx;}

	/*
	@brief 테이블 타입 알아오기
	@return 테이블 타입
	*/
	inline IID_TYPE GetType() {return m_eIIDType;}

	/*
	@brief 테이블 이름 알아오기
	@return 테이블 이름
	*/
	inline TCHAR* GetName() {return m_szTableName;}

	/*
	@brief 테이블 설명 알아오기
	@return 테이블 설명
	*/
	inline TCHAR* GetDesc() {return m_szDesciption;}

	/*
	@brief 필드 Begin Iterator
	*/
	FieldIterator GetBeginField() {return m_Field.begin();}

	/*
	@brief 필드 End Iterator
	*/
	FieldIterator GetEndField()	{return m_Field.end();}

	/*
	@brief 필드 추가 메소드
	@param pColumn 추가될 컬럼의 정보가 담긴 클래스
	*/
	void AddField(QUERY_Columns* pColumn)
	{
		m_Field.push_back(new SFieldInfo(CHAR_TO_TCHAR(pColumn->name), pColumn->system_type_id, pColumn->max_length, pColumn->precision, pColumn->scale, 
			                             (pColumn->is_identity==0 ? false : true) ));
	}

protected:
	///테이블 인덱스
	INT32 m_nTableIdx;

	///테이블 분류
	IID_TYPE m_eIIDType;

	///테이블 이름
	TCHAR* m_szTableName;

	///테이블 설명
	TCHAR* m_szDesciption;

	///필드 정보
	std::list<SFieldInfo*>	m_Field;
};

} //namespace Redmoon
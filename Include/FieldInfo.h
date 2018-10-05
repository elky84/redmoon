#pragma once

#include <string>

namespace Redmoon
{

/**
@brief 필드 정보 구조체
*/
struct SFieldInfo
{
private:

	///필드 이름
	TCHAR* m_szName;

	///데이터 타입
	BYTE	m_nDataType;

	///문자열일 때에만 사용
	INT32	m_nDataLength; 

	//숫자타입일때 열의 전체 자리수
	USHORT	m_nNumericPrecision;

	//숫자타입일때 열의 소수 자리수
	SHORT	m_nNumericScale;

	//자동증가 여부
	bool    m_bIdentity;

public:
	/**
	@brief 생성자. 입력받은 정보를 바탕으로 테이블 정보를 생성해준다.
	@param szName 필드명
	@param szDesc 필드 설명
	@param nDataType 데이터 타입
	@param nLength 데이터 길이
	*/
	SFieldInfo(TCHAR* szName, BYTE nDataType, INT32 nLength, INT32 nNumericPrecision, INT32 nNumericScale, bool bIdentity) 
		: m_szName(0), m_nDataType(nDataType), m_nDataLength(nLength), m_nNumericPrecision(nNumericPrecision), m_nNumericScale(nNumericScale), m_bIdentity(bIdentity)
	{
		size_t len = _tcslen(szName);
		m_szName = new TCHAR[len+1];
		TCSCPY(m_szName, sizeof(TCHAR) * (len + 1), szName);

		if(10000 < m_nDataLength)
			m_nDataLength = 10000;
	}

	/**
	@brief 소멸자. 동적할당했던 필드 문자열을 해제해준다.
	*/
	~SFieldInfo()
	{
		SAFE_DELETE_ARRAY(m_szName);
	}

	/**
	@brief 필드명 알아오기
	@return 필드명
	*/
	inline TCHAR* GetName(){return m_szName;}

	/**
	@brief 데이터 타입 알아오기
	@return 데이터 타입
	*/
	inline INT32 GetDataType(){return m_nDataType;}

	/**
	@brief 데이터 크기 (문자열일때만)
	@return 데이터 크기
	*/
	inline INT32 GetDataLength(){return m_nDataLength;}

	/**
	@brief 숫자타입일때 열의 전체 자리수
	@return 숫자타입일때 열의 전체 자리수
	*/
	inline USHORT GetNumericPrecision(){return m_nNumericPrecision;}

	/**
	@brief 숫자타입일때 열의 소수 자리수
	@return 숫자타입일때 열의 소수 자리수
	*/
	inline SHORT GetNumericScale(){return m_nNumericScale;}

	/**
	@brief identity 여부
	@return identity 여부
	*/
	inline bool IsIdentiry(){return m_bIdentity;}
};

} //namespace Redmoon
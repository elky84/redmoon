#pragma once

#include "Peer.h"

namespace Redmoon
{

/**
@brief CPeer의 확장 클래스. 일 대 다 관계를 위해 Idx와 AccKey가 추가된 클래스다. 
*/
class CPeerEx : public CPeer
{
	///식별 키 값 
	const int m_nIdx;

public:
	///생성자
	CPeerEx(const int& nIdx);

	///소멸자
	virtual ~CPeerEx();

	/**
	@brief 식별 키 값을 반환
	@return 식별 키 값
	*/
	const int GetIdx(){return m_nIdx;}

	///주기적으로 불려지는 함수
	virtual void Act();

	///접속 완료시 불려지는 메소드
	virtual void OnConnect();

	///접속 종료시 불려지는 메소드
	virtual void OnDisconnect();

	///데이터 수신시 불려지는 메소드
	virtual void OnReceive(int nLen, void* data);
};

} //namespace Redmoon
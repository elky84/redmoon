#include "PeerEx.h"

namespace Redmoon
{

/**
@brief 생성자. 식별용 두개의 키 값을 설정하고, 참조 카운트를 증가 시킨다.
*/
CPeerEx::CPeerEx(const int& nIdx) : m_nIdx(nIdx)
{
}

/**
@brief 소멸자
*/
CPeerEx::~CPeerEx()
{
}

/**
@brief 접속 완료시 불려지는 메소드. 원할 시 재정의 필요.
*/
void CPeerEx::OnConnect()
{
	LOG_INFO(_T("%s [%d]"), __TFUNCTION__, GetIdx());
	__super::OnConnect();
}

/**
@brief 접속 종료시 불려지는 메소드. 원할 시 재정의 필요.
*/
void CPeerEx::OnDisconnect()
{
	LOG_INFO(_T("%s [%d]"), __TFUNCTION__, GetIdx());
	__super::OnDisconnect();
}

/**
@brief 데이터 수신시 불려지는 메소드. 원할 시 재정의 필요.
@param nLen 수신온 데이터 크기
@param data 데이터 포인터
*/
void CPeerEx::OnReceive(int nLen, void* data)
{
	LOG_INFO(_T("%s [%d] [%d]"), __TFUNCTION__, nLen, GetIdx());
	return 	__super::OnReceive(nLen, data);;
}

/**
@brief 주기적으로 불려지는 메소드. 원할 시 재정의 해야함.
*/
void CPeerEx::Act()
{
	LOG_INFO(_T("%s [%d]"), __TFUNCTION__, GetIdx());
	__super::Act();
}
} //namespace Redmoon
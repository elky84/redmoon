#include "Peer.h"
#include "IOInterface.h"

namespace Redmoon
{

/**
@brief 생성자. 각 값을 초기화
@param pcIOInterface IO를 실제로 담당할 객체의 포인터
*/
CPeer::CPeer(CIoInterface* pcIOInterface) : m_pcIOInterface(pcIOInterface), m_pSockaddrIn(NULL)
{
}

/**
@param 소멸자. 암것도 안함.
*/
CPeer::~CPeer()
{

}

/**
@brief 접속 완료시 불려지는 메소드. 재정의 하라고 걍 별일 안한다.
*/
void CPeer::OnConnect()
{
	LOG_DEBUG(_T("%s"), __TFUNCTION__);
}

/**
@brief 접속 종료시 불려지는 메소드. 재정의 하라고 걍 별일 안한다.
*/
void CPeer::OnDisconnect()
{
	LOG_DEBUG(_T("%s"), __TFUNCTION__);
}

/**
@brief 데이터 수신시 불려지는 메소드. 재정의 하라고 걍 별일 안한다.
@param nLen 수신된 데이터 크기
@param pData 수신된 데이터 포인터
*/
void CPeer::OnReceive(int nLen, void* pData)
{
	LOG_DEBUG(_T("%s [%d, %p]"), __TFUNCTION__, nLen, pData);
}

/**
@brief 데이터 송신씨 사용하는 메소드.
@param nLen Receive된 데이터 길이
@param pData 데이터 포인터
@param nAddLen pAddData의 크기
@param pAddData 기록할 추가 데이터
@param 성공 여부
*/
bool CPeer::Send(const int nLen, const void* pData, const int nAddLen, const void* pAddData)
{
	LOG_DEBUG(_T("%s [%d, %p] [%d, %p]"), __TFUNCTION__, nLen, pData, nAddLen, pAddData);
	if(m_pcIOInterface)
	{
		return m_pcIOInterface->Send(nLen, pData, nAddLen, pAddData);
	}
	return false;
}

/**
@brief 주기적으로 불려지는 메소드.
*/
void CPeer::Act()
{
	LOG_DEBUG(_T("%s"), __TFUNCTION__);
}

/**
@brief 접속 종료 요청
*/
void CPeer::Disconnect()
{
	if(m_pcIOInterface)
	{
		m_pcIOInterface->Disconnect();
	}
}

/**
@brief IO 재가능해졌을 때 
*/
void CPeer::Reuse()
{
	if(m_pcIOInterface)
	{
		return m_pcIOInterface->Reuse();
	}
}

} //namespace Redmoon

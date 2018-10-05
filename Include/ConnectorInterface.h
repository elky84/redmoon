#pragma once

#include "SocketCommon.h"

namespace Redmoon
{

/**
@brief Connecotr의 공통된 기능을 사용하는 Interface 클래스.
*/
class CConnectorInterface
{
public:
	///생성자
	CConnectorInterface();

	///소멸자
	virtual ~CConnectorInterface();
	
	///접속 동작
	virtual bool Connect(SOCKET Socket, SOCKADDR_IN* sockaddrin);

	///접속중인지 여부를 반환하는 순수 가상 함수
	virtual BOOL isConnected() = 0;
	
	/**
	@brief 소켓 아직 사용중인지 여부
	@return 사용중인지 여부
	*/
	BOOL isRefreshing(){return m_bRefreshing;}

	/**
	@brief 소켓 재사용 대기 상태인지 여부
	*/
	void SetRefreshing(BOOL bRefreshing);

private:
	///현재 소켓 사용중인지 여부
	BOOL m_bRefreshing;
};

} //namespace Redmoon
#pragma once

#include "socketcommon.h"
#include "WinSockObject.h"
#include <stdio.h>
#include <stdarg.h>

namespace Redmoon
{

/**
@brief 소켓을 사용하는 클래스들의 공통된 기능을 가진 베이스 클래스.
*/
class CSocketInterface
{
	///윈속 초기화를 범용적으로 하기 위해, static으로 선언.
	static CWinSockObject m_WinSockObject;

protected:
	///소켓 핸들
	SOCKET m_Socket;

	///주소값 구조체
	SOCKADDR_IN m_SockAddrIn;

public:
	///생성자
	CSocketInterface();

	///소멸자
	virtual ~CSocketInterface();

	///소켓을 닫는 메소드
	bool CloseSocket();

	///소켓을 닫는 메소드
	bool CloseSocket(SOCKET& refSocket);

	/**
	@brief 소켓을 얻어오는 메소드
	@return 소켓의 참조자
	*/
	inline const SOCKET &GetSocket(){return m_Socket;}

	/**
	@brief 주소값 구조체를 얻어오는 메소드
	@return 주소값 구조체의 포인터
	*/
	inline SOCKADDR_IN* GetSockAddrIn(){return &m_SockAddrIn;}

protected:
	///소켓 생성 메소드. 순수 가상함수므로 재정의 반드시 해야함.
	virtual bool Socket() = 0;

};

} //namespace Redmoon

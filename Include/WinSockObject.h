#pragma once

#include "SocketCommon.h"

namespace Redmoon
{

/**
@brief 윈속 초기화에 쓰이는 클래스
*/
class CWinSockObject
{
public:
	/**
	@brief 생성자에서 윈속 초기화
	*/
	CWinSockObject()
	{
		WSADATA wsaData;
		int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if(0 != nRet)
		{
			ExitProcess(1);
		}
	}

	/**
	@brief 소멸자에서 윈속 클린업
	*/
	~CWinSockObject()
	{
		WSACleanup();
	}
};

} //namespace Redmoon

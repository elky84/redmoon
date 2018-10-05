#pragma once

#include "SocketCommon.h"
#include "Thread.h"

namespace Redmoon
{

class CIocpInterface;

/**
@brief IOCP의 IO동작을 하는 IOCP워커 클래스
*/
class CIocpWorker : public CThread
{
public:
	///생성자
	CIocpWorker(HANDLE hIOCP);

	///소멸자
	virtual ~CIocpWorker();

	///루프 메소드
	virtual void Act();

	///스레드 정지
	virtual void Stop();

private:
	///스레드에서 사용하는 IOCP핸들
	HANDLE m_hIOCP;
};

} //namespace Redmoon

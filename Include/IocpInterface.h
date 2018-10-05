#pragma once

#include "SocketCommon.h"
#include "IocpCommon.h"

namespace Redmoon
{

class CSession;

/**
@brief IOCP 공통된 기능을 묶은 인터페이스
@desc IOCP기능을 사용할때만 매니저를 생성한다 (스마트 싱글턴)
*/
class CIocpInterface
{
public:
	///생성자
	CIocpInterface();

	///소멸자
	virtual ~CIocpInterface();

	///접속 유지 검사
	virtual void AliveCheck() = 0;

	///반복 동작
	virtual void Act() = 0;

	///접속 끊기 동작	
	virtual void Disconnect(CSession* pcSession) = 0;

	///IO실패시 처리
	virtual void OnIoFailed(CSession* pcSession) = 0;

	///소켓 재사용 요청시 부르는 메소드
	virtual void Reuse(CSession* pcSession) = 0;

	///접속 해제 이벤트	
	virtual void OnDisconnect(CSession* pcSession) = 0;

	///접속 완료 이벤트
	virtual void OnConnected(CSession* pcSession) = 0;

	///소켓 재사용 가능시 불려지는 메소드
	virtual void OnReuse(CSession* pcSession) = 0;

	///접속/억셉트 실패시
	virtual void ConnectFailed(CSession* pcSession) = 0;

protected:
	///IOCP핸들 생성 메소드
	static HANDLE CreateIOCPHandle();

private:
	///IOCP핸들을 각 기능별로 공유할 것이기에 순수 가상함수로 만들어둔다.
	virtual HANDLE GetIOCP() = 0;
};

} //namespace Redmoon

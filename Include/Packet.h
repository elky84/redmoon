#pragma once

#include "SocketCommon.h"
#include "AutoPtr.h"

namespace Redmoon
{

class CSession;
class CBuffer;

/**
@brief 패킷을 큐에 넣는 클래스.
*/
class CPacket
{
public:
	///생성자
	CPacket(const PACKET_EVENT& ePacketEvent, CSession* pcSession, CBuffer* pcBuffer = NULL);

	///소멸자
	~CPacket();

	///패킷 처리 메소드
	void Proc();

	///패킷을 워커에 집어 넣는 메소드
	void Register();

	/**
	@brief 처리까지 걸린 시간을 반환하는 메소드
	@param dwCurrentTick 현재 시간
	@return 처리까지 걸린 시간
	*/
	DWORD GetElapsedTick(DWORD dwCurrentTick){return dwCurrentTick - m_dwCreationTick;}

private:
	///CSession 포인터
	CSession* m_pcSession;

	const PACKET_EVENT m_ePacketEvent;

	///데이터 담을 버퍼
	CAutoPtr<CBuffer> m_pcBuffer;

	///패킷 생성 틱
	DWORD m_dwCreationTick;
};

} //namespace Redmoon

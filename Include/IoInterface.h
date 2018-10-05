#pragma once

#include "SocketCommon.h"
#include "AutoPtr.h"

namespace Redmoon
{

class CPeer;

/**
@brief 소켓 IO 처리 하는 클래스들이 상속 받아야 할 베이스 클래스.
*/
class CIoInterface 
{
public:
	///생성자
	CIoInterface(CPeer* pcPeer);

	///소멸자
	virtual ~CIoInterface();

	///데이터 전송
	virtual bool Send(const unsigned int nLen, const void* pData, const unsigned int nAddLen = 0, const void* pAddData = 0) = 0;

	///데이터 수신
	virtual bool OnReceive(unsigned int nLen, void* data) = 0;

	///소켓 재사용 요청
	virtual void Reuse() = 0;

	///피어 설정
	void SetPeer(CPeer* pcPeer);

	/**
	@brief 피어 얻기
	@return 피어
	*/
	CPeer* GetPeer(){return m_pcPeer.Get();}

	///접속 끊기 동작
	virtual void Disconnect() = 0;

protected:
	///피어의 포인터를 가진다.
	CAutoPtr<CPeer> m_pcPeer;
};

}
#pragma once

#include "SocketInterface.h"
#include "SmartSingleton.h"
#include "EventWorker.h"
#include "CriticalSection.h"
#include "Buffer.h"
#include <queue>

namespace Redmoon
{

/**
@brief 이벤트 셀렉트 공통 기능을 묶은 인터페이스
*/
class CEventSelectInterface : public CSmartSingleton< CEventWorker<CEventSelectInterface> >
{
public:
	///한개 단위로 쪼개진 패킷을 queue에서 빼는 메소드
	void Pop();

	/**
	@brief 크리티컬 섹션 진입
	*/
	inline void Enter(){m_CriticalSection.Enter();}

	/**
	@brief 크리티컬 섹션 퇴장
	*/
	inline void Leave(){m_CriticalSection.Leave();}

protected:

	///생성자. 소켓을 받는다.
	CEventSelectInterface(const SOCKET& refSocket);

	///생성자. 소켓을 전달 받음.
	virtual ~CEventSelectInterface();


	///Send 이벤트시 불려질 가상 함수
	virtual bool OnSend() = 0;

	///Receive 이벤트시 불려질 가상 함수
	virtual bool OnReceive() = 0;

	///Close 이벤트시 불려질 가상 함수
	virtual void OnDisconnect();

	///Connect 이벤트시 불려질 가상 함수
	virtual void OnConnect();

	///패킷을 한개단위로 쪼개서 풀때 불려지는 가상 함수
	virtual bool OnReceive(unsigned int nLen, void* data) = 0;

	///EventWorker에 등록
	bool Register();

	///EventWorker에서 빼기
	bool Unregister();

	///한개 단위로 쪼개진 패킷을 queue에 담는 메소드
	void Push(CBuffer* pcBuffer);

	/**
	@brief 크리티컬 섹션 객체 반환하는 메소드
	@return 크리티컬 섹션 객체 참조자 반환
	*/
	CCriticalSection& GetCriticalSection(){return m_CriticalSection;}

private:
	///이벤트 워커여! 날 사용하시오!
	friend class CEventWorker;

	///소켓 참조자
	const SOCKET& m_refSocket;

	///EVENT 핸들 
	HANDLE m_hEvent;

	///패킷을 한개 단위로 쪼개서 담아둔 버퍼
	std::queue<CBuffer*> m_stl_queue_Buffer;

	/**
	@brief EVENT 핸들을 반환하는 메소드
	@return 사용중인 EVENT 핸들
	*/
	HANDLE GetEvent(){return m_hEvent;}

	/// 신호 왔을때 불려질 메소드
	void Proc();

	///크리티컬 섹션
	CCriticalSection m_CriticalSection;

};

} //namespace Redmoon
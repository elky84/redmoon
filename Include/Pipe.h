#pragma once

#include "Common.h"
#include "PipeContext.h"
#include "Peer.h"
#include "AutoPtr.h"
#include "IoInterface.h"
#include "Buffer.h"

namespace Redmoon
{

const int PIPE_BUF_SIZE = 1024;

/**
@brief 파이프 기능을 구현한 클래스. 클라이언트/서버의 공통 기능을 담당한다.
*/
class CPipe : public CIoInterface
{
	friend class CPipeContext;
public:
	///생성자.
	CPipe(const tstring& strName, CPeer* pcPeer = new CPeer);

	///소멸자
	virtual ~CPipe();

	///Disconnect 순수 가상 함수
	virtual void Disconnect() = 0;

	///주기적인 반복 동작
	virtual void Act() = 0;

	///데이터 전송
	virtual bool Send(const unsigned int nLen, const void* pData, const unsigned int nAddLen = 0, const void* pAddData = 0);

	///파이프 핸들 반환
	const HANDLE& GetPipe(){return m_hPipe;}

	/**
	@brief 리시브 버퍼의 헤드 버퍼 반환
	@return 리시브 버퍼의 헤드 버퍼
	*/
	char* GetRecvHeadBuffer(){return m_pcRecvBuffer->GetHeadBuffer();}

	/**
	@brief 접속 여부 플래그 값
	@return 접속 여부
	*/
	BOOL isConnected(){return m_bConnected;}

protected:
	/**
	@brief 재사용 트리거 중인지 여부
	@return 재사용 트리거 중 여부
	*/
	BOOL isRefreshing(){return m_bRefreshing;}

	/**
	@brief 접속 시도 중인지 여부
	@return 접속 시도중 여부
	*/
	BOOL isTryConnect(){return m_bTryConnect;}

	/**
	@brief 접속 시도 플래그값 설정 메소드
	@param bTryConnect 접속 시도 플래그 값
	*/
	void SetTryConnect(BOOL bTryConnect){m_bTryConnect = bTryConnect;}

	/**
	@brief 파이프 핸들 설정
	@param hPipe 파이프 핸들
	*/
	void SetPipe(HANDLE& hPipe){m_hPipe = hPipe;}

	/**
	@brief 파이프 이름 반환
	@return 파이프 이름
	*/
	const tstring& GetName(){return m_strName;}

	/**
	@brief 파이프 Accept 컨텍스트 반환
	@return Accept 컨텍스트
	*/
	CPipeContext* GetPipeContextAccept(){return &m_cPipeContextAccept;}

	/**
	@brief 파이프 Recv 컨텍스트 반환
	@return Recv 컨텍스트
	*/
	CPipeContext* GetPipeContextRecv(){return &m_cPipeContextRecv;}

	/**
	@brief 파이프 Send 컨텍스트 반환
	@return Send 컨텍스트
	*/
	CPipeContext* GetPipeContextSend(){return &m_cPipeContextSend;}

	///이벤트 워커에 등록 요청
	void Register();

	/// Receive 등록 요청
	bool Receive();

	/// 재사용 가능시 호출하는 메소드
	virtual void Reuse();

protected:
	/// 접속 완료시 이벤트 메소드
	virtual void OnConnect();

	/// Receive 완료시 이벤트 메소드
	virtual bool OnReceive(unsigned int nLen, void* pData);

	/// Send 완료시 이벤트 메소드
	virtual void OnSend(unsigned int nLen);

	/// 접속 해제시 이벤트 메소드
	virtual void OnDisconnect();

private:
	///접속 여부 플래그 값
	BOOL m_bConnected;

	///재사용중인 플래그 값
	BOOL m_bRefreshing;

	///접속 시도 여부 플래그 값
	BOOL m_bTryConnect;

	///파이프 핸들
	HANDLE m_hPipe;

	///파이프 이름
	tstring m_strName;

	///Connect용 파이프 컨텍스트
	CPipeContext m_cPipeContextAccept;

	///Recv용 파이프 컨텍스트
	CPipeContext m_cPipeContextRecv;

	///Send용 파이프 컨텍스트
	CPipeContext m_cPipeContextSend;

	///Send용 버퍼
	CAutoPtr<CBuffer> m_pcSendBuffer;

	///Recv용 버퍼
	CAutoPtr<CBuffer> m_pcRecvBuffer;

	///Send 동기화를 위한 크리티컬 섹션 객체
	CCriticalSection m_cSendLock;
};

} //namespace Redmoon
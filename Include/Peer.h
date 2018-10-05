#pragma once

#include "Common.h"
#include "SocketCommon.h"

namespace Redmoon
{

class CIoInterface;

/**
@brief IO를 담당하는 CPeer 클래스. 실질적인 처리보다는 사용자에게 이벤트를 전달하기 위한 클래스이다.
*/
class CPeer
{
protected:
	///실제로 IO를 처리해줄 IOInterface의 포인터
	CIoInterface* m_pcIOInterface;

	///소켓 주소 클래스.
	sockaddr_in* m_pSockaddrIn;

public:
	///생성자
	CPeer(CIoInterface* pcIOInterface = NULL);

	///소멸자
	virtual ~CPeer();

public:
	/**
	@brief 아이피를 얻어온다
	@return 아이피
	*/
	inline std::string GetIP(){return std::string(inet_ntoa(m_pSockaddrIn->sin_addr));}

	/**
	@brief IO처리자 등록한다.
	@param pcIOInterface IO처리자 포인터
	*/
	inline void SetIOInterface(CIoInterface* pcIOInterface){m_pcIOInterface = pcIOInterface;}

	/**
	@brief 주소 구조체를 설정한다
	@param sockaddrin_ptr 주소 구조체 포인터
	*/
	inline void SetSockAddrIn(sockaddr_in* sockaddrin_ptr){m_pSockaddrIn = sockaddrin_ptr;}

	/**
	@brief 주소 구조체를 얻어온다
	@return 주소 구조체
	*/
	inline sockaddr_in* GetSockAddrIn(){return m_pSockaddrIn;}

	///접속 완료시불려지는 메소드
	virtual void OnConnect();

	///접속 종료시 불려지는 메소드
	virtual void OnDisconnect();

	///주기적으로 불려지는 메소드 (그 주기는 설정하기 나름)
	virtual void Act();

	/**
	@brief 패킷을 읽어 완성된 패킷 1개 단위로 짜르는 메소드
	@param nLen 데이터 크기
	@param pData 데이터 포인터
	@return 유효한 패킷의 크기. 0이면 패킷이 아직 덜 온것, -1이면 비정상 패킷
	*/
	virtual int Verify(const unsigned int nLen, const void* pData){return nLen;}

	///패킷이 왔을때 불려지는 메소드
	virtual void OnReceive(int nLen, void* pData);

	///패킷을 보내는 메소드
	virtual bool Send(const int nLen, const void* pData, const int nAddLen = 0, const void* pAddData = 0);

	///접속 종료 요청 메소드
	void Disconnect();

	///재 사용시 요청
	void Reuse();
};

} //namespace Redmoon

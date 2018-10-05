#pragma once

#include "SocketCommon.h"
#include "IocpContext.h"
#include "IOInterface.h"
#include "TCPSocket.h"
#include "AutoPtr.h"

namespace Redmoon
{

class CPeerEx;
class CListener;
class CTick;
class CIocpInterface;

/**
@brief IOCP에서 사용되는 소켓 데이터 클래스
*/
class CSession : public CTCPSocket, public CIoInterface
{
public:
	///생성자. 인덱스와 Iocp Commander 포인터 
	CSession(const int nIdx, CIocpInterface* pcIocpInterface = NULL);

	///소멸자.
	~CSession();

	/**
	@brief IocpInterface 지정. 
	@param pcIocpInterface 
	*/
	void SetIocpInterface(CIocpInterface* pcIocpInterface)
	{
		m_pcIocpInterface = pcIocpInterface;
	}
	
	/**
	@brief 소켓 주소 구조체를 설정하는 메소드
	@param sockaddr 소켓 주소 구조체의 포인터
	*/
	void SetSockAddr(const SOCKADDR_IN* sockaddr)
	{
		m_SockAddrIn = *sockaddr;
		SetIP(inet_ntoa(m_SockAddrIn.sin_addr));
	}

	/**
	@brief 아이피를 설정하는 메소드
	@param szIP 설정할 아이피
	*/
	void SetIP(const char* szIP)
	{
		m_strIP = szIP;
	}

	/**
	@brief 아이피를 얻어오는 메소드
	@return 아이피
	*/
	std::string GetIP()
	{
		return m_strIP;
	}

	int GetPort()
	{
		return ntohs(m_SockAddrIn.sin_port);
	}

	/**
	@brief 접속중인지 여부를 얻어오는 메소드
	@return 접속중인지 여부
	*/
	BOOL isConnected(){return m_bConnected;}
	
	///현재 틱을 기준으로 유효한지 검사하는 메소드.
	virtual bool CheckTick(DWORD curtick);

	/// 기준 틱을 초기화한다.
	virtual void ResetTick();

	///접속 종료를 시도하는 메소드.
	virtual void Disconnect();

	///초기화 메소드
	void Initialize();

	///패킷을 송신하는 메소드.
	virtual bool Send(const unsigned int nLen, const void* data, const unsigned int nAddLen = 0, const void* pAddData = 0);

	///패킷을 받았을 때 불려지는 메소드.
	virtual bool OnReceive(unsigned int nLen, void* pData);

	///통신에 사용될 소켓을 지정하는 메소드.
	void SetSocket(const SOCKET &socket){m_Socket = socket;}
	
	/**
	@brief 사용될 소켓핸들을 얻어오는 메소드
	@return 소켓 핸들
	*/
	SOCKET GetSocket(){return m_Socket;}
	
	/**
	@brief 사용될 소켓의 참조자를 얻어오는 메소드
	@return 소켓 참조자
	*/
	const SOCKET& GetSocketRef(){return m_Socket;}

	/**
	@brief Send시에 Iocp에 등록하는 IocpContext를 얻어오는 메소드
	@return CIocpContext 포인터
	*/
	CIocpContext* GetSendIocpContext(){return m_pSendIocpContext.Get();}
	
	/**
	@brief Recv시에 Iocp에 등록하는 IocpContext를 얻어오는 메소드
	@return CIocpContext 포인터
	*/
	CIocpContext* GetRecvIocpContext(){return m_pRecvIocpContext.Get();}

	/**
	@brief Accept시에 Iocp에 등록하는 IocpContext를 얻어오는 메소드
	@return CIocpContext 포인터
	*/
	CIocpContext* GetConnectIocpContext(){return m_pConnectIocpContext.Get();}

	/**
	@brief Close시에 Iocp에 등록하는 IocpContext를 얻어오는 메소드
	@return CIocpContext 포인터
	*/
	CIocpContext* GetReuseIocpContext(){return m_pReuseIocpContext.Get();}

	///접속 완료시 불려지는 메소드
	void OnConnect();
	
	///접속 종료 신호시 불려지는 메소드
	void OnDisconnect();

	///접속 종료 처리를 하는 메소드
	void DisconnectProcess();

	///Send 완료시 불려지는 메소드
	void OnSend(unsigned int nLen);

	///소켓 재사용 가능시 불려지는 메소드
	void OnReuse();

	///Connect/Accept 시도 실패시
	void ConnectFailed();

	///소켓 재사용 하고 싶을 때 부르는 메소드
	void Reuse();

	///WSARecv를 걸어서, 다음 Recv이벤트때 GQCS에서 리턴되도록 등록하는 메소드
	bool Receive();
	
	///한개씩 쪼개진 패킷을 푸는 메소드.
	void PacketProcess(CBuffer* pcBuffer);
	
	///Send를 거는 메소드. Send가 완료 되면 GQCS에서 완료 했다는 신호가 온다.
	bool Send();

	///IO를 받아갈 Peer를 등록하는 메소드
	void SetPeer(CPeer* pcPeer);

	///피어를 제거하는 메소드
	void ClearPeer();

	/**
	@brief 소켓 인덱스를 반환하는 메소드
	@return 소켓 식별값
	*/
	const int GetIdx(){return m_nIdx;}
	
	///소켓을 닫기 요청 메소드.
	bool CloseSocket();

	/// Connect 플래그 세팅
	void SetConnected(BOOL bConnect);

private:
	///AliveCheck용 틱 클래스. 소유한다
	CAutoPtr<CTick> m_pcTick;

	///IP를 담고 있는 string
	std::string m_strIP;

	///Send걸려 있는 상태인지 여부
	BOOL m_bSending; 

	///접속 중인지 여부
	BOOL m_bConnected;

	///소켓 식별값. 재사용됨.
	const int m_nIdx;	

	///Iocp Commander 포인터
	CIocpInterface* m_pcIocpInterface;

	///Send시에 Iocp에 등록하는 IocpContext
	CAutoPtr<CIocpContext> m_pSendIocpContext;

	///Recv시에 Iocp에 등록하는 IocpContext
	CAutoPtr<CIocpContext> m_pRecvIocpContext;

	///Accept시에 Iocp에 등록하는 IocpContext
	CAutoPtr<CIocpContext> m_pConnectIocpContext;

	///Reuse시에 Iocp에 등록하는 IocpContext
	CAutoPtr<CIocpContext> m_pReuseIocpContext;

	///Send에 대한 Lock
	CCriticalSection m_cSendLock;
};

} //namespace Redmoon

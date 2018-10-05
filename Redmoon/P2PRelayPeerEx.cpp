#include "P2PRelayPeerEx.h"
#include "P2PRelay.h"

namespace Redmoon
{

/**
@brief 생성자. 패킷 함수 포인터를 설정하고 변수 값들을 초기화한다.
@param nIdx 식별 인덱스
@param nAccKey 식별 누적 키 값
*/
CP2PRelayPeerEx::CP2PRelayPeerEx(const int& nIdx) : CPeerEx(nIdx), m_pstl_vector_P2PRelayPeerEx(NULL), m_nReservationGroupNo(-1), m_bRecvInternalSockaddrin(false)
{
	Register(PRCOMMAND_RECV_INTERNAL_SOCKADDRIN, &CP2PRelayPeerEx::_RecvInternalSockAddrIn);
	Register(PRCOMMAND_ENTER, &CP2PRelayPeerEx::_Enter);
	Register(PRCOMMAND_EXIT, &CP2PRelayPeerEx::_Exit);
	Register(PRCOMMAND_COMMAND, &CP2PRelayPeerEx::_Command);
	Register(PRCOMMAND_PING, &CP2PRelayPeerEx::_Ping);
}

/**
@brief 소멸자. 안전하게 OnDisconnect이벤트를 호출해준다.
*/
CP2PRelayPeerEx::~CP2PRelayPeerEx()
{
	OnDisconnect();
}

/**
@brief 접속 완료 이벤트
*/
void CP2PRelayPeerEx::OnConnect()
{
	m_nReservationGroupNo = -1;
	m_bRecvInternalSockaddrin = false;
}

/**
@brief 접속 해제 이벤트. 참여중인 목록에서 뺀다.
*/
void CP2PRelayPeerEx::OnDisconnect()
{
	if(m_pstl_vector_P2PRelayPeerEx)
		GetP2PRelay()->Exit(this);
}

/**
@brief 완성된 패킷 처리 메소드
@param nLen 패킷 길이
@param pData 패킷 데이터 포인터
*/
void CP2PRelayPeerEx::OnReceive(int nLen, void* pData)
{
	PRHeader* header = ((PRHeader*)pData);
	PFTYPE pf = Get(header->GetCommand());
	if(pf)
	{
		Call(header, pf);
	}
	else
	{
		LOG_ERROR(_T("%s [%d] [%d, %d]"), __TFUNCTION__, GetIdx(), header->GetCommand(), header->GetLen());
	}
}

/**
@brief 수신 데이터를 분석해서 패킷으로 구성한다.
@param nLen 수신된 데이터 크기
@param pData 수신된 데이터 포인터
@return 패킷의 길이. 패킷으로 구성하지 못할 유효하지 않은 데이터면 0을 리턴한다.
*/
int CP2PRelayPeerEx::Verify(const unsigned int nLen, const void* pData)
{
	if(nLen < sizeof(PRHeader))
	{
		LOG_DEBUG(_T("%s [%d < %d]"), __TFUNCTION__, nLen, sizeof(PRHeader));
		return 0;
	}

	PRHeader* header = (PRHeader*)pData;
	if(nLen < header->GetLen()) //헤더에서의 길이가, 패킷 길이보다 작으면?
	{
		LOG_DEBUG(_T("%s [%d] [%d < %d]"), __TFUNCTION__, header->GetCommand(), nLen, header->GetLen());
		return 0;
	}
	else if(header->GetLen() <= 0)
	{
		LOG_ERROR(_T("%s [%d] [%d <= 0]"), __TFUNCTION__, header->GetCommand(), header->GetLen());
		Disconnect();
		return 0;
	}

	return header->GetLen();
}

/**
@brief 주기적으로 불려지는 메소드. 핑을 날린다.
*/
void CP2PRelayPeerEx::Act()
{
	RPPing sData;
	Send(&sData);
}

/**
@brief 해당 피어에게 데이터를 전송한다.
@param header 전달할 패킷 헤더
@nAddLen 부가 데이터 길이
@pAddData 부가 데이터 포인터
@return 전송 결과
*/
bool CP2PRelayPeerEx::Send(RPHeader* header, int addlen, void* adddata)
{
	return CPeer::Send(header->GetLen() - addlen, header, addlen, adddata);
}

/**
@brief 클라이언트로부터 내부 주소를 전달 받는다
@param sockaddrin 전달 받은 주소 구조체 포인터
*/
void CP2PRelayPeerEx::SetInternalSockaddrIn(sockaddr_in* sockaddrin)
{
	m_bRecvInternalSockaddrin = true;
	m_InternalSockaddrIn = *sockaddrin; ///전달된 주소를 내부 주소로 사용.
	m_ExternalSockaddrIn = *GetSockAddrIn(); ///IP주소 값은, 클라이언트와 연결에 쓰인 값을 쓰기 위해서 우선 구조체 복사를 한다.
	m_ExternalSockaddrIn.sin_port = sockaddrin->sin_port; //외부 주소 포트를 내부주소에서 넘어온 포트로 세팅하라. 내부 주소로 설정된 포트가 외부에서 들어오는 포트로 사용될 가능성이 높기 떄문.
}

/**
@brief 자신의 내부 주소 패킷 처리
@param header 패킷 헤더
*/
void CP2PRelayPeerEx::_RecvInternalSockAddrIn(PRHeader* header)
{
	if(m_bRecvInternalSockaddrin) //내부주소가 또 오면
	{
		LOG_ERROR(_T("%s NULL != GetInternalSockaddrIn() [%d]"), __TFUNCTION__, GetIdx());
		return;
	}

	PRInternalSockaddrIn* internalsockaddrin = (PRInternalSockaddrIn*)header;
	SetInternalSockaddrIn(&internalsockaddrin->m_SockaddrIn); //내부 주소 설정하고

	RPExternalSockaddrIn sData(GetIdx(), GetExternalSockaddrIn());  //외부 주소 리턴~!
	Send(&sData);

	if(m_nReservationGroupNo != -1) //예약된 입장 그룹 번호가 있다면
	{
		GetP2PRelay()->Enter(m_nReservationGroupNo, this); //해당 그룹으로 입장하고
		m_nReservationGroupNo = -1; //예약번호를 해제한다.
	}
}

/**
@brief 그룹 참여 패킷 처리
@param header 패킷 헤더
*/
void CP2PRelayPeerEx::_Enter(PRHeader* header)
{
	PREnter* enter = (PREnter*)header;
	if(NULL == GetInternalSockaddrIn()) //아직 내부주소를 받지 않았는데 해당 패킷이 오면
	{
		LOG_INFO(_T("%s [%d] [%d]"), __TFUNCTION__, enter->m_nGroupNo, GetIdx());
		m_nReservationGroupNo = enter->m_nGroupNo; //해당 그룹 번호를 예약해둬라
	}
	else
	{
		GetP2PRelay()->Enter(enter->m_nGroupNo, this);
	}
}

/**
@brief 그룹 퇴장 패킷 처리
@param header 패킷 헤더
*/
void CP2PRelayPeerEx::_Exit(PRHeader* header)
{
	GetP2PRelay()->Exit(this);
}

/**
@brief 사용자 정의 패킷 처리
@param header 패킷 헤더
*/
void CP2PRelayPeerEx::_Command(PRHeader* header)
{
	PRCommand* command = (PRCommand*)header;
	GetP2PRelay()->Send(this, command->m_nReceiverIdx, reinterpret_cast<RUDPHeader*>((char*)command + sizeof(PRCommand)));
}

/**
@brief 핑 패킷 처리
@param header 패킷 헤더
*/
void CP2PRelayPeerEx::_Ping(PRHeader* header)
{

}

} //namespace Redmoon

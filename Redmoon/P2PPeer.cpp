#include "P2PPeer.h"
#include "P2PCommon.h"
#include "Buffer.h"
#include "BitOperator.h"
#include "P2PEventSelect.h"
#include "P2PRelayConnector.h"
#include "P2PRelayCommon.h"

namespace Redmoon
{

/**
@brief 생성자.
@param refSocket UDP통신에 사용할 소켓 참조자
@param nIdx 피어 식별값
@param sockaddrin UDP통신에 사용할 주소값
@param dwRetransmissionTick 재전송 주기
@param dwRudpFlag RUDP 통신 옵션
*/
CP2PPeer::CP2PPeer(const SOCKET &refSocket, int nIdx, SOCKADDR_IN* sockaddrin, DWORD dwRetransmissionTick, DWORD dwRUdpFlag) : 
m_refSocket(refSocket), m_nIdx(nIdx), m_dwRUdpLevelFlag(dwRUdpFlag), 
m_dwRetransmissionTick(dwRetransmissionTick), m_bUdpEnable(true)
{
	Initialize();
	SetSockAddrIn(sockaddrin);
}

/**
@brief 소멸자. 각종 리소스를 해제한다.
*/
CP2PPeer::~CP2PPeer()
{
	m_listKeepingBuffer.clear();
	m_stl_map_RetransmissionBuffer.clear();

	Release();
}

/**
@brief 초기화.
*/
void CP2PPeer::Initialize()
{
	m_bUdpEnable = false;
	m_nPacketNumber = 0;
	m_dwPingTick = GetTickLimit49Day();
	m_nStoredPacketNumber = 0;
}

/**
@brief 릴리즈. 아무것도 안함.
*/
void CP2PPeer::Release()
{
}

/**
@brief 유효한 패킷인지 검사
@param header 검사할 패킷의 헤더 포인터
@return 유효한지 여부
@desc 데이터 신뢰성 옵션을 안켰으면 걍 인정해준다.
*/
bool CP2PPeer::_isValidPacket(RUDPHeader* header)
{
	if(BitState(m_dwRUdpLevelFlag, RUDPLEVEL_RELIABLE_DATA))
	{
		if(header->isValidPacket())
		{
			return true;
		}
		else
		{
			LOG_ERROR(_T("%s [%d != %d]"), __TFUNCTION__, header->GetCheckSum(), header->GetSumValue());
			return false;
		}
	}
	return true;
}

/**
@brief 패킷을 잘 받았다고 전송한다. 더 보내지 말란건 안전하게 릴레이로 전달한다.
@param nPacketNumber 해당 패킷 번호를 잘 받았다고 전송한다.
*/
void CP2PPeer::_SendRecvPacketNumber(PACKET_NUMBER nPacketNumber)
{
	RUDPRecvPacketNumber sData(nPacketNumber); //유효한 패킷이면, 그만 줘도 된다고 알려준다.
	_RelaySendNotSetPacketNumber(&sData); //안전하게 릴레이로 번호 지정 안하고 전달한다. 
}

/**
@brief Receive된 데이터를 분석해서 유효한지 검증하는 메소드.
@param nLen 전달된 데이터 크기
@param pData 전달된 데이터 포인터
@desc 다른 PacketParse메소드들과 달리 패킷을 큐에 넣거나 처리하는 역할까지 한다.
*/
int CP2PPeer::Verify(const unsigned int nLen, const void* pData)
{
	RUDPHeader* header = reinterpret_cast<RUDPHeader*>(const_cast<void*>(pData));
	if(nLen < sizeof(RUDPHeader))
		return 0;

	if(header->GetLen() < nLen)
		return 0;

	if(!_isValidPacket(header))
		return header->GetLen();

	if(!BitState(m_dwRUdpLevelFlag, RUDPLEVEL_RELIABLE_RECEIVE)) //도착 보장 옵션이 꺼져있다면. 
	{
		GetP2PInterface()->RecvPacket(header);
		return header->GetLen();
	}

	if(!BitState(m_dwRUdpLevelFlag, RUDPLEVEL_RELIABLE_ORDER)) //순서 보장 옵션이 꺼져있다면 그냥 푼다.
	{
		GetP2PInterface()->RecvPacket(header);
		return header->GetLen();
	}
	if(header->GetCommand() == RUDPCOMMAND_RECV_PACKET_ANSWER) //패킷을 받았다는 보장이면, 패킷 번호가 없다. 그냥 푼다.
	{
		GetP2PInterface()->RecvPacket(header);
		return header->GetLen();
	}

	if(header->GetCommand() == RUDPCOMMAND_ISCONNECTED) //접속 완료 패킷을 받았을 땐 그냥 푼다.
	{
		GetP2PInterface()->RecvPacket(header);
		return header->GetLen();
	}

	PACKET_NUMBER nPacketNumber = header->GetPacketNumber();
	if(m_listWaitingPacketNumber.empty()) //대기중인 패킷이 없다면? 대기 목록 작성.
	{
		if(m_nStoredPacketNumber < nPacketNumber) //저장된 번호보다 커야만됨!
		{
			_NonWaitingPacketProc(nPacketNumber, nLen, const_cast<void*>(pData));
		}
		else
		{
			return 0;
		}
	}
	else
	{
		STL_LIST_Buffer::iterator it = find_if(m_listKeepingBuffer.begin(), m_listKeepingBuffer.end(), FindPacketNumber(nPacketNumber)); //패킷이 이미 저장 중인지 쌓였는지 찾는다.
		if(it == m_listKeepingBuffer.end())
		{
			if(_isWaitingPacketNumber(nPacketNumber)) //대기중인 패킷이라면
			{
				_AddKeepingBuffer(nPacketNumber, nLen, const_cast<void*>(pData)); //패킷을 모아둔다.
			}
			else
			{
				LOG_DEBUG(_T("%s !isWaitingPacketNumber() [%d] [%d, %d]"), __TFUNCTION__, header->GetIdx(), nPacketNumber, header->GetCommand());
				return 0;
			}
		}
	}

	if(m_listKeepingBuffer.size() == m_listWaitingPacketNumber.size()) //두 사이즈가 같으면 도착할 패킷 순서 맞춰서 다 도착한 것이다!
		_ProcKeepingData();

	return header->GetLen();
}

/**
@brief 완성된 패킷을 처리하는 메소드
@param nLen 전달된 데이터 크기
@param pData 전달된 데이터 포인터
*/
void CP2PPeer::OnReceive(int nLen, void* data)
{
	_Proc(reinterpret_cast<RUDPHeader*>(data));
}

/**
@brief 대상의 주소를 설정하는 메소드
@param sockaddrin 대상의 주소 구조체 포인터
*/
void CP2PPeer::SetSockAddrIn(SOCKADDR_IN* sockaddrin)
{
	m_SockAddrIn =* sockaddrin;
	__super::SetSockAddrIn(&m_SockAddrIn);
}

/**
@brief 대기할 패킷 번호 목록에 추가
@param nPacketNumber 대기할 패킷 번호
*/
void CP2PPeer::_AddWaitingList(PACKET_NUMBER nPacketNumber)
{
	m_listWaitingPacketNumber.push_back(nPacketNumber);
}

/**
@brief 패킷 번호를 맞춰서 풀기 위해 목록에 저장하는 메소드
@param nPacketNumber 대기할 패킷 번호
@param nLen 데이터 크기
@param pData 데이터 포인터
*/
void CP2PPeer::_AddKeepingBuffer(PACKET_NUMBER nPacketNumber, int nLen, void* pData)
{
	m_listKeepingBuffer.insert(_GetInsertPosition(nPacketNumber), new SBufferWrapper(nPacketNumber, new CBuffer(nLen, pData)));
	_SendRecvPacketNumber(nPacketNumber); //보관중인 버퍼에 넣을땐, 더이상 안보내도 된다고 알려준다.
}

/**
@brief 대기중인 패킷이 없을때 전달받은 패킷의 처리 메소드
@param nPacketNumber 전달된 패킷 번호
@param nLen 데이터 크기
@param pData 데이터 포인터
*/
void CP2PPeer::_NonWaitingPacketProc(PACKET_NUMBER nPacketNumber, int nLen, void* pData) //대기중이지 않지만 유효한 패킷의 처리 방법이다.
{
	for(unsigned short i = m_nStoredPacketNumber + 1; i != (nPacketNumber + 1); i++) //부호가 !=인 이유는 오버플로우 되는 상황까지도 만들려고!
		_AddWaitingList(i);

	m_nStoredPacketNumber = nPacketNumber;
	_AddKeepingBuffer(nPacketNumber, nLen, pData);
}

/**
@brief 해당 패킷 번호가 대기중인 패킷 번호 리스트에서 저장될 위치를 잡아주는 메소드
@param nPacketNumber 전달 받은 패킷 번호
@return 대기중인 패킷 번호 리스트에서 저장될 위치 반복자 (iterator)
*/
STL_LIST_Buffer::iterator CP2PPeer::_GetInsertPosition(PACKET_NUMBER nPacketNumber)
{
	STL_LIST_PacketNumber::iterator packetnumber_it = _GetAdjoinPacketNumber(nPacketNumber);
	if(packetnumber_it == m_listWaitingPacketNumber.end())
		return m_listKeepingBuffer.end();

	PACKET_NUMBER nNextPacketNumber =* packetnumber_it;
	STL_LIST_Buffer::iterator it = find_if(m_listKeepingBuffer.begin(), m_listKeepingBuffer.end(), FindOverflowPoint());
	if(it == m_listKeepingBuffer.end()) //오버플로우 포인트가 없으면?
		it = m_listKeepingBuffer.begin(); //첨부터 검색한다

	return find_if(it, m_listKeepingBuffer.end(), FindMorethanPacketNumber(nNextPacketNumber)); //다음 패킷보다 같거나 큰거면 콜~
}

/**
@brief 특정 패킷 번호의 패킷이 대기중인지 여부
@param nPacketNumber 패킷 번호
@return 대기중인 패킷인지 여부
*/
bool CP2PPeer::_isWaitingPacketNumber(PACKET_NUMBER nPacketNumber)
{
	return _FindWaitingPacketIterator(nPacketNumber) != m_listWaitingPacketNumber.end();
}

/**
@brief 저장중인 데이터를 처리하고 리스트도 비우는 메소드.
*/
void CP2PPeer::_ProcKeepingData() 
{
	STL_LIST_Buffer listBuffer = m_listKeepingBuffer;
	for(STL_LIST_Buffer::iterator it = listBuffer.begin(); it != listBuffer.end(); ++it) //리스트에 들어가 있는 순서대로 푼다.
	{
		CBuffer* buffer = it->GetPtr()->m_pcBuffer;
		GetP2PInterface()->RecvPacket(reinterpret_cast<RUDPHeader*>(buffer->GetHeadBuffer()));
	}

	m_listKeepingBuffer.clear();
	m_listWaitingPacketNumber.clear();
}

/**
@brief 특정 패킷이 대기중인지를 반환하는 메소드
@param nPacketNumber 전달 받은 패킷 번호
@return 대기중인 패킷 번호의 반복자. (iterator) 
*/
STL_LIST_PacketNumber::iterator CP2PPeer::_FindWaitingPacketIterator(PACKET_NUMBER nPacketNumber)
{
	return find_if(m_listWaitingPacketNumber.begin(), m_listWaitingPacketNumber.end(), FindPacketNumber(nPacketNumber));
}

/**
@brief 특정 패킷보다 큰 번호의 반복자를 리턴하는 메소드
@param nPacketNumber 전달 받은 패킷 번호
@return 넘어온 패킷번호보다 큰 패킷번호의 반복자. (iterator) 
*/
STL_LIST_PacketNumber::iterator CP2PPeer::_GetAdjoinPacketNumber(PACKET_NUMBER nPacketNumber)
{
	return find_if(m_listWaitingPacketNumber.begin(), m_listWaitingPacketNumber.end(), FindMorePacketNumber(nPacketNumber));
}

/**
@brief 넘어온 header에 패킷 번호를 설정한다
@param header 패킷 헤더
*/
void CP2PPeer::_SetPacketNumber(RUDPHeader* header)
{
	_IncreasePacketNumber();
	header->SetPacketNumber(_GetPacketNumber());
}

/**
@brief 접속 여부 질의 패킷 처리 메소드
@param header 패킷 헤더
*/
void CP2PPeer::_IsConnected(RUDPHeader* header)
{
	RUDPIsConnected* isconnected = (RUDPIsConnected*)header;

	RUDPSuccessed sData;
	Send(&sData);

	LOG_INFO(_T("%s [%d, %d, %d] [%s]"), __TFUNCTION__, header->GetIdx(), header->GetPacketNumber(), header->GetCommand(), GetIP().c_str());	
}

/**
@brief UDP연결 성공 패킷 처리 메소드
@param header 패킷 헤더
*/
void CP2PPeer::_Successed(RUDPHeader* header)
{
	RUDPSuccessed* successed = (RUDPSuccessed*)header;
	m_bUdpEnable = true;
	LOG_INFO(_T("%s [%d, %d, %d] [%s]"), __TFUNCTION__, header->GetIdx(), header->GetPacketNumber(), header->GetCommand(), GetIP().c_str());
}

/**
@brief 해당 패킷 수신 확인 패킷의 처리 함수
@param header 패킷 헤더
*/
void CP2PPeer::_RecvPacketNumber(RUDPHeader* header)
{
	RUDPRecvPacketNumber* recvpacketnumber = (RUDPRecvPacketNumber*)header;
	_RemoveRetransmissionPacket(recvpacketnumber->nRecvPacketNumber); //패킷 왔으니 해당 패킷은 재전송 멈춤!
}

/**
@brief 핑 패킷 처리 함수
@param header 패킷 헤더
*/
void CP2PPeer::_Ping(RUDPHeader* header)
{
	RUDPPing* ping = (RUDPPing*)header;
	LOG_DEBUG(_T("%s [%d] [%d]"), __TFUNCTION__, GetIdx(), header->GetPacketNumber());
}

/**
@brief 사용자 정의 패킷 처리 함수. 순수 가상함수 Proc으로 전달한다.
@param header 패킷 헤더
*/
void CP2PPeer::_Command(RUDPHeader* header)
{
	Proc((RUDPCommand*)header);
}

/**
@brief 패킷 처리함수를 결정하는 메소드
@param header 패킷 헤더
@return 처리한 패킷 길이
*/
int CP2PPeer::_Proc(RUDPHeader* header)
{
	switch(header->GetCommand())
	{
	case RUDPCOMMAND_ISCONNECTED:
		{
			_IsConnected(header);
		}
		break;
	case RUDPCOMMAND_SUCCESSED:
		{
			_Successed(header);
		}
		break;
	case RUDPCOMMAND_RECV_PACKET_ANSWER:
		{
			_RecvPacketNumber(header);
		}
		break;
	case RUDPCOMMAND_PING:
		{
			_Ping(header);
		}
		break;
	case RUDPCOMMAND_COMMAND:
		{
			_Command(header);
		}
		break; 
	default:
		LOG_INFO(_T("%s Denied [%d] [%d, %d]"), __TFUNCTION__,header->GetIdx(), header->GetPacketNumber(), header->GetCommand());
		break;
	}
	return header->GetLen();
}

/**
@brief 패킷 재전송
@param nPacketNumber 전달할 패킷 번호
@return 재전송 결과
*/
bool CP2PPeer::_Retransmission(int nPacketNumber)
{
	STL_MAP_RetransmissionBuffer::iterator it = m_stl_map_RetransmissionBuffer.find(nPacketNumber);
	if(it == m_stl_map_RetransmissionBuffer.end())
	{
		LOG_ERROR(_T("%s Critical Error. [%d]th Packet m_stl_map_RetransmissionBuffer.find() Notfind PacketBuffer"), __TFUNCTION__, nPacketNumber);;
		return false;
	}
	return _UdpSend(it->second->m_pcBuffer);
}

/**
@brief 릴레이로 패킷을 전송한다.
@param nPacketNumber 전달할 패킷 번호
@return 재전송 결과
*/
bool CP2PPeer::_RelaySend(CBuffer* pcBuffer)
{
	PRCommand sData(GetIdx(), pcBuffer->GetUsingSize());
	return GetP2PRelayConnector()->Send(sizeof(sData), &sData, pcBuffer->GetUsingSize(), pcBuffer->GetHeadBuffer());
}

/**
@brief 릴레이로 패킷 전송
@param header 전달할 패킷 헤더
@nAddLen 부가 데이터 길이
@pAddData 부가 데이터 포인터
@return 전송 결과
*/
bool CP2PPeer::_RelaySend(RUDPHeader* header, int nAddLen, void* pAddData)
{
	header->SetIdx(GetP2PInterface()->GetIdx());
	_SetPacketNumber(header);

	CBuffer *pcBuffer = new CBuffer(header->GetLen() - nAddLen, header, nAddLen, pAddData);
	bool bRet = _RelaySend(pcBuffer);
	delete pcBuffer;
	return bRet;
}

/**
@brief 재전송 없이 UDP로 전송한다. 패킷 번호도 설정하지 않는다.
@param header 전달할 패킷 헤더
@nAddLen 부가 데이터 길이
@pAddData 부가 데이터 포인터
@return 전송 결과
*/
bool CP2PPeer::_UdpSendNotRetransmission(RUDPHeader* header, int nAddLen /* = 0 */, void* pAddData /* = 0 */)
{
	header->SetIdx(GetP2PInterface()->GetIdx());
	CBuffer *pcBuffer = new CBuffer(header->GetLen() - nAddLen, header, nAddLen, pAddData);
	bool bRet = _UdpSend(pcBuffer);
	delete pcBuffer;
	return bRet;
}

/**
@brief 패킷 번호를 설정하지 않고 릴레이로 보낸다.
@param header 전달할 패킷 헤더
@nAddLen 부가 데이터 길이
@pAddData 부가 데이터 포인터
@return 전송 결과
*/
bool CP2PPeer::_RelaySendNotSetPacketNumber(RUDPHeader* header, int nAddLen, void* pAddData)
{
	header->SetIdx(GetP2PInterface()->GetIdx());
	CBuffer *pcBuffer = new CBuffer(header->GetLen() - nAddLen, header, nAddLen, pAddData);
	bool bRet = _RelaySend(pcBuffer);
	delete pcBuffer;
	return bRet;
}

/**
@brief UDP로 전송한다.
@param header 전달할 패킷 헤더
@nAddLen 부가 데이터 길이
@pAddData 부가 데이터 포인터
@return 전송 결과
*/
bool CP2PPeer::_UdpSend(RUDPHeader* header, int nAddLen /* = 0*/, void* pAddData /* = 0*/)
{
	header->SetIdx(GetP2PInterface()->GetIdx());
	_SetPacketNumber(header);

	CBuffer *pcBuffer = new CBuffer(header->GetLen() - nAddLen, header, nAddLen, pAddData);
	bool bRet = _UdpSend(pcBuffer);
	_AddRetransmissionPacket(pcBuffer);
	return bRet;
}

/**
@brief UDP로 패킷 전송
@param pcBuffer 데이터가 담긴 버퍼 포인터
@return 전송 결과
*/
bool CP2PPeer::_UdpSend(CBuffer* pcBuffer)
{
	return CUDPSocket::Send(m_refSocket, m_SockAddrIn, pcBuffer->GetUsingSize(), pcBuffer->GetHeadBuffer());
}

/**
@brief 패킷 전달 범용 메소드. UDP 연결 되어있으면 UDP로, 아니면 릴레이로 보낸다.
@param header 전달할 패킷 헤더
@nAddLen 부가 데이터 길이
@pAddData 부가 데이터 포인터
@return 전송 결과
*/
bool CP2PPeer::Send(RUDPHeader* header, int nAddLen /* = 0*/, void* pAddData /* = 0*/)
{
	bool bRet = false;
	if(m_bUdpEnable)
	{
		bRet =  _UdpSend(header, nAddLen, pAddData);
	}
	else
	{
		bRet = _RelaySend(header, nAddLen, pAddData);
	}	

	if(!bRet)
	{
		LOG_ERROR(_T("%s Failed [%d] [%d, %d]"), __TFUNCTION__, header->GetIdx(), header->GetPacketNumber(), header->GetCommand());
	}
	return bRet;
}

/**
@brief 재전송 목록에 추가한다. 전달된 pcSendBuffer는 이 메소드가 불려진 이후 사용하면 안된다.
@param pcSendBuffer 데이터 보낸 버퍼
@return 재전송 목록에 추가 결과
*/
bool CP2PPeer::_AddRetransmissionPacket(CBuffer* pcSendBuffer)
{
	RUDPHeader* header = reinterpret_cast<RUDPHeader*>(pcSendBuffer->GetBuffer());
	CScopeCriticalSection cs(m_CriticalSection);
	STL_MAP_RetransmissionBuffer::iterator it = m_stl_map_RetransmissionBuffer.find(header->GetPacketNumber());
	if(it == m_stl_map_RetransmissionBuffer.end()) 
	{
		if(BitState(m_dwRUdpLevelFlag, RUDPLEVEL_RELIABLE_RECEIVE)) //도착 보장시에만 재전송 버퍼 사용
		{
			m_stl_map_RetransmissionBuffer.insert(STL_MAP_RetransmissionBuffer::value_type(header->GetPacketNumber(), new SRetransmissionBuffer(GetTickLimit49Day(), pcSendBuffer)));
			return true;
		}
		else
		{
			delete pcSendBuffer;
			return true;	
		}
	}

	//이미 같은 패킷번호로 있다면? 넵둔다~ 왜냐면 계속 재전송 해야 되기에 여러번 불려질 수 있다!
	delete pcSendBuffer;
	return false;
}

/**
@brief 재전송 목록에서 해당 패킷 번호를 제거한다.
@param nPacketNumber 패킷 번호
@return 목록에서 제거 결과
*/
bool CP2PPeer::_RemoveRetransmissionPacket(PACKET_NUMBER nPacketNumber)
{
	CScopeCriticalSection cs(m_CriticalSection);
	if(BitState(m_dwRUdpLevelFlag, RUDPLEVEL_RELIABLE_ORDER))
	{
		m_stl_map_RetransmissionBuffer.erase(nPacketNumber);
	}
	else
	{
		STL_MAP_RetransmissionBuffer::iterator it = m_stl_map_RetransmissionBuffer.upper_bound(nPacketNumber);
		if(it == m_stl_map_RetransmissionBuffer.end())
			return false;

		m_stl_map_RetransmissionBuffer.erase(m_stl_map_RetransmissionBuffer.begin(), it);
	}
	return true;
}

/**
@brief 피어가 주기적으로 처리해야될 일들을 하는 메소드. 패킷 재전송, 핑, UDP연결 시도, 릴레이로 전환 등 다양한 처리를 한다.
*/
void CP2PPeer::Act()
{
	DWORD curtick = GetTickLimit49Day();
	CScopeCriticalSection cs(m_CriticalSection);

	if(50 <= m_stl_map_RetransmissionBuffer.size()) //재전송 목록이 50개가 넘으면 릴레이로 전환한다.
	{
		_RetransmissionPacketToRelay();
		return;
	}

	for(STL_MAP_RetransmissionBuffer::iterator it = m_stl_map_RetransmissionBuffer.begin(); it != m_stl_map_RetransmissionBuffer.end(); ++it)
	{
		SRetransmissionBuffer* pcRetransmissionBuffer = it->second;
		if(RETRANSMISSION_TRY_LIMIT_COUNT <= pcRetransmissionBuffer->m_nTryCount) //5번 이상 재전송 하면 릴레이로 전환한다.
		{
			_RetransmissionPacketToRelay();
			return;
		}

		if(m_dwRetransmissionTick < curtick - pcRetransmissionBuffer->m_dwSendTick) //주기적으로 재전송한다.
		{
			_Retransmission(it->first);
			pcRetransmissionBuffer->m_nTryCount++;
			pcRetransmissionBuffer->m_dwSendTick = curtick;
		}
	}

	if(m_dwRetransmissionTick < curtick - m_dwPingTick) //재전송 주기 확인
	{
		if(!m_bUdpEnable) //UDP연결되어있으면 핑을 날린다.
		{
			RUDPPing sData;
			Send(&sData);
		}
		else //UDP연결 되어있지 않다면 접속 시도를 한다.
		{
			//피어 연결 시도 패킷 쏴본다~! 회신 오는지 여부를 보고, 회신 오면 UDP플래그 켜고, 안오면 걍 릴레이~ ㄱㄱ
			RUDPIsConnected sData;
			_UdpSendNotRetransmission(&sData); //아직 P2P연결 안되었지만, 연결 가능 여부를 확인하기 위해 Udp로 쏴야한다. 그리고 패킷 번호도 할당하지 않는다.
		}

		m_dwPingTick = curtick;
	}
}

/**
@brief UDP전송을 멈추고 릴레이로 전환한다.
*/
void CP2PPeer::_RetransmissionPacketToRelay()
{
	LOG_INFO(_T("%s"), __TFUNCTION__);
	m_bUdpEnable = false;

	for(STL_MAP_RetransmissionBuffer::iterator it = m_stl_map_RetransmissionBuffer.begin(); it != m_stl_map_RetransmissionBuffer.end(); ++it)
	{
		_RelaySend(it->second->m_pcBuffer);
	}
	m_stl_map_RetransmissionBuffer.clear();
}

} //namespace Redmoon
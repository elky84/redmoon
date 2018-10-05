#include "Pipe.h"

namespace Redmoon
{

/**
@brief ������. ������ �̸��� �Ǿ ���� �޴´�.
@param strName �Ǿ� �̸�
@param pcPeer IO�� ����� �Ǿ�
*/
CPipe::CPipe(const tstring& strName, CPeer* pcPeer) : CIoInterface(pcPeer), m_strName(strName), m_hPipe(INVALID_HANDLE_VALUE), m_bTryConnect(FALSE), m_bConnected(FALSE), 
m_bRefreshing(FALSE), m_cPipeContextAccept(this, IOTYPE_CONNECT), m_cPipeContextRecv(this, IOTYPE_RECV), m_cPipeContextSend(this, IOTYPE_SEND),
m_pcSendBuffer(new CBuffer(PIPE_BUF_SIZE)), m_pcRecvBuffer(new CBuffer(PIPE_BUF_SIZE))
{
	
}

/**
@brief �Ҹ���.
*/
CPipe::~CPipe()
{
	
}

/**
@brief �񵿱� Receive �޽����� Trigger�Ŵ� �޼ҵ�
@return ���� ����
*/
bool CPipe::Receive()
{
	if(!isConnected())
		return false;

	DWORD nRecv = 0;
	if(!ReadFile(m_hPipe, m_pcRecvBuffer->GetHeadBuffer(), m_pcRecvBuffer->GetRemainSize(), &nRecv, GetPipeContextRecv()))
	{
		if(ERROR_IO_PENDING != GetLastError())
		{
			LOG_INFO(_T("%s ReadFile Failed [%d] [%p, %d]"), __TFUNCTION__, GetLastError(), m_pcRecvBuffer->GetHeadBuffer(), m_pcRecvBuffer->GetRemainSize());
			return false;
		}
	}
	return true;
}

/**
@brief ���� ���������� �� ȣ���ϴ� �޼ҵ�
*/
void CPipe::Reuse()
{
	m_bRefreshing = FALSE;	
}

/**
@brief Send ��û�� �ҷ����� �޼ҵ�. 
@param nLen Receive�� ������ ����
@param pData ������ ������
@param nAddLen pAddData�� ũ��
@param pAddData ����� �߰� ������
@return ���� ����
*/
bool CPipe::Send(const unsigned int nLen, const void* pData, const unsigned int nAddLen /* = 0 */, const void* pAddData /* = 0 */)
{
	if(!isConnected())
		return false;

	if(!m_pcSendBuffer->AddData(nLen, pData, nAddLen, pAddData))
	{
		LOG_INFO(_T("%s m_pcSendBuffer->AddData(%d, %p, %d, %p) Failed [%d]"), __TFUNCTION__, nLen, pData, nAddLen, pAddData, GetLastError());
		return false;
	}

	CScopeCriticalSection cs(m_cSendLock);
	DWORD nWritten = 0;
	if(!WriteFile(m_hPipe, m_pcSendBuffer->GetHeadBuffer(), m_pcSendBuffer->GetUsingSize(), &nWritten, GetPipeContextSend()))
	{
		if(ERROR_IO_PENDING != GetLastError())
		{
			LOG_INFO(_T("%s WriteFile(%d, %d, %d) Failed [%d]"), __TFUNCTION__, m_hPipe, m_pcSendBuffer->GetHeadBuffer(), m_pcSendBuffer->GetUsingSize(), GetLastError());
			return false;
		}
	}
	return true;
}

/**
@brief Event ���� ������� ��� ��û
*/
void CPipe::Register()
{
	GetPipeContextAccept()->Register();
	GetPipeContextRecv()->Register();
	GetPipeContextSend()->Register();
}

/**
@brief Receive �Ϸ�� �̺�Ʈ �޼ҵ�
@param nLen Receive �� ũ��
@param pData ������ ������
@return ���� ����
*/
bool CPipe::OnReceive(unsigned int nLen, void* pData)
{
	m_pcRecvBuffer->JumpTail(nLen);
	while(0 < m_pcRecvBuffer->GetUsingSize())
	{
		int nPacketLen = m_pcPeer->Verify(m_pcRecvBuffer->GetUsingSize(), m_pcRecvBuffer->GetHeadBuffer());
		if(nPacketLen <= 0)
		{
			Disconnect();
			return 0;
		}

		m_pcPeer->OnReceive(nPacketLen, m_pcRecvBuffer->GetHeadBuffer());
		m_pcRecvBuffer->JumpHead(nPacketLen);
	}
	m_pcRecvBuffer->Compress();
	Receive();
	return true;
}

/**
@brief ���� ������ �̺�Ʈ �޼ҵ�
*/
void CPipe::OnConnect()
{
	m_pcPeer->OnConnect();
	m_bConnected = TRUE;
	Receive();
}

/**
@brief ���� ������ �̺�Ʈ �޼ҵ�
*/
void CPipe::OnDisconnect()
{
	m_bConnected = FALSE;
	m_bRefreshing = FALSE;

	m_pcPeer->OnDisconnect();

	m_pcRecvBuffer->Init();
	m_pcSendBuffer->Init();
}

/**
@brief Send�Ϸ�� �̺�Ʈ �޼ҵ�
*/
void CPipe::OnSend(unsigned int nLen)
{
	CScopeCriticalSection cs(m_cSendLock);
	m_pcSendBuffer->JumpHead(nLen);
	m_pcSendBuffer->Compress();
}

} //namespace Redmoon
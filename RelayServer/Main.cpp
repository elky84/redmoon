#include "stdafx.h"
#include "Main.h"
#include "LogManager.h"

#include "P2PRelayPeerEx.h"

#include "UDPSocket.h"

DECLARE_ALLOCATE_FUNCTION(Redmoon::CP2PRelayPeerEx, RelayPeerAllocater);	

CMain::CMain(TCHAR* svrname) : CEnvironmentManager(svrname)
{
	CThread::Start();
}

CMain::~CMain()
{
	
}

void CMain::Constructer()
{
	Redmoon::CListener::AllocThread(3);
	m_pcP2PRelayListener.Reset(new Redmoon::CListener(&RelayPeerAllocater));

	if(!m_pcP2PRelayListener->Start(7890, 300))
		Stop();

	///이거 라이브러리단으로 옮기고, 리턴 왔을때의 포트와 주소 클라에 전달하도록 만들자!!
	m_pcUDPSocket.Reset(new Redmoon::CUDPSocket);
	if(!m_pcUDPSocket->Bind(Redmoon::P2P_ECHO_PORT))
		Stop();
}

void CMain::Run()
{
	m_pcEvent->Wait(5000);

	m_pcP2PRelayListener->Act();
	m_pcP2PRelayListener->AliveCheck();
}

void CMain::Stop()
{
	Redmoon::CEnvironmentManager::Stop();
	Redmoon::CThread::Stop();
}

void CMain::Destructer()
{
	Stop();
}

void CMain::Act()
{
	Sleep(100);

	if(m_pcUDPSocket.Get())
	{
		sockaddr_in sockaddrin;
		Redmoon::CBuffer* pcRecvBuffer = m_pcUDPSocket->Recv(&sockaddrin);
		LOG_INFO(_T("%s [%s, %d]"), __TFUNCTION__, inet_ntoa(sockaddrin.sin_addr),  ntohs(sockaddrin.sin_port));
	}
}
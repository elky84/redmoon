#include "stdafx.h"
#include <Windows.h>
#include "Main.h"
#include "ClientPeer.h"
#include "Event.h"
#include "Listener.h"

DECLARE_ALLOCATE_FUNCTION(CClientPeer, ClientPeerAllocater);

CMain::CMain(TCHAR* svrname) : CEnvironmentManager(svrname)
{
}

CMain::~CMain()
{
}

void CMain::Constructer()
{
	Redmoon::CListener::AllocThread(10);
	m_pcListener.Reset(new Redmoon::CListener(ClientPeerAllocater));
	if(!m_pcListener->Start(7997, 1000))
	{
		Stop();
		return;
	}
}

void CMain::Run()
{
	m_pcEvent->Wait(100);
	m_pcListener->Act();
	m_pcListener->AliveCheck();

	char buf[128] = "";
	gets(buf);
	if(stricmp(buf, "quit") == 0)
	{
		Stop();
	}
}

void CMain::Destructer()
{

}
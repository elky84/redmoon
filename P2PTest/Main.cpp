#include "stdafx.h"
#include "Main.h"
#include "P2PRelayConnector.h"

CMain::CMain(TCHAR* szProgramName) : CEnvironmentManager(szProgramName), m_pcP2P(new CP2P)
{
}

CMain::~CMain()
{
}

void CMain::Constructer()
{
	new Redmoon::CP2PRelayConnector;
	Redmoon::CLogManager::InstancePtr()->Set(LOGLEVEL_DEBUG, true);

	if(!Redmoon::GetP2PInterface()->Bind(7890))
	{	
		Stop();
	}

	Redmoon::GetP2PRelayConnector()->Connect("192.168.11.17", 50000);
}

void CMain::Run()
{
	m_pcEvent->Wait(100);
	Redmoon::GetP2PInterface()->Act();
	Redmoon::GetP2PRelayConnector()->Act();
	Redmoon::GetP2PRelayConnector()->AliveCheck();

	m_pcP2P->Pop();
	Redmoon::GetP2PRelayConnector()->Pop();
}

void CMain::Destructer()
{
	
}
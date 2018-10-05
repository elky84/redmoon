#include "stdafx.h"
#include "Common.h"
#include "Main.h"
#include "TestPeer.h"
#include "Event.h"

CMain::CMain(TCHAR* szProgramName) : CEnvironmentManager(szProgramName)
{
	
}

CMain::~CMain()
{
	
}

void CMain::Constructer()
{
	Redmoon::CConnector::AllocThread(3);
	m_pcConnector.Reset(new Redmoon::CConnector(new CTestPeer, true));
	m_pcConnector->Set("127.0.0.1", 7997);
}

void CMain::Run()
{
	m_pcEvent->Wait(1000);

	m_pcConnector->Act();
	m_pcConnector->AliveCheck();
}

void CMain::Destructer()
{
}
#include "stdafx.h"
#include "Main.h"
#include "Config.h"
#include "StressTestPeer.h"
#include "Connector.h"
#include "Event.h"

CMain::CMain(TCHAR* szProgramName) : CEnvironmentManager(szProgramName)
{
	//Redmoon::CLogManager::InstancePtr()->ConsoleMode(false);
}

CMain::~CMain()
{
	
}

void CMain::Constructer()
{
	if(!CConfig::InstancePtr()->Load())
	{
		Stop();
	}

	Redmoon::CConnector::AllocThread(CConfig::InstancePtr()->GetThreadCount());
	STL_VECTOR_TESTCLIENTINFO &refVectorTestClientInfo = CConfig::InstancePtr()->GetVectorTestClientInfo();
	int i = 0;
	for(STL_VECTOR_TESTCLIENTINFO::iterator it = refVectorTestClientInfo.begin(); it != refVectorTestClientInfo.end(); ++it)
	{
		STestClientInfo &refTestClientInfo = (*it);
		for(; i < refTestClientInfo.m_nTestClientCount; i++)
		{
			Redmoon::CConnector* pcConnector = new Redmoon::CConnector(new CStressTestPeer(refTestClientInfo.m_eActionType), true);
			pcConnector->Set(GetIP().c_str(), GetPort());
			m_stl_map_Connector.insert(STL_MAP_CONNECTOR::value_type(i, pcConnector));
		}
	}

	LOG_ALARM(_T("%s Start"), GetProgramName());
}

void CMain::Run()
{
	m_pcEvent->Wait(rand() % 1000);
	for(STL_MAP_CONNECTOR::iterator it = m_stl_map_Connector.begin(); it != m_stl_map_Connector.end(); ++it)
	{
		Redmoon::CConnector* pcConnector = it->second;
		pcConnector->Act();		
	}
}

void CMain::Destructer()
{
	
}
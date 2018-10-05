#include "stdafx.h"
#include "Main.h"

#include "PipeClient.h"

#include "ServerPeer.h"

CMain::CMain(TCHAR* szProgramName) : CEnvironmentManager(szProgramName)
{
}

CMain::~CMain()
{
}

void CMain::Constructer()
{
	m_pcPipeClient.Reset(new Redmoon::CPipeClient("\\\\.\\pipe\\RemotePipe", new CServerPeer));
	m_pcPipeClient->Connect();

	LOG_ALARM(_T("Start %s Start"), GetProgramName());
}

void CMain::Run()
{
	Sleep(1000);
	m_pcPipeClient->Act();
}

void CMain::Destructer()
{
}
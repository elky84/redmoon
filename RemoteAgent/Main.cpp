#include "stdafx.h"
#include "Main.h"

#include "PipeServer.h"

#include "ClientPeer.h"

CMain::CMain(TCHAR* szProgramName) : CEnvironmentManager(szProgramName)
{
}

CMain::~CMain()
{
}

void CMain::Constructer()
{
	m_pcPipeServer.Reset(new Redmoon::CPipeServer("\\\\.\\pipe\\RemotePipe", new CClientPeer));
	m_pcPipeServer->Listen();

	LOG_ALARM(_T("Start %s Start"), GetProgramName());	
}

void CMain::Run()
{
	Sleep(1000);
	m_pcPipeServer->Act();
}

void CMain::Destructer()
{
}
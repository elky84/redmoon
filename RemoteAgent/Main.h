#pragma once

#include "EnvironmentManager.h"
#include "singleton.h"
#include "PipeServer.h"

class CMain : public Redmoon::CEnvironmentManager
{
	Redmoon::CAutoPtr<Redmoon::CPipeServer> m_pcPipeServer;
public:
	CMain(TCHAR* szProgramName);
	~CMain();

	virtual void Constructer();
	virtual void Run();
	virtual void Destructer();
};
#pragma once

#include "EnvironmentManager.h"
#include "PipeClient.h"


class CMain : public Redmoon::CEnvironmentManager
{
	Redmoon::CAutoPtr<Redmoon::CPipeClient> m_pcPipeClient;
public:
	CMain(TCHAR* szProgramName);
	~CMain();

	virtual void Constructer();
	virtual void Run();
	virtual void Destructer();
};
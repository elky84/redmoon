#pragma once

#include <map>
#include "EnvironmentManager.h"
#include "singleton.h"
#include <memory>

#include "P2P.h"
#include "P2PRelayConnector.h"

class CMain : public Redmoon::CEnvironmentManager
{
	CAutoPtr<CP2P> m_pcP2P;
public:
	CMain(TCHAR* szProgramName);
	~CMain();

	virtual void Constructer();
	virtual void Run();
	virtual void Destructer();
};
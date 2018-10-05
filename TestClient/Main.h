#pragma once

#include "EnvironmentManager.h"
#include "singleton.h"
#include <memory>
#include "Connector.h"
#include "IocpObject.h"
#include "ManualPtr.h"

class CMain : public Redmoon::CEnvironmentManager, public Redmoon::CIocpObject
{
	Redmoon::CManualPtr<Redmoon::CConnector> m_pcConnector;
public:
	CMain(TCHAR* szProgramName);
	~CMain();

	virtual void Constructer();
	virtual void Run();
	virtual void Destructer();
};
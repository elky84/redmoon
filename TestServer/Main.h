#pragma once

#include "EnvironmentManager.h"
#include "IocpObject.h"
#include "ManualPtr.h"
#include "Listener.h"

class CMain : public Redmoon::CEnvironmentManager, public Redmoon::CIocpObject
{
	Redmoon::CManualPtr<Redmoon::CListener> m_pcListener;
public:
	CMain(TCHAR* svrname);
	~CMain();

	virtual void Constructer();
	virtual void Run();
	virtual void Destructer();
};
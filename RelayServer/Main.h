#pragma once

#include "EnvironmentManager.h"
#include "Singleton.h"
#include "Listener.h"
#include "Thread.h"
#include "IocpObject.h"
#include "AutoPtr.h"
#include "ManualPtr.h"
#include "UDPSocket.h"

class CMain : public Redmoon::CEnvironmentManager , public Redmoon::CSingleton<CMain>, public Redmoon::CThread, public Redmoon::CIocpObject
{
	Redmoon::CManualPtr<Redmoon::CListener> m_pcP2PRelayListener;
	Redmoon::CAutoPtr<Redmoon::CUDPSocket> m_pcUDPSocket;

public:
	CMain(TCHAR* svrname);
	~CMain();

	virtual void Constructer();
	virtual void Run();
	virtual void Destructer();

	virtual void Act();

	void Stop();
};
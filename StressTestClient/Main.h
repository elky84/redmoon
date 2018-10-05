#pragma once

#include <map>
#include "EnvironmentManager.h"
#include "singleton.h"

#include "Config.h"
#include "IocpObject.h"
#include "Connector.h"


class CMain : public Redmoon::CEnvironmentManager, public Redmoon::CIocpObject
{
	typedef std::map<int, Redmoon::CConnector*> STL_MAP_CONNECTOR;
	STL_MAP_CONNECTOR m_stl_map_Connector;

public:
	CMain(TCHAR* szProgramName);
	~CMain();

	virtual void Constructer();
	virtual void Run();
	virtual void Destructer();

	inline const std::string& GetIP(){return CConfig::InstancePtr()->GetIP();}
	inline const int& GetPort(){return CConfig::InstancePtr()->GetPort();}
};
#pragma once

#include "Common.h"

#include "Singleton.h"
#include "CriticalSection.h"

namespace Redmoon
{

class CDBConnection;

class CDBConnectionPool
{
	STATIC_SINGLETON_PTR( CDBConnectionPool )

private:
//	typedef std::map<DWORD, CDBConnection*>					STL_MAP_DBCP;
//	STL_MAP_DBCP											m_stl_map_DBCP;

	typedef std::multimap<DWORD, CDBConnection*>			STL_MULTIMAP_DBCP;
	STL_MULTIMAP_DBCP										m_stl_multimap_DBCP;

	CCriticalSection									m_cs;

private:
	CDBConnectionPool() { Initialize(); }
	~CDBConnectionPool() { Finalize(); }

public:
	bool Initialize();
	bool Finalize();

public:
	bool Add(DWORD dwThreadId, CDBConnection* pcDBConnection);
	CDBConnection* GetConnection(DWORD dwThreadId, DWORD dwConnectionID);
	std::string toString();
};

} //namespace Redmoon
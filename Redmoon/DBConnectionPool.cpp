#include "DBConnectionPool.h"

#include "DBConnection.h"

namespace Redmoon
{

bool CDBConnectionPool::Initialize()
{
	return true;
}

bool CDBConnectionPool::Finalize()
{
	m_cs.Enter();

//	m_stl_map_DBCP.clear();

	for( STL_MULTIMAP_DBCP::iterator i = m_stl_multimap_DBCP.begin(); i != m_stl_multimap_DBCP.end(); ++i )
	{
		delete i->second;
	}
	m_stl_multimap_DBCP.clear();

	m_cs.Leave();

	return true;
}

bool CDBConnectionPool::Add(DWORD dwThreadId, CDBConnection* pcDBConnection)
{
	m_cs.Enter();

//	m_stl_map_DBCP.insert( std::make_pair(dwThreadId, pcDBConnection) );

	m_stl_multimap_DBCP.insert( std::make_pair(dwThreadId, pcDBConnection) );

	m_cs.Leave();

	return true;
}

CDBConnection* CDBConnectionPool::GetConnection(DWORD dwThreadId, DWORD dwConnectionID)
{
	CDBConnection* pcDBConnection = NULL;

	m_cs.Enter();

	//STL_MAP_DBCP::iterator itr = m_stl_map_DBCP.find(dwThreadId);
	//if( itr != m_stl_map_DBCP.end() )
	//{
	//	pcDBConnection = itr->second;
	//}

	for( STL_MULTIMAP_DBCP::iterator i = m_stl_multimap_DBCP.find(dwThreadId); i != m_stl_multimap_DBCP.end(); ++i )
	{
		if( i->second->m_dwConnectionID == dwConnectionID )
		{
			pcDBConnection = i->second;
			break;
		}
	}

	m_cs.Leave();

	return pcDBConnection;
}

std::string CDBConnectionPool::toString()
{
	std::stringstream str;

//	LOG_INFO(TEXT("-------------------------------------------------") );
//	LOG_INFO(TEXT("|                    DBPool                       |") );
//	LOG_INFO(TEXT("-------------------------------------------------") );
//	LOG_INFO(TEXT("|   PoolIndex   |      PoolName                   |") );
//	LOG_INFO(TEXT("-------------------------------------------------") );
//
//	m_cs.Enter();
//
//	STL_MAP_DBCP::iterator itr = m_stl_map_DBCP.begin();
//	for( ; itr != m_stl_map_DBCP.end(); ++itr )
//	{
////		CDBConnection* pcDBConnection = itr->second;
////		if( NULL == pcDBConnection ) continue;
//
////		str
////			<< "[" << pcChannel->GetChannelNo() << "] " << pcChannel->GetChannelName() << std::endl;
////
////		LOG_INFO(TEXT("[%8d]      %s"), pcChannel->GetChannelNo(), pcChannel->GetChannelName() );
//
//		//xne::XPRINTF( TEXT("[%8d] %s"), pcChannel->GetChannelNo(), pcChannel->GetChannelName() );
//	}
//	
//	m_cs.Leave();
//
//	LOG_INFO(TEXT("-------------------------------------------------") );

	return str.str();
}

} //namespace Redmoon
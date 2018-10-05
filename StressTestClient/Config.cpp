#include "stdafx.h"
#include "Config.h"

CConfig::CConfig() : m_nThreadCount(0), m_nPort(0)
{
	
}

CConfig::~CConfig()
{
	
}

bool CConfig::Load()
{
	TiXmlDocument xmldoc("cfg\\Config.xml");
	const TiXmlElement* eleSetting = xmldoc.FirstChildElement("Setting");
	if( eleSetting == NULL) return false;

	const TiXmlElement* eleIocp = eleSetting->FirstChildElement("Iocp");
	if(eleIocp == NULL) return false;

	m_nThreadCount = TiAsLong(eleIocp, "ThreadCount");

	const TiXmlElement* eleServer = eleSetting->FirstChildElement("Server");
	if(eleServer == NULL) return false;

	m_strIP	= TiAsString(eleServer, "IP");
	m_nPort	= TiAsLong(eleServer, "PORT");

	const TiXmlElement* eleTestClient = eleSetting->FirstChildElement("TestClient");
	if(eleTestClient == NULL) return false;

	const TiXmlElement* eleInfo = eleTestClient->FirstChildElement("Info");
	while(eleInfo)
	{
		int nTestClientCount	= TiAsLong(eleInfo, "Count");
		ACTION_TYPE eActionType = (ACTION_TYPE)TiAsLong(eleInfo, "Action");	

		m_stl_vector_TestClientInfo.push_back(STestClientInfo(nTestClientCount, eActionType));
		eleInfo = eleInfo->NextSiblingElement();
	}	
	return true;
}
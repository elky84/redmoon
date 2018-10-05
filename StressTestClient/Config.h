#pragma once

#include "tinyxml.h"
#include <vector>

enum ACTION_TYPE
{
	ACTIONTYPE_NONE = -1,
	ACTIONTYPE_TRY_CONNECT_DISCONNECT = 0, //접속/해제 동작 시도
	ACTIONTYPE_SEND_RANDOM_PACKET = 1, //랜덤 패킷 전송
};

struct STestClientInfo
{
	int m_nTestClientCount;
	ACTION_TYPE m_eActionType;
	STestClientInfo(int nTestClientCount, ACTION_TYPE eActionType) : m_nTestClientCount(nTestClientCount), m_eActionType(eActionType)
	{

	}
};

typedef std::vector<STestClientInfo> STL_VECTOR_TESTCLIENTINFO;

class CConfig
{
public:
	STATIC_SINGLETON_PTR(CConfig);

	bool Load();

	inline const std::string& GetIP(){return m_strIP;}
	inline const int& GetPort(){return m_nPort;}
	inline const int& GetThreadCount(){return m_nThreadCount;}
	inline STL_VECTOR_TESTCLIENTINFO& GetVectorTestClientInfo(){return m_stl_vector_TestClientInfo;}

private:
	CConfig();
	~CConfig();

private:
	std::string m_strIP;
	int m_nPort;

	int m_nThreadCount;

	STL_VECTOR_TESTCLIENTINFO m_stl_vector_TestClientInfo;
};
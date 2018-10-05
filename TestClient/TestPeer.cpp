#include "stdafx.h"
#include "TestPeer.h"

CTestPeer::CTestPeer()
{

}

CTestPeer::~CTestPeer()
{

}


void CTestPeer::OnConnect()
{
	LOG_INFO(_T("%s"), __TFUNCTION__);
	
}

void CTestPeer::OnDisconnect()
{
	LOG_INFO(_T("%s"), __TFUNCTION__);
	Reuse();
}

void CTestPeer::OnReceive(int nLen, void* data)
{
	LOG_INFO(_T("%s [%d, %d]"), __TFUNCTION__, nLen, GetTickCount());
}

void CTestPeer::Act()
{
	char szBuffer[1000] = "";
	static int i = 0;
	sprintf(szBuffer, "test %d", i++);
	Send((int)strlen(szBuffer) + 1, szBuffer);
}
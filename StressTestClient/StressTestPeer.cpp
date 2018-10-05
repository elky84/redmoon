#include "stdafx.h"
#include "StressTestPeer.h"

#include "RandomPacket.h"

CStressTestPeer::CStressTestPeer(ACTION_TYPE & refAction) : m_refAction(refAction)
{
	m_pcPacketInterface.Reset(new CRandomPacket);
}

void CStressTestPeer::OnConnect()
{
	LOG_INFO("%s", __TFUNCTION__);

	if(m_refAction == ACTIONTYPE_TRY_CONNECT_DISCONNECT)
		Disconnect();
}

void CStressTestPeer::OnDisconnect()
{
	LOG_INFO("%s", __TFUNCTION__);	
	Reuse();
}

void CStressTestPeer::OnReceive(int nLen, void* data)
{
	//LOG_INFO("%s [%d, %d]", __TFUNCTION__, nLen, GetTickCount());
}

void CStressTestPeer::Act()
{
	if(m_refAction == ACTIONTYPE_SEND_RANDOM_PACKET)
	{
		SPacketHeader sPacket = m_pcPacketInterface->GeneratePacket();
		Send(sPacket.m_nSize, sPacket.m_pData);
	}
}
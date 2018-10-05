#include "stdafx.h"
#include "RandomPacket.h"

CRandomPacket::CRandomPacket()
{

}

CRandomPacket::~CRandomPacket()
{

}

SPacketHeader CRandomPacket::GeneratePacket()
{
	Redmoon::PACKET_SIZE nSize = 0; //unsigned형이라 가정한다.
	nSize = rand() % --nSize;
	
	char* pBuffer = new char[nSize];
	for(int i = 0; i < nSize; i++)
	{
		pBuffer[i] = (char)(rand() % 255); 
	}

	return SPacketHeader(nSize, pBuffer);
}
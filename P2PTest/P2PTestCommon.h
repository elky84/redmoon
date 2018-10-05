#pragma once

#include "P2PCommon.h"

enum P2P_TEST_COMMAND
{ 
	P2PCOMMAND_CHAT = 0,
};

struct P2PHeader : public Redmoon::RUDPHeader
{
private:
	P2P_TEST_COMMAND Command;
public:
	P2PHeader(P2P_TEST_COMMAND com, short len) : RUDPHeader(Redmoon::RUDPCOMMAND_COMMAND, len), Command(com)
	{
	}

	P2P_TEST_COMMAND GetCommand(){return Command;}
};

struct P2PChat : public P2PHeader
{
	P2PChat();
	P2PChat(short len) : P2PHeader(P2PCOMMAND_CHAT, len + sizeof(*this))
	{

	}

	char* GetStr()
	{
		return (char*)this + sizeof(P2PHeader);
	}
};
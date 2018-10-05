#include "P2PRelayConnector.h"
#include "P2PRelayPeer.h"
#include "P2PRelayCommon.h"

namespace Redmoon
{

/**
@brief 생성자. CP2PRealyPeer를 IO담당 피어로 사용.
*/
CP2PRelayConnector::CP2PRelayConnector() : CConnectorEventSelect(new CP2PRelayPeer)
{
}

/**
@brief 소멸자. 아무일도 안함.
*/
CP2PRelayConnector::~CP2PRelayConnector()
{
}

/**
@brief 그룹 입장 요청 메소드
*/
void CP2PRelayConnector::Enter(int nGroupNo)
{
	PREnter sData(nGroupNo);
	Send(sizeof(sData), &sData);
}

/**
@brief 그룹 퇴장 요청 메소드
*/
void CP2PRelayConnector::Exit()
{
	PRExit sData;
	Send(sizeof(sData), &sData);
}

/**
@brief 주기적으로 불려지는 메소드. 핑을 날린다.
*/
void CP2PRelayConnector::Act()
{
	PRPing sData;
	Send(sizeof(sData), &sData);
}

} //namespace Redmoon
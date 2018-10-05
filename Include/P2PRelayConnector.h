#pragma once

#include "Singleton.h"
#include "ConnectorEventSelect.h"

namespace Redmoon
{

/**
@brief 릴레이 서버로 접속할때 쓰는 커넥터.
@desc EventSelect 기능을 사용해서 구현되었다. AsyncSelect를 원할시 직접 구현해서 사용하면 된다.
*/
class CP2PRelayConnector : public CConnectorEventSelect, public CSingleton<CP2PRelayConnector>
{
public:
	///생성자
	CP2PRelayConnector();

	///소멸자
	virtual ~CP2PRelayConnector();

	///그룹 입장
	void Enter(int nGroupNo);

	///그룹 퇴장
	void Exit();

	///주기적으로 불려지는 메소드
	void Act();
};

inline CP2PRelayConnector*  GetP2PRelayConnector()
{
	return CP2PRelayConnector::InstancePtr();
}

} //namespace Redmoon

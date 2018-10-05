#pragma once

#include "Pipe.h"

namespace Redmoon
{

/**
@brief 파이프 클라이언트 기능을 구현한 클래스. 
*/
class CPipeClient : public CPipe
{
public:
	///생성자.
	CPipeClient(const tstring& strName, CPeer* pcPeer);

	///Connect 시도 메소드
	bool Connect();

	///Disconnect 시도 메소드
	virtual void Disconnect();

	///주기적인 동작을 구현한 메소드
	virtual void Act();

	/// 접속 해제시 이벤트 메소드
	virtual void OnDisconnect();
};

} //namespace Redmoon
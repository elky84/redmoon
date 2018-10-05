#pragma once

#include "AutoPtr.h"
#include "ManualPtr.h"

#pragma comment(lib,"mswsock.lib") 

namespace Redmoon
{

class CSessionManager;
class CListener;
class CSession;

/**
@brief Accept 기능을 전담 하는 Class.
*/
class CAcceptor
{
public: ///생성자체가 friend에서만 이뤄지므로, 다른 메소드들은 public으로 해도 괜찮음.
	///생성자. 
	CAcceptor(CListener* pcListener);

	///실질적인 Accept 메소드
	bool Accept(CSession* pcSession);

	///소멸자
	virtual ~CAcceptor();

private:
	///CListener의 포인터를 가진다. 소유권은 가지지 않기에 CManualPtr로 선언되었다.
	CManualPtr<CListener> m_pcListener;
};

}
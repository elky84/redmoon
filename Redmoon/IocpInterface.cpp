#include "IocpInterface.h"
#include "ObjectManager.h"

namespace Redmoon
{

/**
@brief 생성자. 
*/
CIocpInterface::CIocpInterface()
{
	CObjectManager<CIocpInterface>::InstancePtr()->Register(this);
}

/**
@brief 소멸자. 
*/
CIocpInterface::~CIocpInterface()
{
}

/**
@brief IOCP핸들을 생성해서 리턴한다.
@return IOCP핸들
*/
HANDLE CIocpInterface::CreateIOCPHandle()
{
	HANDLE hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if(hIOCP == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR(_T("%s CreateIoCompletionPort() m_hIOCP Fail."), __TFUNCTION__);
	}
	return hIOCP;
}

} //namespace Redmoon

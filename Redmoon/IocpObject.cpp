#include "IocpObject.h"

#include "IocpCommon.h"

#include "ObjectManager.h"
#include "SessionManager.h"
#include "Reactor.h"

#include "IocpThreadManager.h"
#include "IocpInterface.h"

namespace Redmoon
{

/**
@brief »ý¼ºÀÚ. »ý¼º ¼ø¼­¿¡ ¸ÂÃç new.
*/
CIocpObject::CIocpObject()
{
	new CObjectManager<CIocpInterface>;
	new CIocpThreadManager;
	new CSessionManager;
	new CReactor;
}

/**
@brief ¼Ò¸êÀÚ. ¼Ò¸ê ¼ø¼­¿¡ ¸ÂÃç delete.
*/
CIocpObject::~CIocpObject()
{
	CReactor::InstancePtr()->Release();
	SAFE_DELETE_SINGLETON(CObjectManager<CIocpInterface>::InstancePtr());
	SAFE_DELETE_SINGLETON(CReactor::InstancePtr());
	SAFE_DELETE_SINGLETON(CSessionManager::InstancePtr());
	SAFE_DELETE_SINGLETON(CIocpThreadManager::InstancePtr());
}

} //namespace Redmoon

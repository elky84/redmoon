#pragma once

#include <list>
#include "Singleton.h"
#include "SmartPtr.h"

namespace Redmoon
{

/**
@brief 오브젝트들을 소유하고, 자신이 delete 될때, 해당 객체들을 모두 delete해주는 유틸 클래스. 
@desc CSmartPtr로 랩핑되어 있기 때문에, 다른 곳에서 참조 중일때는 사라지지 않는다. 또한 Singleton이지만 template이므로, 타입마다 객체가 별도로 생성된다.
*/
template <typename T>
class CObjectManager : public CSingleton<CObjectManager>
{
public:
	/**
	@brief 객체를 등록해주는 메소드
	@param pcObject 등록할 객체
	*/
	void Register(T* pcOjbect)
	{
		m_stl_list_Object.push_back( pcOjbect );
	}

private:
	///관리할 객체의 목록.
	std::list< CSmartPtr<T> > m_stl_list_Object;
};

} //namespace Redmoon
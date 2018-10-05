#pragma once

#include <list>
#include "Singleton.h"
#include "SmartPtr.h"

namespace Redmoon
{

/**
@brief ������Ʈ���� �����ϰ�, �ڽ��� delete �ɶ�, �ش� ��ü���� ��� delete���ִ� ��ƿ Ŭ����. 
@desc CSmartPtr�� ���εǾ� �ֱ� ������, �ٸ� ������ ���� ���϶��� ������� �ʴ´�. ���� Singleton������ template�̹Ƿ�, Ÿ�Ը��� ��ü�� ������ �����ȴ�.
*/
template <typename T>
class CObjectManager : public CSingleton<CObjectManager>
{
public:
	/**
	@brief ��ü�� ������ִ� �޼ҵ�
	@param pcObject ����� ��ü
	*/
	void Register(T* pcOjbect)
	{
		m_stl_list_Object.push_back( pcOjbect );
	}

private:
	///������ ��ü�� ���.
	std::list< CSmartPtr<T> > m_stl_list_Object;
};

} //namespace Redmoon
#pragma once

#include <assert.h>

namespace Redmoon
{

/**
@brief 단일 인스턴스를 생성해주는 템플릿 클래스. 싱글턴이라 부름.
*/
template <typename T> 
class CSingleton
{
	///싱글턴 포인터
	static T* m_pcPtr;
public:
	///생성자. 싱글턴 객체의 포인터를 저장해둔다
	CSingleton(void)
	{
		assert(!m_pcPtr) ;
		intptr_t offset = (intptr_t)(T*)1 - (intptr_t)(CSingleton<T>*)(T*)1 ;
		m_pcPtr = (T*)((intptr_t)this + offset) ;
	}

	///소멸자. 포인터 위치를 널로 지정.
	virtual ~CSingleton(void)
	{
		m_pcPtr = 0 ;
	}

	///싱글턴 객체 참조자를 반환한다.
	static T& Instance(void)
	{
		return (*m_pcPtr) ;
	}

	///싱글턴 객체 포인터를 반환한다.
	static T* InstancePtr(void)
	{
		return (m_pcPtr) ;
	}
};

template <typename T>T* CSingleton<T>::m_pcPtr = 0;

///static 변수로써 싱글턴을 구현하게 해주는 매크로. 참조자로 반환하게 함
#define STATIC_SINGLETON_REF( cls ) \
public: \
	static cls& Instance()\
	{\
		static cls s_Instance; \
		return s_Instance; \
	}

///static 변수로써 싱글턴을 구현하게 해주는 매크로. 포인터로 반환하게 함
#define STATIC_SINGLETON_PTR( cls ) \
public: \
	static cls* InstancePtr()\
	{\
		static cls s_Instance; \
		return &s_Instance; \
	}

///메소드 호출시 싱글턴이 생성되고, 소멸 시점도 제어 할 수 있게 제공하는 매크로
#define DECLARE_SINGLETON( cls ) \
public: \
	static cls* s_pInstance; \
	static cls* CreateInstance() \
	{ \
		if( !s_pInstance ) \
			s_pInstance = new cls(); \
		return s_pInstance; \
	} \
	static void ReleaseInstance() \
	{ \
		SAFE_DELETE( s_pInstance ); \
	} \
	static cls* InstancePtr() { return s_pInstance; } \
	static cls& Instance() { return *s_pInstance; }

///스태틱 싱글턴 멤버 초기화를 편리하게 해주는 매크로
#define IMPLEMENT_SINGLETON( cls ) \
	cls* cls::s_pInstance = NULL;

} //namespace Redmoon
#ifndef SmartPtr_H
#define SmartPtr_H

#include "Common.h"
#include <assert.h>

template <class T> class CSmartPtr;

/**
@brief IRefCount 참조 횟수 관리를 위한 인터페이스. 클래스들은 이것을 직접 구현해도 되고, 아니면 내부적으로 IRefCount를 구현하고 있는 CSmartPtr을 사용해도 된다.
*/
template <class T> class IRefCount {
	friend class CSmartPtr<T>;
protected:
	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual T* GetPtr() const = 0;
};

/**
@brief IRefCount의 표준적인 구현. 이것을 상속하기만 하면 참조 횟수 관리 기능이 생긴다. class CMyObject : public IrefCountImpl<CMyObjct> { ... };
@desc 개별 클래스가 IRefCount를 꼭 구현할 필요는 없지만, 직접 구현함으로써 메모리 단편화를 줄일 수도 있다.
*/
template <class T> class IRefCountImpl
: public IRefCount<T>
{
private:
	int m_nCount;

protected:
	virtual void AddRef() { m_nCount++; }
	virtual void Release()
	{
		assert( m_nCount>=0);
		m_nCount--;
		if( m_nCount<=0 )
		{
			Destroy();
		}
	}
	virtual T* GetPtr() const { return ( (T*)this ); }
	virtual void Destroy() { if(GetPtr()!=NULL) delete GetPtr(); }

	IRefCountImpl() { m_nCount = 0; }
};

/**
@brief 스마트 포인터 클래스. 참조 값이 1 이상일 때에 new되고, 0 이하가 되면 delete한다.
*/
template <class T> 
class CSmartPtr
{
private:
	IRefCount<T> *m_RefCount;

	/**
	@brief IRefCount를 직접 구현하지 않는 클래스를 위한 내부구현.CSmartPtr은 이 내부 구현과 클래스 T의 IRefCount 구현중 적절한 것을 자동적으로 선택한다. 
	*/
	class RefCounter : public IRefCountImpl<T> {
	private:
		T *m_Ptr;
	protected:
		virtual T* GetPtr() const { return m_Ptr; }
		virtual void Destroy() { delete this; }

	public:
		RefCounter( T* ptr ) { m_Ptr = ptr; }
		virtual ~RefCounter() { IRefCountImpl<T>::Destroy(); }
	};
	//  T가 IRefCount를 구현하지 않았다면 이 메서드가 호출된다.
	void Assign( void *ptr )
	{
		if( ptr==NULL )
			Assign( (IRefCount<T> *)NULL );
		else
		{
			Assign( new RefCounter( static_cast<T*>(ptr) ) );
		}
	}

	// T가 IRefCount를 구현하고 있다면 Assign(void *ptr) 대신
	// 이 메서드가 호출된다.
	// 이를 통해서 메모리 사용이 좀더 최적화된다.
	void Assign( IRefCount<T> *refcount )
	{
		if( refcount != NULL )
			refcount->AddRef();
		IRefCount<T> *oldref = m_RefCount;
		m_RefCount = refcount;
		if( oldref!=NULL )
			oldref->Release();
	}
public:

	CSmartPtr() { m_RefCount = NULL; }
	CSmartPtr( T *ptr ) { m_RefCount = NULL; Assign(ptr); }
	CSmartPtr(const CSmartPtr &sp) { m_RefCount = NULL; Assign(sp.m_RefCount); }
	virtual ~CSmartPtr() { Assign( (IRefCount<T> *)NULL); }

	T* GetPtr() const { return m_RefCount == NULL ? NULL : m_RefCount->GetPtr(); }

	// 배정 연산자들
	CSmartPtr& operator = (const CSmartPtr &sp) { Assign(sp.m_RefCount); return *this; }
	CSmartPtr& operator = (T* ptr) { Assign(ptr); return *this; }

	// T접근 및 const 변환
	T* operator -> () { assert(GetPtr() != NULL); return GetPtr(); }
	operator T* () const { return GetPtr(); }

	// 기타 편의성 연산자들
	bool operator ! () { return GetPtr() == NULL; }
	bool operator == (const CSmartPtr &sp) { return GetPtr() == sp.GetPtr(); }
	bool operator != (const CSmartPtr &sp) { return GetPtr() != sp.GetPtr(); }
};

#endif
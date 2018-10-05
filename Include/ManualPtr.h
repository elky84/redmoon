#pragma once

#include "AutoPtr.h"

/**
@brief 포인터 소유권은 없지만, 포인터 주소를 가지고 사용할 수 있도록 한 클래스다. 동적할당 하지 않은 객체나, 동적할당한 객체의 포인터 사본을 관리할 때 유용하다.
*/
namespace Redmoon
{
	class CAutoPtr;

	template<typename T>
	class CManualPtr
	{	
	private:
		///참조용으로만 가지는 포인터다.
		T* m_Ptr;

	public:
		/**
		@brief 생성자. 기본 포인터나, 널 포인터를 파라미터로 받아, 초기값으로 지정한다.
		@param ptr 기본 포인터. 기본 값은 널 포인터.
		*/
		CManualPtr(T* ptr = 0) : m_Ptr(ptr)
		{

		}

		/**
		@brief 생성자. CManualPtr 의 참조자를 받아서 포인터를 복사한다.
		@param refManualPtr CManualPtr의 참조자
		*/
		CManualPtr(CManualPtr& refManualPtr) : m_Ptr(0)
		{
			operator =(refManualPtr);
		}

		/**
		@brief 생성자. CAutoPtr 의 참조자를 받아서 포인터를 복사한다.
		@param refAutoPtr CAutoPtr의 참조자
		*/
		CManualPtr(CAutoPtr<T>& refAutoPtr) : m_Ptr(0)
		{
			operator =(refAutoPtr);
		}

		/**
		@brief = 연산자 중복 정의. 기본 포인터를 받아서 포인터를 복사한다.
		@param ptr 기본 포인터
		*/
		void operator =(T* ptr)
		{
			Reset(ptr);
		}

		/**
		@brief = 연산자 중복 정의. CManualPtr 의 참조자를 받아서 포인터를 복사한다.
		@param refManualPtr CManualPtr의 참조자
		*/
		void operator =(CManualPtr& refManualPtr)
		{
			Reset(refManualPtr.Get());
		}

		/**
		@brief = 연산자 중복 정의. CAutoPtr 의 참조자를 받아서 포인터를 복사한다.
		@param refAutoPtr CAutoPtr의 참조자
		*/
		void operator =(CAutoPtr<T>& refAutoPtr)
		{
			Reset(refAutoPtr.Get());
		}

		/**
		@brief -> 연산자 중복 정의. 
		@return 기본 포인터를 반환하게 함으로써, 기본 포인터 사용하는 것과 같은 동작을 유도한다.
		*/
		T* operator ->()
		{
			return m_Ptr;
		}

		/**
		@brief 기본 포인터를 반환한다.
		@return 포인터를 반환.
		*/
		T *Get()
		{
			return m_Ptr;
		}

		/**
		@brief 널 포인터로 초기화한다
		*/
		void Release()
		{
			Reset(0);
		}

		/**
		@brief 포인터 재지정. 기본 포인터를 받아서 포인터를 복사한다.
		@param ptr 기본 포인터
		*/
		void Reset(T* ptr)
		{
			m_Ptr = ptr;
		}

		/**
		@brief 포인터 재지정. CManualPtr 의 참조자를 받아서 포인터를 복사한다.
		@param refManualPtr CManualPtr의 참조자
		*/
		void Reset(CManualPtr& refManualPtr)
		{
			operator=(refManualPtr);
		}

		/**
		@brief 포인터 재지정. CAutoPtr 의 참조자를 받아서 포인터를 복사한다.
		@param refAutoPtr CAutoPtr의 참조자
		*/
		void Reset(CAutoPtr<T>& refAutoPtr)
		{
			operator=(refAutoPtr);
		}
	};
} //namespace Redmoon

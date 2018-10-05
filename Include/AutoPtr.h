#pragma once

/**
@brief 포인터 소유권을 가지는 클래스다. 동적할당한 객체에 대한 포인터만 넘겨야 한다.
*/
namespace Redmoon
{
	template<typename T>
	class CAutoPtr
	{	
	private:
		///해당 포인터의 
		T* m_Ptr;

	public:
		/**
		@brief 생성자. 기본 포인터나, 널 포인터를 파라미터로 받아, 초기값으로 지정한다.
		@param ptr 기본 포인터. 기본 값은 널 포인터.
		*/
		CAutoPtr(T* ptr = 0) : m_Ptr(ptr)
		{

		}

		/**
		@brief 생성자. CAutoPtr 의 참조자를 받아서 포인터를 복사한다.
		@param refAutoPtr CAutoPtr의 참조자
		*/
		CAutoPtr(CAutoPtr& refAutoPtr) : m_Ptr(0)
		{
			operator=(refAutoPtr);
		}

		/**
		@brief 소멸자. 소유하고 있는 포인터를 동적할당 해제한다.
		*/
		~CAutoPtr()
		{
			if(m_Ptr)
				delete m_Ptr;
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
		@brief = 연산자 중복 정의. CAutoPtr 의 참조자를 받아서 해당 객체의 포인터 소유권을 이전한다.
		@param refAutoPtr CAutoPtr의 참조자
		*/
		void operator =(CAutoPtr& refAutoPtr)
		{
			Reset(refAutoPtr.Get());
			refAutoPtr.Release();
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
		@brief 소유권을 포기하고, 해당 포인터 반환
		@return 해당 포인터 반환
		*/
		T* Release()
		{
			T* ptr = m_Ptr;
			m_Ptr = 0;
			return ptr;
		}

		/**
		@brief 포인터 재지정. 기본 포인터를 받아서 포인터를 복사한다.
		@param ptr 기본 포인터
		*/
		void Reset(T* ptr = 0)
		{
			if(m_Ptr)
				delete m_Ptr;

			m_Ptr = ptr;
		}

		/**
		@brief 포인터 재지정. CAutoPtr 의 참조자를 받아서 해당 객체의 포인터 소유권을 이전한다.
		@param refAutoPtr CAutoPtr의 참조자
		*/
		void Reset(CAutoPtr& refAutoPtr)
		{
			operator=(refAutoPtr);
		}
	};
} //namespace Redmoon

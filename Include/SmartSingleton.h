#pragma once

namespace Redmoon
{

/**
@brief 상속한 객체가 하나라도 존재할 때, template으로 넘어온 객체가 new 되고, 해당 객체가 사라지면 delete되는 스마트한 싱글턴 클래스.
*/
template <typename T>
class CSmartSingleton
{
private:
	///참조 값
	static int m_nRefCount;

	///스마트 포인터 오브젝트 포인터
	static T* m_pcPtr;

protected:
	/**
	@brief 생성자. 객체가 처음 생성될 때만 new한다.	상속으로만 사용가능하게 하기 위해서 protected이다.
	*/
	CSmartSingleton()
	{
		if(++m_nRefCount == 1)
		{
			m_pcPtr = new T;
		}
	}

	/**
	@brief 소멸자. 참조 값이 0이 되면 객체를 delete한다.
	*/
	virtual ~CSmartSingleton()
	{
		if(--m_nRefCount == 0)
		{
			delete m_pcPtr;
			m_pcPtr = 0;
		}
	}

	/**
	@brief 스마트 싱글턴 오브젝트 반환하는 메소드
	@return 스마트 싱글턴으로 생성된 오브젝트 포인터
	*/
	static T* GetSmartSingleton()
	{
		return m_pcPtr;
	}
};

template <typename T> int CSmartSingleton<T>::m_nRefCount = 0;
template <typename T> T* CSmartSingleton<T>::m_pcPtr = 0;

} //namespace Redmoon
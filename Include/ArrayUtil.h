#pragma once


namespace Redmoon
{

typedef int ARRAYUTIL_INDEX;

template <typename T>
class CArrayUtil
{
public:
	CArrayUtil(T* arrayObject, const ARRAYUTIL_INDEX nArraySize, const T& defaultValue = 0) : m_arrayObject(arrayObject), m_nArraySize(nArraySize), m_defaultValue(defaultValue), m_nUsingCount(0)
	{
		for(ARRAYUTIL_INDEX i = 0; i < m_nArraySize; i++)
		{
			m_arrayObject[i] = m_defaultValue;
		}
	}

	ARRAYUTIL_INDEX Add(const T& refObject)
	{
		for(ARRAYUTIL_INDEX i = 0; i < m_nArraySize; i++)
		{
			if(m_arrayObject[i] == m_defaultValue)
			{
				m_arrayObject[i] = refObject;
				m_nUsingCount++;
				return i;
			}
		}
		return -1;
	}

	ARRAYUTIL_INDEX Remove(const T& refObject)
	{
		for(ARRAYUTIL_INDEX i = 0; i < m_nArraySize; i++)
		{
			if(m_arrayObject[i] == refObject)
			{
				m_arrayObject[i] = m_arrayObject[m_nUsingCount - 1];
				m_arrayObject[m_nUsingCount - 1] = m_defaultValue;
				m_nUsingCount--;
				return i;
			}
		}
		return -1;
	}

	inline int GetUsingCount(){return m_nUsingCount;}

private:
	int m_nUsingCount;
	T* m_arrayObject;
	const ARRAYUTIL_INDEX m_nArraySize;
	const T m_defaultValue;
};

} //namespace Redmoon
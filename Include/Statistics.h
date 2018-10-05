#pragma once

#include "Common.h"

namespace Redmoon
{
	template <typename T>
	class CStatistics
	{
	public:
		CStatistics(const tstring& strName) : m_strName(strName), m_Average(0), m_Min(0), m_Max(0)
		{

		}

		~CStatistics()
		{
			LOG_INFO(_T("Statistics [Name:%s][Avg:%d][Min:%d][Max:%d]"), m_strName.c_str(), m_Average, m_Min, m_Max);
		}

		void Record(const T& refData)
		{
			m_Average = (m_Average + refData) / 2;

			m_Min = min(m_Min, refData);
			m_Max = max(m_Max, refData);
		}

		const T& GetAverage(){return m_Average;}

		const T& GetMin(){return m_Min;}

		const T& GetMax(){return m_Max;}

	private:
		T m_Average;

		T m_Min;

		T m_Max;

		const tstring m_strName;
	};
} //namespace Redmoon
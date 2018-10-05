#pragma once

#include "Str.h"

namespace Redmoon
{
	class CIcon
	{
	public:
		CIcon(const tstring& strName) : m_hIcon(::ExtractIcon(AfxGetInstanceHandle(), strName.c_str(), 0)), m_strName(strName), m_nIconCount((size_t)::ExtractIcon(AfxGetInstanceHandle(), strName.c_str(), -1))
		{
		}

		~CIcon()
		{
			::DestroyIcon(m_hIcon);
		}

		HICON GetIcon(){return m_hIcon;}

	private:
		const tstring m_strName;
		HICON m_hIcon;
		size_t m_nIconCount;
	};
} //namespace Redmoon
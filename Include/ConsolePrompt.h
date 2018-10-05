#pragma once

#include "PromptInterface.h"

namespace Redmoon
{

class CConsolePrompt : public CPromptInterface
{
public:
	CConsolePrompt(const tstring& strNotice = _T("")) : CPromptInterface(strNotice)
	{
		Start();
	}

	virtual ~CConsolePrompt()
	{

	}

private:
	virtual void Begin()
	{
		std::cout<< m_strNotice << std::endl;
	}

	virtual void Act()
	{
		std::cout << _T("CommandPrompt : ");
		std::cin >> m_strCommand;
		Call();
	}

	virtual void SetNotice(const tstring& strNotice)
	{
		__super::SetNotice(strNotice);
		std::cout<< m_strNotice << std::endl;
	}
};

} //namespace Redmoon
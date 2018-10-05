#pragma once

#include "Thread.h"
#include "Str.h"
#include "FunctionPtr.h"
#include "PromptInterface.h"
#include "AutoPtr.h"

namespace Redmoon
{

class CPromptInterface : public CThread, public CSingleton<CPromptInterface>
{
public:
	CPromptInterface(const tstring& strNotice = _T("")) : m_strNotice(strNotice)
	{
	}

	virtual ~CPromptInterface()
	{
	}

	void SetCallback(const tstring& strNotice, CCallBack* pcCallback)
	{
		SetNotice(strNotice);
		m_pcCallback = pcCallback;
	}

	void SetCallback(CCallBack* pcCallback)
	{
		m_pcCallback = pcCallback;
	}

	virtual bool Call()
	{
		if(m_pcCallback.Get())
		{
			if(true == m_pcCallback->Interpret(&GetCommand()))
			{
				m_pcCallback = NULL;
				return true;
			}
		}
		SetNotice(_T("Not Defined Callback"));
		return false;
	}

	virtual void SetNotice(const tstring& strNotice)
	{
		m_strNotice = strNotice;
	}

protected:
	tstring& GetCommand() {return m_strCommand;}

protected:
	tstring m_strCommand;
	tstring m_strNotice;

	CAutoPtr<CCallBack> m_pcCallback;
};

} //namespace Redmoon
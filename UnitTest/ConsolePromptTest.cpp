#include "TestCommon.h"
#include "ConsolePrompt.h"

namespace Redmoon
{
	class CConsoleCallback : public CCallBack
	{
	public:
		CConsoleCallback()
		{
			CConsolePrompt::InstancePtr()->SetCallback(SetProc(this, CConsoleCallback::Parse));
		}

		virtual bool Parse(void* pArgument)
		{
			tstring strCommand = (tstring)(*(tstring*)pArgument);
			if(strCommand == _T("quit"))
			{
				CConsolePrompt::InstancePtr()->Stop();
				return true;
			}
			return false;
		}
	};

	TEST(CONSOLEPROMPT_TEST)
	{
		CConsolePrompt console(_T("Type quit to exit."));
		CConsoleCallback callback;
		while(console.isActive())
		{
			Sleep(1);
		}
	}
} //namespace Redmoon
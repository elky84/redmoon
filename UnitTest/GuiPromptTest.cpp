#include "TestCommon.h"
#include "GuiPrompt.h"

namespace Redmoon
{
	class CGuiCallback : public CCallBack
	{
	public:
		CGuiCallback()
		{
			CGuiPrompt::InstancePtr()->SetCallback(SetProc(this, CGuiCallback::Parse));
		}

		bool Parse(void* pArgument)
		{
			tstring strCommand = (tstring)(*(tstring*)pArgument);
			if(strCommand == _T("quit"))
			{
				CGuiPrompt::InstancePtr()->Stop();
				return true;
			}
			return false;
		}
	};

	TEST(GUIPROMPT_TEST)
	{
		CGuiPrompt gui(_T("Type quit to exit."));
		CGuiCallback callback;
		while(gui.isActive())
		{
			Sleep(1);
		}
	}
} //namespace Redmoon

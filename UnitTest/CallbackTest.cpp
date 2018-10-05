#include "TestCommon.h"
#include "FunctionPtr.h"
#include "AutoPtr.h"

namespace Redmoon
{

	//CScript를 템플릿 클래스로 만들었으며 생성시 객체와 멤버 함수를 하도록 구조를 바꿨습니다. 이렇게 하면 아래와 같이 사용할 수 있습니다.

	class CTest
	{
	public:
		bool fun(void* pArgument)
		{
			tstring strCommand = (tstring)(*(tstring*)pArgument);
			cout << strCommand << endl;
			return true;
		}
	};

	TEST(CALLBACK_TEST)
	{
		tstring s(_T("test"));
		CTest ct;
		CAutoPtr<CCallBack> pcCallback = SetProc(&ct, CTest::fun);
		pcCallback->Interpret(&s);
	}
} //namespace Redmoon

#include "TestCommon.h"
#include "FunctionPtr.h"
#include "AutoPtr.h"

namespace Redmoon
{

	//CScript�� ���ø� Ŭ������ ��������� ������ ��ü�� ��� �Լ��� �ϵ��� ������ �ٲ���ϴ�. �̷��� �ϸ� �Ʒ��� ���� ����� �� �ֽ��ϴ�.

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

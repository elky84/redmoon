#include "TestCommon.h"
#include "Str.h"

namespace Redmoon
{
	TEST(STL_STRING_TEST)
	{
		tstring str = _T("Test");
		str.erase(--str.end()); //�� ������ ���� �����
		CHECK(_T("Tes" == str));
	}
} //namespace Redmoon
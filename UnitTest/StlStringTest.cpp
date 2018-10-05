#include "TestCommon.h"
#include "Str.h"

namespace Redmoon
{
	TEST(STL_STRING_TEST)
	{
		tstring str = _T("Test");
		str.erase(--str.end()); //맨 마지막 글자 지우기
		CHECK(_T("Tes" == str));
	}
} //namespace Redmoon
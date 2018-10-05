#include "TestCommon.h"
#include "ArrayUtil.h"

namespace Redmoon
{
	TEST(ARRAYUTIL_TEST)
	{
		int nArray[10];
		CArrayUtil<int> arrayUtilTest(nArray, _countof(nArray), -1);
		for(int i = 0; i < _countof(nArray); i++)
		{
			CHECK(-1 != arrayUtilTest.Add(i));
		}

		for(int i = 0; i < _countof(nArray); i++)
		{
			CHECK(-1 != arrayUtilTest.Remove(i));
		}

		for(int i = 0; i < _countof(nArray); i++)
		{
			CHECK(-1 != arrayUtilTest.Add(i));
		}

		for(int i = (_countof(nArray) -1) / 2; 0 <= i; i--)
		{
			CHECK(-1 != arrayUtilTest.Remove(i));
		}

		for(int i = 0; i < _countof(nArray) / 2; i++)
		{
			CHECK(-1 != arrayUtilTest.Add(i));
		}

		for(int i = 0; i < _countof(nArray); i++)
		{
			LOG_INFO(_T("ARRAYUTIL_TEST [%d, %d]"), i, nArray[i]);
		}
	}
}
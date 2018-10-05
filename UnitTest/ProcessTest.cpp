#include "TestCommon.h"
#include "ProcessUtil.h"

namespace Redmoon
{
	TEST(PROCESS_TEST)
	{
		CProcess process(_T("notepad"));
		CHECK(process.Create());

		while(process.Observe())
		{
			process.Close();
		}
	}
} //namespace Redmoon
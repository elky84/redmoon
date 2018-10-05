#include "TestCommon.h"
#include "Statistics.h"

#include <time.h>

TEST(STATISTICS_TEST)
{
	srand((unsigned int)time(NULL));
	Redmoon::CStatistics<int> nStatistics(_T("int"));
	for(int i = 0; i < 1000; ++i)
	{
		nStatistics.Record(rand() % 100);
	}
}
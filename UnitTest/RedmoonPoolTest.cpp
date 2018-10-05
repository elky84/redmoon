#include "TestCommon.h"

#include "MemoryPool.h"
#include "CriticalSection.h"

typedef struct SRedmoonPoolData : public Redmoon::CMemoryPool<SRedmoonPoolData>
{
	int m_nData;
} SRedmoonPoolData; 

class CRedmoonPoolTest
{
public:
	CRedmoonPoolTest();
	~CRedmoonPoolTest();

private:
	SRedmoonPoolData* m_arrData[POOL_TEST_DATA_SIZE];
};

CRedmoonPoolTest::CRedmoonPoolTest()
{
	Redmoon::CMemoryManager::InstancePtr()->Alloc(sizeof(SRedmoonPoolData), POOL_TEST_DATA_SIZE);	
	for (int i = 0; i < POOL_TEST_DATA_SIZE; ++i)
	{
		m_arrData[i] = new SRedmoonPoolData;
	}
}

CRedmoonPoolTest::~CRedmoonPoolTest()
{
	for (int i = 0; i < POOL_TEST_DATA_SIZE; ++i)
	{
		delete m_arrData[i];
	}
}

TEST_FIXTURE(CRedmoonPoolTest, RedmoonPOOL_TEST)
{
}
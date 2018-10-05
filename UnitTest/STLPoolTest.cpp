#include "TestCommon.h"
#include <memory>

typedef struct STLPoolData
{
	int m_nData;


	static void *operator new(size_t size) {
		return STL_Pool.allocate(size);
	}
	static void operator delete(void *p, size_t size) {
		STL_Pool.deallocate((STLPoolData*)p, size);
	}
	static void *operator new[](size_t size) {
		return STL_Pool.allocate(size);
	}
	static void operator delete[](void *p, size_t size) {
		STL_Pool.deallocate((STLPoolData *)p, size);
	}
	static std::allocator<STLPoolData> STL_Pool;
} STLPoolData; // has destructor with side-effects

class CSTLPoolTest
{
public:
	CSTLPoolTest();
	~CSTLPoolTest();

protected:
	STLPoolData* m_arrData[POOL_TEST_DATA_SIZE];
};

std::allocator<STLPoolData> STLPoolData::STL_Pool;

CSTLPoolTest::CSTLPoolTest()
{
	for (int i = 0; i < POOL_TEST_DATA_SIZE; ++i)
	{
		m_arrData[i] = new STLPoolData;
	}
}

CSTLPoolTest::~CSTLPoolTest()
{
	for (int i = 0; i < POOL_TEST_DATA_SIZE; ++i)
	{
		delete m_arrData[i];
	}
}

TEST_FIXTURE(CSTLPoolTest, STLPOOL_TEST)
{
	
}
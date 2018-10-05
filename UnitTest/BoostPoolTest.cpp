#include "TestCommon.h"

#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>

typedef struct BoostPoolData
{
	int m_nData;

	static void *operator new(size_t nSize)
	{
		return m_ObjectPool.malloc();
	}

	/*static void operator delete(void* pDelobj)
	{
		m_ObjectPool.free((BoostPoolData*)pDelobj);
	}*/

	static boost::object_pool<BoostPoolData> m_ObjectPool;
} BoostPoolData; // has destructor with side-effects


class CBoostPoolTest
{
public:
	CBoostPoolTest();
	~CBoostPoolTest();

private:
	BoostPoolData* m_arrData[POOL_TEST_DATA_SIZE];
};


boost::object_pool<BoostPoolData> BoostPoolData::m_ObjectPool;

CBoostPoolTest::CBoostPoolTest()
{
	//	  boost::pool<> pool(sizeof(int));
	//   for (int i = 0; i < BOOST_POOL_TEST_DATA_SIZE; ++i)
	//   {
	//       int *p = (int *) pool.malloc();        
	//       // Do something with p; don't take the time to free() it     
	//   }

	for (int i = 0; i < POOL_TEST_DATA_SIZE; ++i)
	{
		m_arrData[i] = new BoostPoolData;
	} //Do something with p; don't take the time to free() it     
}

CBoostPoolTest::~CBoostPoolTest()
{
	for (int i = 0; i < POOL_TEST_DATA_SIZE; ++i)
	{
		m_arrData[i] = NULL;
	}
}

TEST_FIXTURE(CBoostPoolTest, BOOSTPOOL_TEST)
{
	
}
// UnitTest.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "TestCommon.h"
#include "VUTPPRunner.h"

#ifdef _DEBUG
#include <crtdbg.h>

#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
#endif

int _tmain(int argc, char **argv)
{
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	for( int i = 1; i < argc; i++ )
	{
		if( strncmp( argv[i], "--vutpp:", 8 ) == 0 )
		{
			VUTPP_Runner(argv[i] + 8);
		}
	}

	return UnitTest::RunAllTests();
}
// TestServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Main.h"

#ifdef _DEBUG
#include <crtdbg.h>

#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
#endif

int _tmain(int argc, char **argv)
{
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	RUN_MAIN_CONSOLE(CMain, _T("TestServer"));
	return 0;
}


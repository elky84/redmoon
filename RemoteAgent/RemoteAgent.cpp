// RemoteAgent.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Main.h"

int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _DEBUG
	RUN_MAIN_CONSOLE(CMain, _T("RemoteAgent"));
#else
	if(argc <= 1)
	{
		RUN_MAIN_SERVICE(CMain, _T("RemoteAgent"));
	}
	else
	{
		std::string strArgv = argv[1];
		if(strArgv == "install")
			SERVICE_INSTALL_AUTOSTART(_T("RemoteAgent"));
		else if(strArgv == "uninstall")
			SERVICE_UNINSTALL(_T("RemoteAgent"));
	}
#endif
	return 0;
}
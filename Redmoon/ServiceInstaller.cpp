#include "ServiceInstaller.h"

namespace Redmoon
{

/**
@brief 서비스 설치 메소드
@return 성공 여부
*/
bool CServiceInstaller::Install(const TCHAR* szProgramName, DWORD dwServiceStartType)
{
	TCHAR szPath[MAX_PATH];
	GetModuleFileName( GetModuleHandle(NULL), szPath, MAX_PATH );

	bool ret;
	SC_HANDLE hScm, hSrv;

	if(_taccess(szPath, 0)!=0){
		LOG_ERROR(_T("Not Exist Files"));
		return false;
	}

	hScm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if(hScm == NULL){
		LOG_ERROR(_T("Not Opened SCM"));
		return false;
	}

	hSrv = CreateService(hScm, szProgramName, szProgramName, SERVICE_PAUSE_CONTINUE|SERVICE_CHANGE_CONFIG, SERVICE_WIN32_OWN_PROCESS, dwServiceStartType, SERVICE_ERROR_IGNORE, szPath, NULL, NULL, NULL, NULL, NULL);
	if(hSrv==NULL)
	{
		LOG_ERROR(_T("Install Failed."));
		ret = false;
	}
	else
	{
		LOG_ERROR(_T("Install Success."));
		CloseServiceHandle(hSrv);
		ret = true;
	}
	CloseServiceHandle(hScm);
	return ret;
}

/**
@brief 서비스 제거 메소드
@return 성공 여부
*/
bool CServiceInstaller::Uninstall(const TCHAR* szProgramName)
{
	bool ret;
	SC_HANDLE hScm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if(hScm == NULL){
		LOG_ERROR(_T("Not Opened SCM."));
		return false;
	}

	SC_HANDLE hSrv = OpenService(hScm, szProgramName, SERVICE_ALL_ACCESS);
	if(hSrv==NULL){
		CloseServiceHandle(hScm);
		LOG_ERROR(_T("Service Not Installed."));
		return false;
	}

	SERVICE_STATUS ss;
	QueryServiceStatus(hSrv, &ss);
	if(ss.dwCurrentState != SERVICE_STOPPED){
		ControlService(hSrv, SERVICE_CONTROL_STOP, &ss);
		Sleep(2000);
	}

	if(DeleteService(hSrv))
	{
		LOG_ERROR(_T("Service Delete Success."));
		ret = true;
	}
	else
	{
		LOG_ERROR(_T("Service Delete Failed."));
		ret = false;
	}
	CloseServiceHandle(hSrv);
	CloseServiceHandle(hScm);
	return ret;
}

} //namespace Redmoon
#include "EnvironmentManager.h"
#include "Event.h"
#include <stdlib.h>
#include <crtdbg.h>
#include "MiniDump.h"

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv)
{
	Redmoon::GetEnvironmentManager()->ServiceMain(argc, argv);
}

VOID WINAPI ServiceHandler(DWORD fdwControl)
{
	Redmoon::GetEnvironmentManager()->ServiceHandler(fdwControl);
}

void handle_pure_virtual_function_call()
{
	RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, NULL);
}

void handle_set_invalid_parameter_call()
{
	RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, NULL);
}

namespace Redmoon
{
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool GetDirectory(TCHAR* szFullPath, TCHAR* szBuffer)
{
	int nLen = (int)_tcslen(szFullPath) ;
	TCSNCPY(szBuffer, _tcslen(szBuffer), szFullPath) ;
	for(int i = 0 ; i < nLen ; i++)
	{
		if(szBuffer[nLen - i - 1] ==_T('\\'))
		{
			szBuffer[nLen - i - 1] = 0 ;
			return true ;
		}
	}
	return false;
}

/**
@brief 생성자. 각 변수의 초기화와 서버 구동 설정을 한다.
@param szProgramName 서버 이름
@param dwServiceStartType 서비스 시작 타입
*/
CEnvironmentManager::CEnvironmentManager(const TCHAR* szProgramName) : m_strProgramName(szProgramName), m_bPause(false), m_hSrv(NULL), 
	m_eRunType(RUNTYPE_NONE), m_bActive(false), m_pcEvent(new CEvent)
{
	CLogManager::InstancePtr()->SetLogName(szProgramName);
	_set_purecall_handler(handle_pure_virtual_function_call);

	//객체 생성을 통한, ::SetUnhandledExceptionFilter()를 호출하기 위한 세팅.
	CMiniDumper::InstancePtr();
	
#if _MSC_VER > 1400	
	_set_invalid_parameter_handler(handle_set_invalid_parameter_call);
	_CrtSetReportMode(_CRT_ASSERT, 0);
#endif //#if _MSC_VER > 1400	
}

/**
@brief 소멸자. 아무일도 하지 않음.
*/
CEnvironmentManager::~CEnvironmentManager()
{
	LOG_ALARM(_T("%s Complete"), __TFUNCTION__);
}

/**
@brief 타입에 따른 어플리케이션 초기화 메소드
@param eRunType 어플리케이션 구동 타입
*/
void CEnvironmentManager::Init(RUN_TYPE eRunType)
{
	m_eRunType = eRunType;
	m_bActive = true;

	switch(eRunType)
	{
	case RUNTYPE_CONSOLE:
		{
			_ConsoleMain();
		}
		break;
	case RUNTYPE_SERVICE:
		{
			SERVICE_TABLE_ENTRY ste[]=
			{
				{const_cast<TCHAR*>(GetProgramName()), ::ServiceMain},
				{NULL,NULL}
			};

			BOOL ret = StartServiceCtrlDispatcher(ste);
			if(!ret)
			{
				LOG_ERROR(_T("StartServiceCtrlDispatcher() Fail. [%d]"), ret);
				return;
			}
		}
		break;
	case RUNTYPE_WINDOW:
		{
			_WindowMain();
		}
		break;
	}
}

/**
@brief 윈도우 모드 동작 메소드
*/
void CEnvironmentManager::_WindowMain()
{
	MSG msg;
	Constructer();

	while (isActive())
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message==WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Run();
		}
	}
	Destructer();
}

/**
@brief 콘솔 모드 동작 메소드
*/
void CEnvironmentManager::_ConsoleMain()
{
	Constructer();
	while(isActive())
	{
		Run();
	}	
	Destructer();
}

/**
@brief 서비스 모드 동작 메소드
@param argc 매개 변수 갯수
@param argv 매개 변수 값
*/
void CEnvironmentManager::ServiceMain(DWORD argc, LPTSTR* argv)
{
	m_hSrv=RegisterServiceCtrlHandler(GetProgramName(), ::ServiceHandler);
	if(m_hSrv==0)
	{
		LOG_ERROR(_T("RegisterServiceCtrlHandler 실패."));
		return;
	}

	// 서비스가 시작중임을 알린다.
	_SetStatus(SERVICE_START_PENDING);

	m_bPause = false;
	Constructer();
	while(isActive())
	{
		Run();
	}
	Destructer();
}

/**
@brief 서비스 시작 완료 메소드
*/
void CEnvironmentManager::Complete()
{
	if(m_eRunType == RUNTYPE_SERVICE)
		_SetStatus(SERVICE_RUNNING);
}

/**
@brief 서비스 상태 변경 메소드
@param dwState 바꿀 상태
@param dwAccept 서비스가 받을 수 있는 이벤트 상태 설정. 기본적으로 디폴트 파라미터 값을 사용하면 됨.
@return 상태 변경 성공 여부
*/
bool CEnvironmentManager::_SetStatus(DWORD dwState, DWORD dwAccept)
{
	SERVICE_STATUS ss;
	ss.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ss.dwCurrentState = dwState;
	ss.dwControlsAccepted=dwAccept;
	ss.dwWin32ExitCode=0;
	ss.dwServiceSpecificExitCode=0;
	ss.dwCheckPoint=0;
	ss.dwWaitHint=0;

	m_nNowState = dwState;
	if(m_hSrv)
	{
		if(!SetServiceStatus(m_hSrv, &ss))
		{
			LOG_ERROR(_T("SetServiceStatus Fail. %d"), dwState);
			return false;
		}
		else
		{
			LOG_INFO(_T("SetServiceStatus Success. %d"), dwState);
			return true;
		}
	}
	return false;
}

/**
@brief 서비스 핸들러. 서비스 상태 변경시 불려지는 메소드다.
@param fdwControl 바꿀 상태
*/
VOID WINAPI CEnvironmentManager::ServiceHandler(DWORD fdwControl)
{
	if(fdwControl == m_nNowState)
		return;

	switch(fdwControl)
	{
	case SERVICE_CONTROL_PAUSE:
		_SetStatus(SERVICE_PAUSE_PENDING);
		m_bPause = true;
		_SetStatus(SERVICE_PAUSED);
		break;
	case SERVICE_CONTROL_CONTINUE:
		_SetStatus(SERVICE_CONTINUE_PENDING);
		m_bPause = false;
		_SetStatus(SERVICE_RUNNING);
		break;
	case SERVICE_CONTROL_STOP:
		_SetStatus(SERVICE_STOP_PENDING);
		Stop();
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	default:
		_SetStatus(m_nNowState);
		break;
	}
}

} //namespace Redmoon
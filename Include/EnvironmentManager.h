#pragma once

#include "Common.h"
#include "Singleton.h"
#include "Str.h"
#include "ServiceInstaller.h"

namespace Redmoon
{

enum RUN_TYPE{
	RUNTYPE_NONE = -1,
	RUNTYPE_CONSOLE = 0,
	RUNTYPE_SERVICE = 1,
	RUNTYPE_WINDOW = 2,
};

class CEvent;

/**
@brief 프로그램을 다양한 모드로 쉽게 변환 할 수 있게 지원하는 클래스.
*/
class CEnvironmentManager : public CSingleton<CEnvironmentManager>
{
private:
	///콘솔 모드시 동작하는 함수
	void _ConsoleMain();
	
	///윈도우 모드시 동작하는 메소드
	void _WindowMain();

	///상태 변경
	bool _SetStatus(DWORD dwState, DWORD dwAccept = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE);

protected:
	///시작 완료 되었단 메소드
	void Complete();

public:
	///생성자
	CEnvironmentManager(const TCHAR* szProgramName);

	///소멸자
	virtual ~CEnvironmentManager();

	///해당 타입으로 초기화 작업을 한다. 그리고 구동 시도도 한다.
	void Init(RUN_TYPE eRunType);

	/**
	@brief 활성화 여부 검사 메소드
	@return 활성화 여부
	*/
	bool isActive(){return m_bActive;}

	/**
	@brief 서버 이름을 얻어온다
	@return 서버 이름
	*/
	const TCHAR* GetProgramName(){return m_strProgramName.c_str();}

	///서비스 시작 함수.
	void ServiceMain(DWORD argc, LPTSTR* argv);
	
	/**
	@brief 활성화 중지 상태로 만든다
	*/
	void Stop(){InterlockedExchange((LONG*)&m_bActive, (LONG)false);}

	///서비스 상태 변경시 불려지는 메소드
	VOID WINAPI ServiceHandler(DWORD fdwControl);

	///생성 작업
	virtual void Constructer() = 0;

	///반복적으로 불려지는 메소드
	virtual void Run() = 0;

	///소멸 작업
	virtual void Destructer() = 0;

protected:
	///Run메소드에서 해당 스레드를 멈췄다 풀기 위해 사용하도록 이벤트를 소유한다.
	CAutoPtr<CEvent> m_pcEvent;

private:
	///서버의 이름
	tstring m_strProgramName;

	///현재 상태
	DWORD m_nNowState;

	///서비스 상태 핸들
	SERVICE_STATUS_HANDLE m_hSrv;

	///구동 타입
	RUN_TYPE m_eRunType;

	///구동 중인지
	bool m_bActive;

	///멈춤 상태인지
	bool m_bPause;
};

inline CEnvironmentManager* GetEnvironmentManager()
{
	return CEnvironmentManager::InstancePtr();
}

} //namespace Redmoon


#define RUN_MAIN_CONSOLE(ClassName, szProgramName) new ClassName(szProgramName); Redmoon::GetEnvironmentManager()->Init(Redmoon::RUNTYPE_CONSOLE); delete Redmoon::GetEnvironmentManager();
#define RUN_MAIN_WINDOW(ClassName, szProgramName) new ClassName(szProgramName); Redmoon::GetEnvironmentManager()->Init(Redmoon::RUNTYPE_WINDOW); delete Redmoon::GetEnvironmentManager();
#define RUN_MAIN_SERVICE(ClassName, szProgramName) new ClassName(szProgramName); Redmoon::GetEnvironmentManager()->Init(Redmoon::RUNTYPE_SERVICE); delete Redmoon::GetEnvironmentManager();

///서비스 자동 시작으로 설치
#define SERVICE_INSTALL_AUTOSTART(szProgramName) Redmoon::CServiceInstaller::Install(szProgramName, SERVICE_SYSTEM_START | SERVICE_AUTO_START)

///서비스 수동 시작으로 설치
#define SERVICE_INSTALL_MANUALSTART(szProgramName) Redmoon::CServiceInstaller::Install(szProgramName, SERVICE_DEMAND_START)

///서비스 제거
#define SERVICE_UNINSTALL(szProgramName) Redmoon::CServiceInstaller::Uninstall(szProgramName)
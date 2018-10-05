#pragma once

#include "Common.h"

namespace Redmoon
{

/**
@brief 서비스 컨트롤러
*/
class CServiceInstaller
{
public:
	///서비스 설치 메소드
	static bool Install(const TCHAR* szProgramName, DWORD dwServiceStartType);

	///서비스 제거 메소드
	static bool Uninstall(const TCHAR* szProgramName);
};

} //namespace Redmoon
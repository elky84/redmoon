#pragma once

#include "Common.h"

namespace Redmoon
{

/**
@brief ���� ��Ʈ�ѷ�
*/
class CServiceInstaller
{
public:
	///���� ��ġ �޼ҵ�
	static bool Install(const TCHAR* szProgramName, DWORD dwServiceStartType);

	///���� ���� �޼ҵ�
	static bool Uninstall(const TCHAR* szProgramName);
};

} //namespace Redmoon
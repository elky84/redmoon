// EasyExec.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ


// CEasyExecApp:
// �� Ŭ������ ������ ���ؼ��� EasyExec.cpp�� �����Ͻʽÿ�.
//

class CEasyExecApp : public CWinApp
{
public:
	CEasyExecApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CEasyExecApp theApp;

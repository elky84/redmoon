#pragma once

#include "Thread.h"

class CTestThread : public Redmoon::CThread
{

public:
	CTestThread();
	~CTestThread();

	virtual void Act();
};
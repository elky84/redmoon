#pragma once

#include "Pipe.h"

namespace Redmoon
{

/**
@brief ������ Ŭ���̾�Ʈ ����� ������ Ŭ����. 
*/
class CPipeServer : public CPipe
{
public:
	///������.
	CPipeServer(const tstring& strName, CPeer* pcPeer);

	///Listen �õ� �޼ҵ�
	bool Listen();

	///Disconnect �õ� �޼ҵ�
	virtual void Disconnect();

	/// ���� ������ �̺�Ʈ �޼ҵ�
	virtual void OnDisconnect();

	///�ֱ����� ������ ������ �޼ҵ�
	virtual void Act();
};

} //namespace Redmoon
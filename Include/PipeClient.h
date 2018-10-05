#pragma once

#include "Pipe.h"

namespace Redmoon
{

/**
@brief ������ Ŭ���̾�Ʈ ����� ������ Ŭ����. 
*/
class CPipeClient : public CPipe
{
public:
	///������.
	CPipeClient(const tstring& strName, CPeer* pcPeer);

	///Connect �õ� �޼ҵ�
	bool Connect();

	///Disconnect �õ� �޼ҵ�
	virtual void Disconnect();

	///�ֱ����� ������ ������ �޼ҵ�
	virtual void Act();

	/// ���� ������ �̺�Ʈ �޼ҵ�
	virtual void OnDisconnect();
};

} //namespace Redmoon
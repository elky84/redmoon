#include "PipeClient.h"

namespace Redmoon
{

/**
@brief ������. ������ �̸��� �Ǿ ���� �޴´�.
@param strName �Ǿ� �̸�
@param pcPeer IO�� ����� �Ǿ�
*/
CPipeClient::CPipeClient(const tstring& strName, CPeer* pcPeer) : CPipe(strName, pcPeer)
{
}

/**
@brief ���� �õ� �޼ҵ�
@return ���� ����
*/
bool CPipeClient::Connect()
{
	if(isConnected())
		return false;

	if(isRefreshing())
		return false;

	HANDLE hPipe = CreateFile( GetName().c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, GetPipeContextAccept()); 
	if(hPipe == INVALID_HANDLE_VALUE)
	{
		LOG_INFO(_T("%s CreateFile Failed [%d]"), __TFUNCTION__, GetLastError());
		return false;
	}

	SetPipe(hPipe);
	switch (GetLastError()) 
	{ 
	case ERROR_IO_PENDING: // The overlapped connection in progress. 
		{
			LOG_INFO(_T("%s Try"), __TFUNCTION__);
			SetTryConnect(true);
			Register();
		}
		break; 
	case ERROR_SUCCESS: // Client is already connected, so signal an event. 
		{
			Register();
			SetEvent(GetPipeContextAccept()->GetEvent());
			LOG_INFO(_T("%s Connected"), __TFUNCTION__);
		}
		break;
	default: // If an error occurs during the connect operation... 
		{
			LOG_INFO(_T("%s failed. %d."), __TFUNCTION__, GetLastError());
			Disconnect();
			return false;
		}
	} 

	return true;
}

/**
@brief ���� ���� �õ� �޼ҵ�
*/
void CPipeClient::Disconnect()
{
	OnDisconnect();
}

/**
@brief �ֱ����� ���� �޼ҵ�. ���ӽ� �Ǿ��� Act�� ȣ���ϰ�, �����ӽ� ���� �õ��� �Ѵ�.
*/
void CPipeClient::Act()
{
	if(false == isConnected())
	{
		Connect();
	}
	else
	{
		GetPeer()->Act();
	}
}

/**
@brief ���� ������ �̺�Ʈ �޼ҵ�
*/
void CPipeClient::OnDisconnect()
{
	if(!DisconnectNamedPipe(GetPipe()))
	{
		LOG_INFO(_T("%s DisconnectNamedPipe failed [%d]"), __TFUNCTION__, GetLastError());
		return;
	}

	__super::OnDisconnect();
}

} //namespace Redmoon
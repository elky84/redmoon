#include "PipeClient.h"

namespace Redmoon
{

/**
@brief 생성자. 파이프 이름과 피어를 전달 받는다.
@param strName 피어 이름
@param pcPeer IO를 담당할 피어
*/
CPipeClient::CPipeClient(const tstring& strName, CPeer* pcPeer) : CPipe(strName, pcPeer)
{
}

/**
@brief 접속 시도 메소드
@return 성공 여부
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
@brief 접속 해제 시도 메소드
*/
void CPipeClient::Disconnect()
{
	OnDisconnect();
}

/**
@brief 주기적인 동작 메소드. 접속시 피어의 Act를 호출하고, 비접속시 접속 시도를 한다.
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
@brief 접속 해제시 이벤트 메소드
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
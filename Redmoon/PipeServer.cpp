#include "PipeServer.h"

namespace Redmoon
{

/**
@brief 생성자. 파이프 이름과 피어를 전달 받는다.
@param strName 피어 이름
@param pcPeer IO를 담당할 피어
*/
CPipeServer::CPipeServer(const tstring& strName, CPeer* pcPeer) : CPipe(strName, pcPeer)
{
}

/**
@brief Listen 시도 메소드
@return 성공 여부
*/
bool CPipeServer::Listen()
{
	if(isConnected())
		return false;

	if(isRefreshing())
		return false;

	HANDLE hPipe = CreateNamedPipe( GetName().c_str(), PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 
		PIPE_BUF_SIZE, PIPE_BUF_SIZE, NMPWAIT_USE_DEFAULT_WAIT, NULL);
	if(hPipe == INVALID_HANDLE_VALUE)
	{
		LOG_INFO(_T("%s CreateNamedPipe Failed [%d]"), __TFUNCTION__, GetLastError());
		return false;
	}

	SetPipe(hPipe);
	if(ConnectNamedPipe(GetPipe(), GetPipeContextAccept()))
	{
		LOG_INFO(_T("%s ConnectNamedPipe Failed [%d]"), __TFUNCTION__, GetLastError());
		CloseHandle(GetPipe());
		return false;

	}
	switch (GetLastError()) 
	{ 
	case ERROR_IO_PENDING: // The overlapped connection in progress. 
		{
			LOG_INFO(_T("%s ConnectNamedPipe Try"), __TFUNCTION__);
			SetTryConnect(true);
			Register();
		}
		break; 
	case ERROR_PIPE_CONNECTED: // Client is already connected, so signal an event. 
		{
			Register();
			SetEvent(GetPipeContextAccept()->GetEvent());

			LOG_INFO(_T("%s ConnectNamedPipe Connected"), __TFUNCTION__);
			OnConnect();
		}
		break;
	default: // If an error occurs during the connect operation... 
		{
			LOG_INFO(_T("ConnectNamedPipe failed. %d."), GetLastError());
			CloseHandle(GetPipe());
			return false;
		}
	} 
	return true;
}

/**
@brief 접속 해제 시도 메소드
*/
void CPipeServer::Disconnect()
{
	OnDisconnect();	
}

/**
@brief 주기적인 동작 메소드. 
*/
void CPipeServer::Act()
{
}

/**
@brief 접속 해제시 이벤트 메소드
*/
void CPipeServer::OnDisconnect()
{
	if(!DisconnectNamedPipe(GetPipe()))
	{
		LOG_INFO(_T("%s DisconnectNamedPipe failed [%d]"), __TFUNCTION__, GetLastError());
		return;
	}

	__super::OnDisconnect();
	Listen();
}


} //namespace Redmoon

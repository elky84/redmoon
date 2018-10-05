#pragma once

#include "Common.h"
#include "Compatible.h"
#include <winsock2.h>
#include "Str.h"

#pragma comment(lib, "ws2_32.lib")

namespace Redmoon
{

///기본 소켓 윈도우 메시지 번호
const int WM_SOCKET = WM_USER + 5000;

///Client에 사용되는 최대 버퍼 크기
const int MAX_CLIENT_PACKET_BUFFER_SIZE = 100000;

///SESSION의 RecvBuffer에 사용되는 최대 버퍼 크기
const int MAX_SESSION_RECV_BUFFER_SIZE = 10000;

///SESSION의 SendBuffer에 사용되는 최대 버퍼 크기
const int MAX_SESSION_SEND_BUFFER_SIZE = 1000000;

///리스너 기본 큐 크기
const int DEFAULT_WAITING_QUEUE = 5;

//최대 한 패킷 크기
const int MAX_ONE_PACKET_SIZE = 4096;

///기본 최대 유저
const int DEFAULT_MAX_USER_COUNT = 50;

///억셉트 주소 크기
const int ACCEPT_ADDRESS_LENGTH = sizeof(SOCKADDR_IN) + 16;

///억셉트에 사용되는 버퍼 크기
const int ACCEPT_BUFFER_SIZE = ACCEPT_ADDRESS_LENGTH * 2;

///IP문자열 최대 길이
const int MAX_IP = 15 + 1;

enum SOCKET_TYPE{
	SOCKETTYPE_NONE = -1,
	SOCKETTYPE_TCP = 0, //TCP 소켓
	SOCKETTYPE_UDP = 1, //UDP 소켓
};

enum PACKET_EVENT
{
	PACKETEVENT_NONE = -1,
	PACKETEVENT_CONNECT,
	PACKETEVENT_DISCONNECT,
	PACKETEVENT_PACKET,
};

/**
@brief 현재 컴퓨터의IP를 얻어오는 메소드
@return 현재 컴퓨터의 IP
*/
inline std::string GetIP(){
	char ip[MAX_IP] = "";
	char name[128] ;
	PHOSTENT hostinfo ;
	if(gethostname(name, sizeof(name)) == 0)
	{
		if((hostinfo = gethostbyname(name)) != NULL)
		{
			memcpy(ip, inet_ntoa(*(struct in_addr* )*hostinfo->h_addr_list), MAX_IP);
			ip[MAX_IP-1] = 0;
			return ip;
		}
	}
	return "";
}

/**
@brief 특정 IP와 포트를 주소 구조체로 구성해주는 메소드
@param szIP 구성할 IP
@param nPort 구성할 포트
@return 구성된 주소 구조체
*/
inline SOCKADDR_IN GetSockAddr(const char* szIP, int nPort)
{
	char* szIPAddress;
	HOSTENT* pHostEnt;
	unsigned long ulAddr;
	SOCKADDR_IN sockaddrin;
	sockaddrin.sin_family = AF_INET;
	ulAddr = inet_addr( szIP );
	if ( ulAddr == INADDR_NONE ) 
	{
		pHostEnt = gethostbyname (szIP);
		if(pHostEnt == NULL) 
		{
			return sockaddrin;
		}
		szIPAddress = inet_ntoa(*(LPIN_ADDR)*(pHostEnt->h_addr_list));
		ulAddr = inet_addr(szIPAddress);
	}

	sockaddrin.sin_addr.s_addr = ulAddr;
	sockaddrin.sin_port = htons (static_cast<u_short> (nPort));
	memset(sockaddrin.sin_zero, 0, sizeof(sockaddrin.sin_zero));
	return sockaddrin;
}

/**
@param DWORD형 IP를 TCHAR형으로 변환
@param dwIP 구성할 포트
@param szIP 변환할 IP 문자열
*/
inline void IPToTCHAR(DWORD dwIP, TCHAR*  szIP)
{
	unsigned int temp ;
	int i, j = 0 ;
	TCHAR v1, v2 ;
	TCHAR*  p = szIP ;
	for(i = 0 ; i < 4 ; i++ )
	{
		temp = (dwIP >> (i*8) ) & 0x000000ff ;

		v1 = (temp / 100) % 10 ;
		if(v1 )
		{
			p[j] = v1 +_T('0') ;
			++j ;
		}
		temp = temp % 100 ;

		v2 = temp / 10 ;
		if(v1 || v2 )
		{
			p[j] = v2 +_T('0') ;
			++j ;
		}

		p[j] = temp % 10 +_T('0') ;
		++j ;

		p[j] =_T('.') ;
		++j ;
	}

	i = j-1 ;
	while(i < 16 )
	{
		p[i] = 0 ;
		++i ;
	}
}

/**
@brief 객체 값 교환 템플릿 함수
@param objA 값 교환할 객체 A 
@param objB 값 교환할 객체 B
*/
template <typename T>
void Swap(T& objA, T& objB)
{
	T objC = objA;
	objA = objB;
	objB = objC;
}

#pragma pack(1)

typedef unsigned short PACKET_SIZE;
typedef unsigned short PACKET_NUMBER;

/**
@brief 기본 패킷 헤더 구조체
*/
struct SHeader
{
private:
	///길이가 PACKET_NUMBER를 넘어서지 않는다고 가정한다.
	PACKET_SIZE m_nSize;
public:
	/**
	@brief 생성자.
	@param nSize 패킷 길이.
	*/
	SHeader(PACKET_SIZE nSize) : m_nSize(nSize)
	{
	}

	/**
	@brief 패킷 길이 얻어오는 메소드
	@return 패킷 길이
	*/
	inline PACKET_SIZE GetLen(){return m_nSize;}
};

/**
@brief 서버 정보 구조체
*/
struct SServerInformation
{
	///서버 IP
	char m_szIP[MAX_IP];

	///서버 포트
	int m_nPort;
	
	/**
	@brief 빈 생성자
	*/
	SServerInformation()
	{
		memset(this, 0, sizeof(*this));
	}

	/**
	@brief 생성자.
	@param szIP 아이피
	@param nPort 포트
	*/
	SServerInformation(const char* szIP, int nPort) : m_nPort(nPort)
	{
		STRNCPY(m_szIP, szIP, MAX_IP);
	}
};

#pragma pack()

} //namespace Redmoon
#pragma once

#include "Str.h"

#include <winsock2.h>
#include <assert.h>
#include <vector>

#pragma warning(disable:4267)
#pragma warning(disable:4244)

namespace Redmoon
{

const int DEFAULT_PROTOCOL = 0;
const int NO_FLAGS = 0;

const int MAX_HEADER_SIZE = 16385;

class CSmtp  
{
public:        
	CSmtp();    
	virtual ~CSmtp();
	bool    AddRecipient(const TCHAR* email, const TCHAR* name= _T(""));
	bool    AddBCCRecipient(const TCHAR* email, const TCHAR* name=_T(""));
	bool    AddCCRecipient(const TCHAR* email, const TCHAR* name=_T(""));    
	bool    ConnectServer(const TCHAR* server, const unsigned short port=NULL);
	bool    Disconnect();
	bool    GetConnectStatus();    
	const unsigned int    GetBCCRecipientCount();    
	const unsigned int    GetCCRecipientCount();
	const unsigned int    GetRecipientCount();    
	const unsigned int    GetSocket();
	const char*    const    GetLocalHostIp();
	const char*    const    GetLocalHostname();    
	const TCHAR*    const    GetMessageBody();    
	const TCHAR*    const    GetReplyTo();
	const TCHAR*    const    GetSenderEmail();
	const TCHAR*    const    GetSenderName();
	const TCHAR*    const    GetSubject();    
	const TCHAR*    const    GetXMailer();    
	bool    Send();
	void    SetMessageBody(const TCHAR* body);    
	void    SetSubject(const TCHAR* subject);    
	void    SetSenderName(const TCHAR* name);    
	void    SetSenderEmail(const TCHAR* email);    
	void    SetReplyTo(const TCHAR* replyto);    
	void    SetXMailer(const TCHAR* xmailer);
	void	SetAuthInfo(const TCHAR *username, const TCHAR *passwd);
	int		SendNRecv(const TCHAR *buf, int bufsize, TCHAR *recvbuf, int recvbufsize);

	void Debug(const TCHAR *str, ...);

private:
	class CRecipient
	{
	public:
		CRecipient() 
		{ 
			m_szEmail = NULL;
		};
		CRecipient& operator=(const CRecipient& src)
		{
			if (&src != this)
			{
				if (m_szEmail)
					delete  m_szEmail;
				size_t s = _tcslen(src.m_szEmail);
				m_szEmail = new TCHAR[s+1];
				TCSCPY(m_szEmail, s+1, src.m_szEmail);
			}
			return (*this);
		};
		virtual ~CRecipient()
		{
			if (m_szEmail)
				delete  m_szEmail;
		};
		TCHAR* GetEmail()
		{
			return m_szEmail;
		};
		void SetEmail(const TCHAR* email)
		{
			assert(email);
			size_t s=_tcslen(email);
			if (s > 0)
			{
				m_szEmail = new TCHAR[s+1];
				TCSCPY(m_szEmail, s+1, email);
			}        
		};
	private:
		TCHAR *m_szEmail;
	};

	TCHAR*  _formatHeader();    
	bool    _formatMessage();
	SOCKET  _connectServerSocket(const TCHAR* szServer, const unsigned short port=NULL);    


private:

	bool bCon;
	char m_szHostName[MAX_PATH];
	char* m_szIPAddr;

	TCHAR* m_szFromEmail;
	TCHAR* m_szFromName;
	TCHAR* m_szSubject;
	TCHAR* m_szMsgBody;
	TCHAR* m_szXMailer;
	TCHAR* m_szReplyTo;
	TCHAR* m_szUserName;
	TCHAR* m_szPassWord;
	BOOL m_bUseAuth;

	WSADATA wsaData;
	SOCKET hSocket;

	std::vector<CRecipient*> Recipients;
	std::vector<CRecipient*> CCRecipients;
	std::vector<CRecipient*> BCCRecipients;    

	TCHAR m_szMsgHeader[MAX_HEADER_SIZE];
};

} //namespace Redmoon
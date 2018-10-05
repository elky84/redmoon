#include <stdio.h>
#include "Smtp.h"
#include "Base64.h"
#include "LogManager.h"

namespace Redmoon
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSmtp::CSmtp()
{
	bCon            = false;

	m_szFromEmail    = NULL;
	m_szFromName    = NULL;
	m_szSubject        = NULL;
	m_szMsgBody        = NULL;
	m_szXMailer        = NULL;
	m_szReplyTo        = NULL;
	m_szIPAddr        = NULL;
	m_bUseAuth = FALSE;
	m_szUserName = NULL;
	m_szPassWord = NULL;
}

CSmtp::~CSmtp()
{
	// Free recipient lists
	for (unsigned int n = 0; n < Recipients.size(); n++)
		delete Recipients[n];
	for (unsigned int n = 0; n < CCRecipients.size(); n++)
		delete CCRecipients[n];
	for (unsigned int n = 0; n < BCCRecipients.size(); n++)
		delete BCCRecipients[n];

	// Free variables
	if (m_szFromEmail)
		delete m_szFromEmail;
	if (m_szFromName)
		delete m_szFromName;
	if (m_szSubject)
		delete m_szSubject;
	if (m_szMsgBody)
		delete m_szMsgBody;
	if (m_szXMailer)
		delete m_szXMailer;
	if (m_szReplyTo)
		delete m_szReplyTo;

	if(m_bUseAuth)
	{
		free(m_szUserName);
		free(m_szPassWord);
	}

	if (bCon)
		Disconnect();
}

bool CSmtp::AddRecipient(const TCHAR* email, const TCHAR* name)
{
	assert(email);

	int s=_tcslen(email);
	if (s==0)
		return false;

	CRecipient *pRec = new CRecipient();

	size_t nNewLength = s+_tcslen(name)+4;

	TCHAR *pcBuf = new TCHAR[nNewLength];
	SPRINTF(pcBuf, _T("%s<%s>"), name,email);    
	pRec->SetEmail(pcBuf);
	Recipients.insert(Recipients.end(), pRec);

	return (true);    
}

bool CSmtp::AddCCRecipient(const TCHAR* email, const TCHAR* name)
{
	assert(email);

	int s=_tcslen(email);
	if (s==0)
		return false;

	CRecipient *pRec = new CRecipient();    

	size_t nNewLength = s+_tcslen(name)+4;

	TCHAR *pcBuf = new TCHAR[nNewLength];
	SPRINTF(pcBuf, _T("%s<%s>"),name,email);    
	pRec->SetEmail(pcBuf);
	CCRecipients.insert(CCRecipients.end(), pRec);
	delete pcBuf;

	return (true);
}

bool CSmtp::AddBCCRecipient(const TCHAR* email, const TCHAR* name)
{
	assert(email);

	int s=_tcslen(email);
	if (s==0)
		return false;

	CRecipient *pRec = new CRecipient();

	size_t nNewLength = s+_tcslen(name)+4;

	TCHAR *pcBuf = new TCHAR[nNewLength];
	SPRINTF(pcBuf, _T("%s<%s>"),name,email);    
	pRec->SetEmail(pcBuf);
	BCCRecipients.insert(BCCRecipients.end(), pRec);
	delete pcBuf;    

	return (true);
}

bool CSmtp::Disconnect()
{
	if (!bCon) 
	{
		return (false);
	}

	TCHAR        szReceiveBuffer[4096] = {0,};
	int            iLength = 0;
	int            iEnd = 0;

	TCHAR* szQuit = _T("Quit\r\n");

	int iRet = SendNRecv(szQuit, _tcslen(szQuit), szReceiveBuffer, sizeof(szReceiveBuffer));
	if(iRet == -1){
		Debug(_T("SendNRecv Er %d %s %s"), WSAGetLastError(), szQuit, szReceiveBuffer);
		return false;
	}
	iEnd += iLength;

	bCon=false;    

	hSocket=NULL;

	return (true);
}

int CSmtp::SendNRecv(const TCHAR *buf, int bufsize, TCHAR *recvbuf, int recvbufsize)
{
	int iLength = 0;//recv(hSocket, (LPSTR)szTrash, 1024, NO_FLAGS);

	if (send(hSocket, (LPSTR)buf, bufsize, NO_FLAGS) == SOCKET_ERROR)
		return -1;

	iLength = recv(hSocket, (LPSTR)recvbuf, recvbufsize, NO_FLAGS);
	if(0<=iLength && iLength < recvbufsize)
		recvbuf[iLength] = 0;

	return iLength;
}

bool CSmtp::Send()
{
	// verify sender email
	if (m_szFromEmail == NULL) {
		return (false);
	}

	TCHAR        szReceiveBuffer[4096]={0,};
	int            iLength = 0;
	int            iEnd = 0;
	TCHAR        buf[4096]= _T("");
	int iRet =0;
	// get proper header
	_formatHeader();

	if (m_szMsgHeader == NULL) 
	{
		return (false);
	}
	// auth
	if(m_bUseAuth)
	{
		TCSCPY(buf, _countof(buf), _T("AUTH LOGIN\r\n"));

		iRet = SendNRecv(buf, _tcslen(buf), szReceiveBuffer,sizeof(szReceiveBuffer));
		if(iRet == -1){
			//printf("Socket recv error: %d\r\n", WSAGetLastError());    
			Debug(_T("SendNRecv Er %d %s %s"), WSAGetLastError(), buf, szReceiveBuffer);
			return false;
		}
		iEnd += iRet;

		TCHAR szEncodeOuput[1024];
		CBase64 B64;
		B64.EncodeBuffer((CHAR*)m_szUserName, sizeof(m_szUserName), (CHAR*)szEncodeOuput);

		TCSCAT(szEncodeOuput, _countof(szEncodeOuput), _T("\r\n"));
		iRet = SendNRecv(szEncodeOuput, _tcslen(szEncodeOuput), szReceiveBuffer,sizeof(szReceiveBuffer));
		if(iRet == -1){
			Debug(_T("SendNRecv Er %d %s %s"), WSAGetLastError(), buf, szReceiveBuffer);
			return false;
		}
		iEnd += iRet;

		B64.EncodeBuffer((CHAR*)m_szPassWord, sizeof(m_szPassWord), (CHAR*)szEncodeOuput);
		TCSCAT(szEncodeOuput, _countof(szEncodeOuput), _T("\r\n"));
		iRet = SendNRecv(szEncodeOuput, sizeof(szEncodeOuput), szReceiveBuffer,sizeof(szReceiveBuffer));
		if(iRet == -1){
			Debug(_T("SendNRecv Er %d %s %s"), WSAGetLastError(), buf, szReceiveBuffer);
			return false;
		}
		iEnd += iRet;
	}
	// start
	TCSCPY(buf, _countof(buf), _T("MAIL FROM: <"));
	TCSCAT(buf, _countof(buf), m_szFromEmail);
	TCSCAT(buf, _countof(buf), _T(">\r\n"));
	iRet = SendNRecv(buf, _tcslen(buf), szReceiveBuffer,sizeof(szReceiveBuffer));
	if(iRet == -1){
		Debug(_T("SendNRecv Er %d %s %s"), WSAGetLastError(), buf, szReceiveBuffer);
		return false;
	}
	iEnd += iLength;
	// create message receipts
	
	TCHAR *token;
	for(unsigned int i=0;i<Recipients.size();i++) 
	{
		token = TCSTOK(Recipients.at(i)->GetEmail(), _T("<"), &nexttoken);
		token = TCSTOK(NULL, _T("<"), &nexttoken);    
		if (token == NULL) 
			token = TCSTOK(Recipients.at(i)->GetEmail(), _T("<"), &nexttoken);
		TCSCPY(buf, _countof(buf), _T("RCPT TO: <"));
		TCSCAT(buf, _countof(buf), token);
		TCSCAT(buf, _countof(buf), _T("\r\n"));
		iRet = SendNRecv(buf, sizeof(buf), szReceiveBuffer,sizeof(szReceiveBuffer));
		if(iRet == -1){
			Debug(_T("SendNRecv Er %d %s %s"), WSAGetLastError(), buf, szReceiveBuffer);
			//printf("Socket recv error: %d\r\n", WSAGetLastError());    
			return false;
		}
		iEnd += iLength;
	}
	
	for(unsigned int i=0;i<CCRecipients.size();i++) 
	{
		token = TCSTOK(CCRecipients.at(i)->GetEmail(), _T("<"), &nexttoken);
		token = TCSTOK(NULL, _T("<"), &nexttoken);
		if (token == NULL) 
			token = TCSTOK(CCRecipients.at(i)->GetEmail(), _T("<"), &nexttoken);
		TCSCPY(buf, _countof(buf), _T("RCPT TO: <"));
		TCSCAT(buf, _countof(buf), token);
		TCSCAT(buf, _countof(buf), _T("\r\n"));
		iRet = SendNRecv(buf, _tcslen(buf), szReceiveBuffer,sizeof(szReceiveBuffer));
		if(iRet == -1)
		{
			Debug(_T("SendNRecv Er %d %s %s"), WSAGetLastError(), buf, szReceiveBuffer);
			return false;
		}
		iEnd += iLength;
	}

	for(unsigned int i=0;i<BCCRecipients.size();i++)
	{
		token = TCSTOK(BCCRecipients.at(i)->GetEmail(), _T("<"), &nexttoken);
		token = TCSTOK(NULL, _T("<"), &nexttoken);
		if (token == NULL) 
			token = TCSTOK(BCCRecipients.at(i)->GetEmail(), _T("<"), &nexttoken);
		TCSCPY(buf, _countof(buf), _T("RCPT TO: <"));
		TCSCAT(buf, _countof(buf), token);
		TCSCAT(buf, _countof(buf), _T("\r\n"));
		iRet = SendNRecv(buf, _tcslen(buf), szReceiveBuffer,sizeof(szReceiveBuffer));
		if(iRet == -1)
		{
			Debug(_T("SendNRecv Er %d %s %s"), WSAGetLastError(), buf, szReceiveBuffer);
			//printf("Socket recv error: %d\r\n", WSAGetLastError());    
			return false;
		}
		iEnd += iLength;
	}

	// init data
	TCHAR* szData = _T("DATA\r\n");
	iRet = SendNRecv(szData, sizeof(szData), szReceiveBuffer,sizeof(szReceiveBuffer));
	if(iRet == -1)
	{
		Debug(_T("SendNRecv Er %d %s %s"), WSAGetLastError(), buf, szReceiveBuffer);
		return false;
	}
	iEnd += iLength;

	// send header   
	if (send(hSocket, (char*)m_szMsgHeader, sizeof(m_szMsgHeader), NO_FLAGS) == SOCKET_ERROR) 
		return (false);
	
	// send body    
	if (send(hSocket, (char*)m_szMsgBody, sizeof(m_szMsgBody), NO_FLAGS) == SOCKET_ERROR) 
		return (false);

	// signal end
	TCHAR* szSignalEnd = _T("\r\n.\r\n");
	iRet = SendNRecv(szSignalEnd, sizeof(szSignalEnd), szReceiveBuffer,sizeof(szReceiveBuffer));
	if(iRet == -1)
	{
		Debug(_T("SendNRecv Er %d %s %s"), WSAGetLastError(), buf, szReceiveBuffer);
		return false;
	}
	iEnd += iLength;
	return (true);
}

bool CSmtp::ConnectServer(const TCHAR* server, const unsigned short port)
{
	assert(server);

	if (bCon)
		Disconnect();
	bCon=false;
	hSocket = INVALID_SOCKET;

	hSocket = _connectServerSocket(server, port);    
	if (hSocket != INVALID_SOCKET) 
	{        
		TCHAR        szReceiveBuffer[4096] = _T("");
		TCHAR        buf[4096]= _T("");

		TCSCPY(buf, _countof(buf), _T("EHLO "));
		TCSCAT(buf, _countof(buf), server);
		TCSCAT(buf, _countof(buf), _T("\r\n"));

		int iRet = SendNRecv(buf, sizeof(buf), szReceiveBuffer,sizeof(szReceiveBuffer));
		if(iRet == -1){
			Debug(_T("SendNRecv Er %d %s %s"), WSAGetLastError(), buf, szReceiveBuffer);
			return false;
		}
		TCHAR szTrash[1024];
		int iTemp = 0;
		iTemp = recv(hSocket, (LPSTR)szTrash, 1024, NO_FLAGS);
	}
	else 
	{
		return (false);
	}

	bCon=true;
	return (true);
}

SOCKET CSmtp::_connectServerSocket(const TCHAR* szServer,
									   const unsigned short port)
{
	int                nConnect;
	short            nProtocolPort;
	LPHOSTENT        lpHostEnt;
	LPSERVENT        lpServEnt;
	SOCKADDR_IN        sockAddr;        

	SOCKET            hServerSocket = INVALID_SOCKET;

	lpHostEnt = gethostbyname(TCHAR_TO_CHAR(szServer));
	if (lpHostEnt) {        
		hServerSocket = socket(AF_INET, SOCK_STREAM,DEFAULT_PROTOCOL);
		if (hServerSocket != INVALID_SOCKET) {
			if (port != NULL) {
				nProtocolPort = port;
			}else{
				lpServEnt = getservbyname("mail", DEFAULT_PROTOCOL);
				if (lpServEnt == NULL) 
					nProtocolPort = htons(IPPORT_SMTP); 
				else 
					nProtocolPort = lpServEnt->s_port;
			}
			sockAddr.sin_family = AF_INET;
			sockAddr.sin_port = nProtocolPort;
			sockAddr.sin_addr = *((LPIN_ADDR)*lpHostEnt->h_addr_list);
			nConnect = connect(hServerSocket, (PSOCKADDR)&sockAddr, 
				sizeof(sockAddr));
			if(nConnect) 
				hServerSocket = INVALID_SOCKET;
		} else {
			//printf("Invalid socket\r\n");
			throw;
		}
	}
	return(hServerSocket);
}

TCHAR* CSmtp::_formatHeader()
{
	// check for at least one recipient
	if(Recipients.size() <= 0 )    {
		//printf("Please add a message recipient!\r\n");
		return NULL;
	}
	int cbto=0;
	int cbcc=0;
	int cbbcc=0;
	TCHAR *msgHeader = m_szMsgHeader;
	//TCHAR to[1024];
	for (unsigned int i=cbto=0;i<Recipients.size();i++) {        
		cbto+=_tcslen(Recipients.at(i)->GetEmail())+1;
	} if (cbto==0) cbto=1; TCHAR *to = new TCHAR[cbto];
	//TCHAR cc[1024];
	for (unsigned int i=cbcc=0;i<CCRecipients.size();i++) {        
		cbcc+=_tcslen(CCRecipients.at(i)->GetEmail())+1;
	} if (cbcc==0) cbcc=1; TCHAR *cc = new TCHAR[cbcc];
	//TCHAR bcc[1024];
	for (unsigned int i=cbbcc=0;i<BCCRecipients.size();i++) {        
		cbbcc+=_tcslen(BCCRecipients.at(i)->GetEmail())+1;
	} if (cbbcc==0) cbbcc=1; TCHAR *bcc = new TCHAR[cbbcc];

	TCHAR szDate[500];
	TCHAR sztTime[500];

	// create the recipient string, cc string, and bcc string
	to[0] = '\0';        
	for (unsigned int i=0;i<Recipients.size();i++) {        
		i > 0 ? TCSCAT(to, cbto, _T(",")):TCSCAT(to, cbto, _T(""));
		TCSCAT(to, cbto, Recipients.at(i)->GetEmail());
	}

	cc[0] = '\0';    
	for (unsigned int i=0;i<CCRecipients.size();i++) {
		i > 0 ? TCSCAT(cc,cbcc, _T(",")):TCSCAT(cc,cbcc, _T(""));
		TCSCAT(cc, cbcc, CCRecipients.at(i)->GetEmail());
	}

	bcc[0] = '\0';    
	for (unsigned int i=0;i<BCCRecipients.size();i++) {
		i > 0 ? TCSCAT(bcc,cbbcc, _T(",")):TCSCAT(bcc,cbbcc, _T(""));
		TCSCAT(bcc, cbbcc, BCCRecipients.at(i)->GetEmail());
	}
	// get the current date and time
	SYSTEMTIME st={0};
	::GetSystemTime(&st);

	::GetDateFormat(LOCALE_SYSTEM_DEFAULT,0,&st, _T("MMM dd yyyy"),szDate,sizeof(szDate));
	::GetTimeFormat(LOCALE_SYSTEM_DEFAULT,TIME_FORCE24HOURFORMAT,&st, _T("HH':'mm':'ss"),sztTime,sizeof(sztTime));
	// here it is...the main data of the message
	wsprintf(msgHeader, _T("DATE: %s %s\r\n"), szDate, sztTime);    
	if (m_szFromName != NULL) {
		TCSCAT(msgHeader, _countof(msgHeader), _T("FROM: "));
		TCSCAT(msgHeader, _countof(msgHeader), m_szFromName);
		TCSCAT(msgHeader, _countof(msgHeader), _T("\r\n"));
	}
	TCSCAT(msgHeader, _countof(msgHeader),_T("To: "));
	TCSCAT(msgHeader, _countof(msgHeader), to);
	TCSCAT(msgHeader, _countof(msgHeader), _T("\r\n"));
	TCSCAT(msgHeader, _countof(msgHeader),_T("Cc: "));
	TCSCAT(msgHeader, _countof(msgHeader), cc);
	TCSCAT(msgHeader, _countof(msgHeader), _T("\r\n"));
	if (m_szSubject != NULL) {
		TCSCAT(msgHeader, _countof(msgHeader), _T("Subject: "));
		TCSCAT(msgHeader, _countof(msgHeader), m_szSubject);
		TCSCAT(msgHeader, _countof(msgHeader), _T("\r\n"));
	}
	if (m_szXMailer != NULL) {
		TCSCAT(msgHeader, _countof(msgHeader),_T("X-Mailer: "));
		TCSCAT(msgHeader, _countof(msgHeader), m_szXMailer);
		TCSCAT(msgHeader, _countof(msgHeader), _T("\r\n"));
	}
	// start optional fields
	if (m_szReplyTo != NULL) {
		TCSCAT(msgHeader, _countof(msgHeader), _T("Reply-To: "));
		TCSCAT(msgHeader, _countof(msgHeader), m_szReplyTo);
		TCSCAT(msgHeader, _countof(msgHeader), _T("\r\n"));
	}
	// start MIME versions
	TCSCAT(msgHeader, _countof(msgHeader), _T("MIME-Version: 1.0\r\nContent-type: text/plain; TCHARset=US-ASCII\r\n"));

	// send header finish command
	TCSCAT(msgHeader, _countof(msgHeader), _T("\r\n"));    
	// clean up
	delete to;
	delete cc;
	delete bcc;
	// done    
	return msgHeader;    
}

const char* const CSmtp::GetLocalHostIp() 
{
	in_addr    *iaHost;
	if (gethostname(m_szHostName,255) != SOCKET_ERROR)
	{
		HOSTENT *pHe = NULL;        
		pHe = gethostbyname(m_szHostName);
		if (pHe != NULL) 
		{
			for (int i=0;pHe->h_addr_list[i] != 0;i++) 
			{
				iaHost = (LPIN_ADDR)pHe->h_addr_list[i];
				m_szIPAddr = inet_ntoa(*iaHost);
			}
		}            
	}
	return m_szIPAddr;
}

const char* const CSmtp::GetLocalHostname() 
{    
	if (gethostname((char FAR*)m_szHostName,255) == SOCKET_ERROR)    
	{
		Debug(_T("Failed to get the local hostname"));
	}
	return m_szHostName;
}
bool CSmtp::GetConnectStatus()
{
	return (bCon);
}

unsigned const int CSmtp::GetBCCRecipientCount()
{
	return (BCCRecipients.size());
}

unsigned const int CSmtp::GetCCRecipientCount() 
{
	return (CCRecipients.size());
}

unsigned const int CSmtp::GetSocket()
{
	return (hSocket);
}

const TCHAR* const CSmtp::GetMessageBody() 
{
	return (m_szMsgBody);
}

unsigned const int CSmtp::GetRecipientCount()
{
	return (Recipients.size());
}

const TCHAR* const CSmtp::GetReplyTo()  
{
	return (m_szReplyTo);
}

const TCHAR* const CSmtp::GetSenderEmail() 
{
	return (m_szFromEmail);
}

const TCHAR* const CSmtp::GetSenderName() 
{
	return (m_szFromName);
}

const TCHAR* const CSmtp::GetSubject() 
{
	return (m_szSubject);
}

const TCHAR* const CSmtp::GetXMailer() 
{
	return (m_szXMailer);
}

void CSmtp::SetMessageBody(const TCHAR* body)
{
	assert(body);
	int s=_tcslen(body);
	if (m_szMsgBody)
		delete  m_szMsgBody;
	m_szMsgBody = new TCHAR[s+1];
	TCSCPY(m_szMsgBody, s+1, body);    
}

void CSmtp::SetReplyTo(const TCHAR* replyto)
{
	assert(replyto);
	int s=_tcslen(replyto);
	if (m_szReplyTo)
		delete  m_szReplyTo;
	m_szReplyTo = new TCHAR[s+1];
	TCSCPY(m_szReplyTo, s+1, replyto);
}

void CSmtp::SetSenderEmail(const TCHAR* email)
{
	assert(email);
	int s=_tcslen(email);
	if (m_szFromEmail)
		delete  m_szFromEmail;
	m_szFromEmail = new TCHAR[s+1];
	TCSCPY(m_szFromEmail, s+1, email);        
}

void CSmtp::SetSenderName(const TCHAR* name)
{
	assert(name);
	int s=_tcslen(name);
	if (m_szFromName)
		delete  m_szFromName;
	m_szFromName = new TCHAR[s+1];
	TCSCPY(m_szFromName, s+1, name);
}

void CSmtp::SetSubject(const TCHAR* subject)
{
	assert(subject);
	int s=_tcslen(subject);
	if (m_szSubject)
		delete  m_szSubject;
	m_szSubject = new TCHAR[s+1];
	TCSCPY(m_szSubject, s+1, subject);
}

void CSmtp::SetXMailer(const TCHAR* xmailer)
{
	assert(xmailer);
	int s=_tcslen(xmailer);
	if (m_szXMailer)
		delete  m_szXMailer;
	m_szXMailer = new TCHAR[s+1];
	TCSCPY(m_szXMailer, s+1, xmailer);
}

void CSmtp::SetAuthInfo(const TCHAR *username, const TCHAR *passwd)
{
	m_bUseAuth = TRUE;

	m_szUserName = _tcsdup(username);
	m_szPassWord = _tcsdup(passwd);
}

void CSmtp::Debug(const TCHAR *str, ...)
{
	TCHAR buf[1000];
	va_list vl;
	va_start(vl, str);
	VSTPRINTF(buf, _countof(buf), str, vl);
	va_end(vl);
	buf[1000-1] = 0;

	LOG_DEBUG(_T("%s [%s]"), __TFUNCTION__, buf);
}

} //namespace Redmoon
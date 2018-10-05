#pragma hdrstop

#include "HTTPClient.h"
#include <winreg.h>
#include <stdexcept>

#include "Str.h"

#pragma  comment(lib,"wininet.lib")

namespace Redmoon
{
#define __HTTP_VERB_GET				_T("GET")
#define __HTTP_VERB_POST			_T("POST")
#define __HTTP_ACCEPTYPE			_T("*/*")
#define __HTTP_ACCEPT				_T("Accept: */*\r\n")

#ifdef _DEBUG
	#define TRACEERROR(x) PrintLastError(x)
#else	//_DEBUG
	#define TRACEERROR(x)
#endif	//_DEBUG


void	PrintLastError( DWORD lastError )
{
	LPVOID     lpMsgBuffer;
	DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		lastError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPTSTR>(&lpMsgBuffer),
		0,
		NULL);
	LOG_ERROR(_T("ERROR> %d : %s\n"), lastError, reinterpret_cast<LPTSTR>(lpMsgBuffer));

	LocalFree(lpMsgBuffer);
}


CHTTPClient::CHTTPClient()
{
	m_hHTTPOpen = m_hHTTPConnection = m_hHTTPRequest = NULL;
	m_dwFlagSSL = 0;
}

CHTTPClient::~CHTTPClient()
{
	m_hHTTPOpen = m_hHTTPConnection = m_hHTTPRequest = NULL;
}

CHTTPClient::enumRequestMethod CHTTPClient::GetMethod(int nMethod)
{	
	return nMethod<=0 ? RequestGetMethod : static_cast<CHTTPClient::enumRequestMethod>(nMethod);
}

CHTTPClient::enumTypePostArgument CHTTPClient::GetPostArgumentType(int nType)
{
	return nType<=0 ? TypeNormal : static_cast<CHTTPClient::enumTypePostArgument>(nType);
}

BOOL CHTTPClient::Connect(LPCTSTR szAddress, LPCTSTR szAgent, unsigned short nPort, LPCTSTR szUserAccount, LPCTSTR szPassword){

	m_hHTTPOpen=::InternetOpen(szAgent,	// agent name
		INTERNET_OPEN_TYPE_PRECONFIG,	// proxy option
		_T(""),								// proxy
		_T(""),								// proxy bypass
		0);								// flags

	if(NULL == m_hHTTPOpen)
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return FALSE;
	}	
	
	m_hHTTPConnection=::InternetConnect( m_hHTTPOpen,	// internet opened handle
							szAddress,				// server name
							nPort,					// ports
							szUserAccount,			// user name
							szPassword,				// password 
							INTERNET_SERVICE_HTTP,	// service type
							INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE,	// service option																														
							0);						// context call-back option
	

	if(NULL == m_hHTTPConnection)
	{		
		m_dwError=::GetLastError();
		::CloseHandle(m_hHTTPOpen);
		TRACEERROR(m_dwError);
		return FALSE;
	}

	if(::InternetAttemptConnect(NULL) != ERROR_SUCCESS)
	{		
		m_dwError=::GetLastError();
		::CloseHandle(m_hHTTPConnection);
		::CloseHandle(m_hHTTPOpen);
		TRACEERROR(m_dwError);
		return FALSE;
	}

	return TRUE;															
}

BOOL CHTTPClient::Close()
{
	if(m_hHTTPRequest)
		::InternetCloseHandle(m_hHTTPRequest);

	if(m_hHTTPConnection)
		::InternetCloseHandle(m_hHTTPConnection);

	if(m_hHTTPOpen)
		::InternetCloseHandle(m_hHTTPOpen);

	return TRUE;
}

BOOL CHTTPClient::Request(LPCTSTR szURL, int nMethod, LPCTSTR szAgent)
{
	BOOL  bReturn=TRUE;
	DWORD dwPort=0;
	TCHAR szProtocol[__SIZE_BUFFER] = _T("");
	TCHAR szAddress[__SIZE_BUFFER] = _T("");	
	TCHAR szURI[__SIZE_BUFFER] = _T("");
	DWORD dwSize=0;

	ParseURL(szURL, szProtocol, szAddress, dwPort, szURI);

	if( dwPort == INTERNET_DEFAULT_HTTPS_PORT )
		m_dwFlagSSL = INTERNET_FLAG_SECURE;
	else
		m_dwFlagSSL = 0;

	if(Connect(szAddress, szAgent, (WORD)dwPort))
	{		
		if(NULL == RequestOfURI(szURI, nMethod))
		{
			bReturn=FALSE;
		}
		else
		{
			if(!Response((PBYTE)m_szHTTPResponseHeader, __SIZE_HTTP_BUFFER, (PBYTE)m_szHTTPResponseHTML, __SIZE_HTTP_BUFFER, dwSize)){
				bReturn=FALSE;		
			}
		}
		Close();
	} 
	else 
	{
		bReturn=FALSE;
	}

	return bReturn;
}

BOOL CHTTPClient::RequestOfURI(LPCTSTR szURI, int nMethod){

	BOOL bReturn=TRUE;

	try
	{
		switch(nMethod)
		{
		case	RequestGetMethod:
		default:
			bReturn=RequestGet(szURI);
			break;
		case	RequestPostMethod:		
			bReturn=RequestPost(szURI);
			break;
		case	RequestPostMethodMultiPartsFormData:
			bReturn=RequestPostMultiPartsFormData(szURI);
			break;
		}
	}
#ifdef _DEBUG
	catch(TCHAR * str)
	{
		LOG_ERROR(str);
	}
#else
	catch( char* )
	{

	}
#endif

	return bReturn;
}

BOOL CHTTPClient::RequestGet(LPCTSTR szURI)
{
	CONST TCHAR *szAcceptType = __HTTP_ACCEPTYPE;

	m_hHTTPRequest=::HttpOpenRequest(
		m_hHTTPConnection,
		__HTTP_VERB_GET,		// HTTP Verb
		szURI,					// Object Name
		HTTP_VERSION,			// Version
		_T(""),						// Reference
		&szAcceptType,			// Accept Type
		INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | m_dwFlagSSL,
		0);						// context call-back point

	if(NULL == m_hHTTPRequest)
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return FALSE;
	}

	// REPLACE HEADER
	if(NULL == ::HttpAddRequestHeaders( m_hHTTPRequest, __HTTP_ACCEPT, (DWORD)_tcslen(__HTTP_ACCEPT), HTTP_ADDREQ_FLAG_REPLACE)){
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return FALSE;
	}

	// SEND REQUEST
	if( NULL == ::HttpSendRequest( m_hHTTPRequest,	// handle by returned HttpOpenRequest
		NULL,	// additional HTTP header
		0,		// additional HTTP header length
		NULL,	// additional data in HTTP Post or HTTP Put
		0) )	// additional data length
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return FALSE;
	}

	return TRUE;
}

BOOL CHTTPClient::RequestPost(LPCTSTR szURI){

	CONST TCHAR *	szAcceptType	=__HTTP_ACCEPTYPE;
	TCHAR			szPostArguments[__SIZE_BUFFER]= _T("");
	LPCTSTR			szContentType	= TEXT("Content-Type: application/x-www-form-urlencoded\r\n");

	DWORD dwPostArgumentsLegnth = GetPostArguments(szPostArguments, __SIZE_BUFFER);
	
	m_hHTTPRequest=::HttpOpenRequest( m_hHTTPConnection,
		__HTTP_VERB_POST,	// HTTP Verb
		szURI,				// Object Name
		HTTP_VERSION,		// Version
		_T(""),					// Reference
		&szAcceptType,		// Accept Type
		INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT | m_dwFlagSSL,
		0);					// context call-back point

	if(NULL == m_hHTTPRequest)
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return FALSE;
	}

	// REPLACE HEADER
	if(NULL == ::HttpAddRequestHeaders( m_hHTTPRequest, __HTTP_ACCEPT, (DWORD)_tcslen(__HTTP_ACCEPT), HTTP_ADDREQ_FLAG_REPLACE))
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return FALSE;
	}


	// SEND REQUEST
	if(NULL == ::HttpSendRequest( m_hHTTPRequest,	// handle by returned HttpOpenRequest
		szContentType,						// additional HTTP header
		(DWORD)_tcslen(szContentType),				// additional HTTP header length
		(LPVOID)szPostArguments,			// additional data in HTTP Post or HTTP Put
		(DWORD)_tcslen(szPostArguments)) )			// additional data length
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return FALSE;
	}

	return TRUE;
}

BOOL CHTTPClient::RequestPostMultiPartsFormData(LPCTSTR szURI)
{
	CONST TCHAR *szAcceptType=__HTTP_ACCEPTYPE;	
	LPCTSTR szContentType=TEXT("Content-Type: multipart/form-data; boundary=--MULTI-PARTS-FORM-DATA-BOUNDARY\r\n");		
	
	// ALLOCATE POST MULTI-PARTS FORM DATA ARGUMENTS
	PBYTE	pPostBuffer = NULL;
	DWORD	dwPostBufferLength = AllocMultiPartsFormData(pPostBuffer, _T("--MULTI-PARTS-FORM-DATA-BOUNDARY"));

	m_hHTTPRequest= ::HttpOpenRequest( m_hHTTPConnection,
			__HTTP_VERB_POST,	// HTTP Verb
			szURI,				// Object Name
			HTTP_VERSION,		// Version
			_T(""),					// Reference
			&szAcceptType,		// Accept Type
			INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT | m_dwFlagSSL,	// flags
			0);					// context call-back point

	if(NULL == m_hHTTPRequest)
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return FALSE;
	}

	// REPLACE HEADER
	if(!::HttpAddRequestHeaders( m_hHTTPRequest, __HTTP_ACCEPT, (DWORD)_tcslen(__HTTP_ACCEPT), HTTP_ADDREQ_FLAG_REPLACE))
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return FALSE;
	}

	if(!::HttpAddRequestHeaders( m_hHTTPRequest, szContentType, (DWORD)_tcslen(szContentType), HTTP_ADDREQ_FLAG_ADD_IF_NEW))
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return FALSE;
	}

	TCHAR	szContentLength[__SIZE_BUFFER] = _T("");
	::ZeroMemory(szContentLength, __SIZE_BUFFER);

	_stprintf( szContentLength, _T("Content-Length: %d\r\n"), dwPostBufferLength);

	if(NULL == ::HttpAddRequestHeaders( m_hHTTPRequest, szContentLength, (DWORD)_tcslen(szContentLength), HTTP_ADDREQ_FLAG_ADD_IF_NEW))
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return FALSE;
	}

#ifdef	_DEBUG
	TCHAR	szHTTPRequest[__SIZE_HTTP_BUFFER]= _T("");
	DWORD	dwHTTPRequestLength=__SIZE_HTTP_BUFFER;

	::ZeroMemory(szHTTPRequest, __SIZE_HTTP_BUFFER);
	if(NULL == ::HttpQueryInfo(m_hHTTPRequest, HTTP_QUERY_RAW_HEADERS_CRLF, szHTTPRequest, &dwHTTPRequestLength, NULL))
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		//return FALSE;
	}
#endif

	// SEND REQUEST WITH HttpSendRequestEx and InternetWriteFile
	INTERNET_BUFFERS InternetBufferIn={0};
	InternetBufferIn.dwStructSize=sizeof(INTERNET_BUFFERS);
	InternetBufferIn.Next=NULL;	
	
	if(NULL == ::HttpSendRequestEx(m_hHTTPRequest, &InternetBufferIn, NULL, HSR_INITIATE, 0))
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return FALSE;
	}

	DWORD dwOutPostBufferLength=0;
	if(NULL == ::InternetWriteFile(m_hHTTPRequest, pPostBuffer, dwPostBufferLength, &dwOutPostBufferLength))
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return FALSE;
	}

	if(!::HttpEndRequest(m_hHTTPRequest, NULL, HSR_INITIATE, 0))
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return FALSE;
	}

	// FREE POST MULTI-PARTS FORM DATA ARGUMENTS
	FreeMultiPartsFormData(pPostBuffer);

	return TRUE;
}

DWORD CHTTPClient::ResponseOfBytes(PBYTE pBuffer, DWORD dwSize)
{
	DWORD dwBytes=0;
	if(NULL == m_hHTTPRequest)
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return 0;
	}

	::ZeroMemory(pBuffer, dwSize);
	if(!::InternetReadFile(	m_hHTTPRequest,
		pBuffer,
		dwSize,
		&dwBytes))
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return 0;
	}

	return dwBytes;
}

BOOL CHTTPClient::Response(PBYTE pHeaderBuffer, DWORD dwHeaderBufferLength, PBYTE pBuffer, DWORD dwBufferLength, DWORD &dwResultSize)
{
	BYTE pResponseBuffer[__SIZE_BUFFER]="";	
	DWORD dwNumOfBytesToRead=0;

	if(NULL == m_hHTTPRequest)
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return FALSE;
	}

	::ZeroMemory(pBuffer, dwBufferLength);
	dwResultSize=0;
	while((dwNumOfBytesToRead=ResponseOfBytes(pResponseBuffer, __SIZE_BUFFER))!=NULL && dwResultSize<dwBufferLength)
	{
		::CopyMemory( (pBuffer+dwResultSize), pResponseBuffer, dwNumOfBytesToRead);		
		dwResultSize+=dwNumOfBytesToRead;
	}

	::ZeroMemory(pHeaderBuffer, dwHeaderBufferLength);
	if(!::HttpQueryInfo(m_hHTTPRequest, HTTP_QUERY_RAW_HEADERS_CRLF, pHeaderBuffer, &dwHeaderBufferLength, NULL))
	{
		m_dwError=::GetLastError();
		TRACEERROR(m_dwError);
		return FALSE;
	}

	return (dwResultSize? TRUE: FALSE);
}

VOID CHTTPClient::AddPostArguments(LPCTSTR szName, LPCTSTR szValue, BOOL bBinary)
{
	GenericHTTPArgument	arg;
	::ZeroMemory(&arg, sizeof(arg));

	_tcscpy(arg.szName, szName);
	_tcscpy(arg.szValue, szValue);

	if(!bBinary)
		arg.dwType = TypeNormal;
	else
		arg.dwType = TypeBinary;	

	m_stl_vector_Arguments.push_back(arg);
	return;
}

VOID CHTTPClient::AddPostArguments(LPCTSTR szName, DWORD nValue)
{
	GenericHTTPArgument	arg;
	::FillMemory(&arg, sizeof(arg), 0x00);

	_tcscpy(arg.szName, szName);
	_stprintf(arg.szValue, _T("%d"), nValue);
	arg.dwType = TypeNormal;

	m_stl_vector_Arguments.push_back(arg);

	return;
}

DWORD CHTTPClient::GetPostArguments(LPTSTR szArguments, DWORD dwLength)
{
	std::vector<GenericHTTPArgument>::iterator itArg;

	::ZeroMemory(szArguments, dwLength);
	for(itArg=m_stl_vector_Arguments.begin(); itArg<m_stl_vector_Arguments.end(); )
	{
		TCSCAT(szArguments, _countof(szArguments), itArg->szName);
		TCSCAT(szArguments, _countof(szArguments), _T("="));
		TCSCAT(szArguments, _countof(szArguments), itArg->szValue);

		if((++itArg)<m_stl_vector_Arguments.end()){
			TCSCAT(szArguments, _countof(szArguments), _T("&"));
		}
	}

	*(szArguments+dwLength)='\0';

	return (DWORD)_tcslen(szArguments);
}


VOID CHTTPClient::InitilizePostArguments()
{
	m_stl_vector_Arguments.clear();
	return;
}

VOID CHTTPClient::FreeMultiPartsFormData(PBYTE &pBuffer){

	if(pBuffer)
	{
		::LocalFree(pBuffer);
		pBuffer=NULL;
	}

	return;
}

DWORD CHTTPClient::AllocMultiPartsFormData(PBYTE &pInBuffer, LPCTSTR szBoundary){

	if(pInBuffer)
	{
		::LocalFree(pInBuffer);
		pInBuffer=NULL;
	}

	pInBuffer=(PBYTE)::LocalAlloc(LPTR, GetMultiPartsFormDataLength());	
	std::vector<GenericHTTPArgument>::iterator itArgv;

	DWORD dwPosition=0;
	DWORD dwBufferSize=0;

	for(itArgv=m_stl_vector_Arguments.begin(); itArgv<m_stl_vector_Arguments.end(); ++itArgv)
	{
		PBYTE pBuffer=NULL;

		// SET MULTI_PRATS FORM DATA BUFFER
		switch(itArgv->dwType)
		{
		case	TypeNormal:
			{
				pBuffer=(PBYTE)::LocalAlloc(LPTR, __SIZE_HTTP_HEAD_LINE*4);
				_stprintf(	(TCHAR*)pBuffer,							
					_T("--%s\r\n")
					_T("Content-Disposition: form-data; name=\"%s\"\r\n")
					_T("\r\n")
					_T("%s\r\n"),
					szBoundary,
					itArgv->szName,
					itArgv->szValue);

				dwBufferSize= (DWORD)_tcslen((TCHAR*)pBuffer);
			}
			break;
		case	TypeBinary:
			{
				DWORD	dwNumOfBytesToRead=0;
				DWORD	dwTotalBytes=0;

				HANDLE	hFile=::CreateFile(itArgv->szValue, 
					GENERIC_READ,				// desired access
					FILE_SHARE_READ,			// share mode
					NULL,						// security attribute
					OPEN_EXISTING,				// create disposition
					FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, // flag and attributes
					NULL);						// template file handle

				if( hFile == INVALID_HANDLE_VALUE )
					break;

				DWORD	dwSize=::GetFileSize(hFile, NULL);

				pBuffer=(PBYTE)::LocalAlloc(LPTR, __SIZE_HTTP_HEAD_LINE*3+dwSize+1);
				BYTE	pBytes[__SIZE_BUFFER+1]="";

				_stprintf( (TCHAR*)pBuffer,
					_T("--%s\r\n")
					_T("Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n")
					_T("Content-Type: %s\r\n")
					_T("\r\n"),
					szBoundary,
					itArgv->szName, itArgv->szValue,
					GetContentType(itArgv->szValue)
					);

				DWORD	dwBufPosition= (DWORD)_tcslen((TCHAR*)pBuffer);	
				while(::ReadFile(hFile, pBytes, __SIZE_BUFFER, &dwNumOfBytesToRead, NULL) && dwNumOfBytesToRead>0 && dwTotalBytes<=dwSize)
				{
					::CopyMemory((pBuffer+dwBufPosition+dwTotalBytes), pBytes, dwNumOfBytesToRead);
					::ZeroMemory(pBytes, __SIZE_BUFFER+1);
					dwTotalBytes+=dwNumOfBytesToRead;				
				}

				::CopyMemory((pBuffer+dwBufPosition+dwTotalBytes), _T("\r\n"), _tcslen(_T("\r\n")));
				::CloseHandle(hFile);

				dwBufferSize=dwBufPosition+dwTotalBytes+_tcslen(_T("\r\n"));
			}
			break;			
		}

		::CopyMemory((pInBuffer+dwPosition), pBuffer, dwBufferSize);
		dwPosition+=dwBufferSize;

		if(pBuffer){
			::LocalFree(pBuffer);
			pBuffer=NULL;
		}
	}

	::CopyMemory((pInBuffer+dwPosition), "--", 2);
	::CopyMemory((pInBuffer+dwPosition+2), szBoundary, _tcslen(szBoundary));
	::CopyMemory((pInBuffer+dwPosition+2+_tcslen(szBoundary)), "--\r\n", 4);

	return dwPosition+6+_tcslen(szBoundary);
}

DWORD CHTTPClient::GetMultiPartsFormDataLength()
{
	DWORD dwLength=0;
	std::vector<GenericHTTPArgument>::iterator itArgv;

	for(itArgv=m_stl_vector_Arguments.begin(); itArgv<m_stl_vector_Arguments.end(); ++itArgv)
	{
		switch(itArgv->dwType)
		{
		case	TypeNormal:
			{
				dwLength+=__SIZE_HTTP_HEAD_LINE*4;
			}
			break;
		case	TypeBinary:
			{
				HANDLE	hFile=::CreateFile(itArgv->szValue, 
					GENERIC_READ,			// desired access
					FILE_SHARE_READ,		// share mode
					NULL,					// security attribute
					OPEN_EXISTING,			// create disposition
					FILE_ATTRIBUTE_NORMAL,	// flag and attributes
					NULL);					// template file handle

				if( hFile == INVALID_HANDLE_VALUE )
					break;

				dwLength+=__SIZE_HTTP_HEAD_LINE*3+::GetFileSize(hFile, NULL);
				::CloseHandle(hFile);			
			}
			break;
		}

	}

	return dwLength;
}

tstring CHTTPClient::GetContentType(LPCTSTR szName)
{
	TCHAR	szReturn[1024]= _T("");
	const TCHAR* szDefaultReturn = _T("application/octet-stream");
	LONG	dwLen=1024;
	DWORD	dwDot=0;

	for(dwDot= (DWORD)_tcslen(szName);dwDot>0;dwDot--)
	{
		if(!_tcsncmp((szName+dwDot), _T("."), 1))
			break;
	}

	HKEY	hKEY;
	LPTSTR	szWord=(TCHAR*)(szName+dwDot);

	if(RegOpenKeyEx(HKEY_CLASSES_ROOT, szWord, 0, KEY_QUERY_VALUE, &hKEY)==ERROR_SUCCESS) 
	{
		if(RegQueryValueEx(hKEY, TEXT("Content Type"), NULL, NULL, (LPBYTE)szReturn, (unsigned long*)&dwLen)!=ERROR_SUCCESS)
			_tcsncpy(szReturn, szDefaultReturn, _tcslen(szDefaultReturn));

		RegCloseKey(hKEY);
	} 
	else 
	{
		_tcsncpy(szReturn, szDefaultReturn, _tcslen(szDefaultReturn));
	}

	return szReturn;
}

DWORD CHTTPClient::GetLastError()
{
	return m_dwError;
}

VOID CHTTPClient::ParseURL(LPCTSTR szURL, LPTSTR szProtocol, LPTSTR szAddress, DWORD &dwPort, LPTSTR szURI)
{
	TCHAR	szPort[__SIZE_BUFFER]= _T("");
	DWORD	dwPosition=0;
	BOOL	bFlag=FALSE;

	while(_tcslen(szURL)>0 && dwPosition<_tcslen(szURL) && _tcsncmp((szURL+dwPosition), _T(":"), 1))
		++dwPosition;

	if(!_tcsncmp((szURL+dwPosition+1), _T("/"), 1)) //isProtocol
	{	
		if(szProtocol)
		{
			_tcsncpy(szProtocol, szURL, dwPosition);
			szProtocol[dwPosition]=0;
		}
		bFlag=TRUE;
	}
	else //is Host
	{	
		if(szProtocol)
		{
			_tcsncpy(szProtocol, _T("http"), 4);
			szProtocol[5]=0;
		}
	}

	DWORD dwStartPosition=0;
	if(bFlag)
	{
		dwStartPosition=dwPosition+=3;				
	}
	else{
		dwStartPosition=dwPosition=0;
	}

	bFlag=FALSE;
	while(_tcslen(szURL)>0 && dwPosition<_tcslen(szURL) && _tcsncmp((szURL+dwPosition), _T("/"), 1))
			++dwPosition;

	DWORD dwFind=dwStartPosition;

	for(;dwFind<=dwPosition;dwFind++)
	{
		if(!_tcsncmp((szURL+dwFind), _T(":"), 1))
		{	// find PORT
			bFlag=TRUE;
			break;
		}
	}

	if(bFlag)
	{
		TCHAR sztmp[__SIZE_BUFFER]= _T("");
		_tcsncpy(sztmp, (szURL+dwFind+1), dwPosition-dwFind);
		dwPort=_ttol(sztmp);
		_tcsncpy(szAddress, (szURL+dwStartPosition), dwFind-dwStartPosition);
	}
	else if(!_tcscmp(szProtocol, _T("https")))
	{
		dwPort=INTERNET_DEFAULT_HTTPS_PORT;
		_tcsncpy(szAddress, (szURL+dwStartPosition), dwPosition-dwStartPosition);
	}
	else 
	{
		dwPort=INTERNET_DEFAULT_HTTP_PORT;
		_tcsncpy(szAddress, (szURL+dwStartPosition), dwPosition-dwStartPosition);
	}

	if(dwPosition<_tcslen(szURL))
	{	// find URI
		_tcsncpy(szURI, (szURL+dwPosition), _tcslen(szURL)-dwPosition);
	} else {
		szURI[0]=0;
	}

	return;
}

LPCTSTR CHTTPClient::QueryHTTPResponseHeader()
{
	return m_szHTTPResponseHeader;
}

LPCTSTR CHTTPClient::QueryHTTPResponse()
{
	return m_szHTTPResponseHTML;
}

} //namespace Redmoon
#pragma once

#include "Common.h"

#include <tchar.h>
#include <wininet.h>
#include <vector>

namespace Redmoon
{

// PRE-DEFINED CONSTANTS
#define __DEFAULT_AGENT_NAME		_T("MERONG(0.9/;p)")

// PRE-DEFINED BUFFER SIZE
#define	__SIZE_HTTP_ARGUMENT_NAME	256
#define __SIZE_HTTP_ARGUMENT_VALUE	1024*64

#define __SIZE_HTTP_BUFFER			100000
#define __SIZE_HTTP_RESPONSE_BUFFER	100000
#define __SIZE_HTTP_HEAD_LINE		2048

#define __SIZE_BUFFER				1024
#define __SIZE_SMALL_BUFFER			256

class CHTTPClient 
{
public:					
	typedef struct __GENERIC_HTTP_ARGUMENT										// ARGUMENTS STRUCTURE
	{							
		TCHAR	szName[__SIZE_HTTP_ARGUMENT_NAME];
		TCHAR	szValue[__SIZE_HTTP_ARGUMENT_VALUE];
		DWORD	dwType;

		operator==(const __GENERIC_HTTP_ARGUMENT &argV)
		{
			return !_tcscmp(szName, argV.szName) && !_tcscmp(szValue, argV.szValue);
		}
	} GenericHTTPArgument;

	enum enumRequestMethod														// REQUEST METHOD
	{															
		RequestUnknown=0,
		RequestGetMethod=1,
		RequestPostMethod=2,
		RequestPostMethodMultiPartsFormData=3
	};

	enum enumTypePostArgument													// ARGUMENTS STRUCTURE
	{																		
		TypeUnknown=0,
		TypeNormal=1,
		TypeBinary=2
	};

	// CONSTRUCTOR & DESTRUCTOR
	CHTTPClient();
	virtual ~CHTTPClient();

	static enumRequestMethod	GetMethod(int nMethod);
	static enumTypePostArgument GetPostArgumentType(int nType);

	// Connection handler	
	BOOL	Connect(LPCTSTR szAddress, LPCTSTR szAgent = __DEFAULT_AGENT_NAME, unsigned short nPort = INTERNET_DEFAULT_HTTP_PORT, LPCTSTR szUserAccount = NULL, LPCTSTR szPassword = NULL);
	BOOL	Close();
	VOID	InitilizePostArguments();

	// HTTP Arguments handler	
	VOID	AddPostArguments(LPCTSTR szName, DWORD nValue);
	VOID	AddPostArguments(LPCTSTR szName, LPCTSTR szValue, BOOL bBinary = FALSE);

	// HTTP Method handler 
	BOOL	Request(LPCTSTR szURL, int nMethod = RequestGetMethod, LPCTSTR szAgent = __DEFAULT_AGENT_NAME);
	BOOL	RequestOfURI(LPCTSTR szURI, int nMethod = RequestGetMethod);
	BOOL	Response(PBYTE pHeaderBuffer, DWORD dwHeaderBufferLength, PBYTE pBuffer, DWORD dwBufferLength, DWORD &dwResultSize);	
	LPCTSTR QueryHTTPResponse();
	LPCTSTR QueryHTTPResponseHeader();	

	// General Handler
	DWORD	GetLastError();
	tstring GetContentType(LPCTSTR szName);
	VOID	ParseURL(LPCTSTR szURL, LPTSTR szProtocol, LPTSTR szAddress, DWORD &dwPort, LPTSTR szURI);

	
protected:				
	std::vector<GenericHTTPArgument> m_stl_vector_Arguments;			// POST ARGUMENTS VECTOR

	TCHAR		m_szHTTPResponseHTML[__SIZE_HTTP_BUFFER];	// RECEIVE HTTP BODY
	TCHAR		m_szHTTPResponseHeader[__SIZE_HTTP_BUFFER];	// RECEIVE HTTP HEADR

	HINTERNET	m_hHTTPOpen;									// internet open handle
	HINTERNET	m_hHTTPConnection;							// internet connection hadle
	HINTERNET	m_hHTTPRequest;								// internet request hadle

	DWORD		m_dwError;									// LAST ERROR CODE
	LPCTSTR		m_szHost;									//	 HOST NAME
	DWORD		m_dwPort;									//  PORT
	DWORD		m_dwFlagSSL;									// HTTPS PROTOCOL REQUEST FLAG

	// HTTP Method handler
	DWORD	ResponseOfBytes(PBYTE pBuffer, DWORD dwSize);
	DWORD	GetPostArguments(LPTSTR szArguments, DWORD dwLength);
	BOOL	RequestPost(LPCTSTR szURI);
	BOOL	RequestPostMultiPartsFormData(LPCTSTR szURI);
	BOOL	RequestGet(LPCTSTR szURI);
	DWORD	AllocMultiPartsFormData(PBYTE &pInBuffer, LPCTSTR szBoundary = _T("--MULTI-PARTS-FORM-DATA-BOUNDARY-"));
	VOID	FreeMultiPartsFormData(PBYTE &pBuffer);
	DWORD	GetMultiPartsFormDataLength();
};

void	PrintLastError( DWORD lastError );

} //namespace Redmoon
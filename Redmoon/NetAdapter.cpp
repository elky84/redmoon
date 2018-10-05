#include "NetAdapter.h"

namespace Redmoon
{

CNetAdapter::CNetAdapter()
{
	DWORD dwRetVal = 0;
	IP_ADAPTER_INFO* pAdapter = NULL;
	m_pAdapterInfo.Reset((IP_ADAPTER_INFO* ) malloc(sizeof(IP_ADAPTER_INFO)));
	unsigned long ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo( m_pAdapterInfo.Get(), &ulOutBufLen) != ERROR_SUCCESS) 
	{
		m_pAdapterInfo.Reset();
		m_pAdapterInfo.Reset((IP_ADAPTER_INFO*) malloc(ulOutBufLen));
	}

	if ((dwRetVal = GetAdaptersInfo(m_pAdapterInfo.Get(), &ulOutBufLen)) == NO_ERROR) 
	{
		LOG_DEBUG(_T("GetAdapterInfo Success"));
		pAdapter = m_pAdapterInfo.Get();
		while(pAdapter)
		{
			m_stl_vector_IPAdapterInfo.push_back(pAdapter);
			pAdapter = pAdapter->Next;
		}
	}
	else 
	{
		LOG_ERROR(_T("Call to GetAdaptersInfo failed."));
	}
}

CNetAdapter::~CNetAdapter()
{
	
}

PIP_ADAPTER_INFO CNetAdapter::GetIPAdapterInfo(const char* szIP)
{
	for(unsigned int i = 0; i < m_stl_vector_IPAdapterInfo.size(); i++)
	{
		if(_stricmp(GetIPAddress(m_stl_vector_IPAdapterInfo[i]), szIP) == 0)
		{
			return m_stl_vector_IPAdapterInfo[i];
		}
	}
	return NULL;
}

}
#pragma once

#include "Common.h"
#include <IPHlpApi.h>
#include "AutoPtr.h"
#include <vector>

#pragma comment(lib, "Iphlpapi.lib")

namespace Redmoon
{

/**
@brief 네트웍 장치 정보 얻어오는 클래스
*/
class CNetAdapter
{
	CAutoPtr<IP_ADAPTER_INFO> m_pAdapterInfo;
	std::vector<PIP_ADAPTER_INFO> m_stl_vector_IPAdapterInfo;
	CNetAdapter();
	~CNetAdapter();

public:

	inline BYTE* GetMacAddress(PIP_ADAPTER_INFO adapterinfo)
	{
		return adapterinfo->Address;
	}

	inline char* GetPrimaryWinsIPAddress(PIP_ADAPTER_INFO adapterinfo)
	{
		return adapterinfo->PrimaryWinsServer.IpAddress.String;
	}

	inline char* GetSecondWinsIPAddress(PIP_ADAPTER_INFO adapterinfo)
	{
		return adapterinfo->SecondaryWinsServer.IpAddress.String;
	}

	inline char* GetDhcpServerIPAddress(PIP_ADAPTER_INFO adapterinfo)
	{
		return adapterinfo->DhcpServer.IpAddress.String;
	}

	char* GetIPAddress(PIP_ADAPTER_INFO adapterinfo){
		return adapterinfo->IpAddressList.IpAddress.String;
	}

	char* GetAdapterName(PIP_ADAPTER_INFO adapterinfo){
		return adapterinfo->AdapterName;
	}

	char* GetDesc(PIP_ADAPTER_INFO adapterinfo){
		return adapterinfo->Description;
	}

	char* GetMask(PIP_ADAPTER_INFO adapterinfo){
		return adapterinfo->IpAddressList.IpMask.String;
	}

	char* GetGateWay(PIP_ADAPTER_INFO adapterinfo){
		return adapterinfo->GatewayList.IpAddress.String;
	}

	PIP_ADAPTER_INFO GetIPAdapterInfo(const char* szIP); //통신에 쓰인 IP에 해당하는 PIP 정보 얻어온다.

	///싱글턴으로 선언
	STATIC_SINGLETON_PTR(CNetAdapter);
};

inline CNetAdapter* GetNetAdapter()
{
	return CNetAdapter::InstancePtr();
}

} //namespace Redmoon
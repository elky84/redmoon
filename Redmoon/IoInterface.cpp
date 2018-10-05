#include "IoInterface.h"
#include "Peer.h"

namespace Redmoon
{

/**
@brief 생성자. 피어를 지정해준다.
@param peer 피어
*/
CIoInterface::CIoInterface(CPeer* pcPeer)
{
	if(pcPeer)
	{
		SetPeer(pcPeer);
	}
}

/**
@brief 소멸자. 아무일도 하지 않음.
*/
CIoInterface::~CIoInterface()
{

}

/**
@brief 피어 설정. 기존 피어가 있다면 삭제하고 피어를 세팅해준다.
*/
void CIoInterface::SetPeer(CPeer* pcPeer)
{
	m_pcPeer.Reset(pcPeer);
	m_pcPeer->SetIOInterface(this);
}

} //namespace Redmoon
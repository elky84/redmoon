#pragma once

#include "Common.h"

#include "SmartSingleton.h"
#include "EventWorker.h"

namespace Redmoon
{

class CPipe;

/**
@brief Pipe�� �񵿱� ó���� ���� Ŭ����.
*/
class CPipeContext : public STRUCT_EX<OVERLAPPED>, public CSmartSingleton< CEventWorker<CPipeContext> >
{
public:
	///������
	CPipeContext(CPipe* pcPipe, IO_TYPE eIoType);

	///�Ҹ���
	virtual ~CPipeContext();

	/**
	@brief IOŸ���� ��ȯ�Ѵ�.
	*/
	IO_TYPE GetIoType(){return m_eIoType;}

	/**
	@brief WSAEvent�� ��ȯ�ϴ� �޼ҵ�
	@return ������� WSAEVENT �ڵ�
	*/
	HANDLE GetEvent(){return hEvent;}

	///EventWorker�� ��� ��û
	void Register();

	/// ��ȣ ������ �ҷ��� �޼ҵ�
	void Proc();

private:
	///� IO�� ���� ��û������ ���� ��
	IO_TYPE m_eIoType;

	///������ ��ü ������
	CPipe* m_pcPipe;
};

} //namespace Redmoon
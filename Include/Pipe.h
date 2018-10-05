#pragma once

#include "Common.h"
#include "PipeContext.h"
#include "Peer.h"
#include "AutoPtr.h"
#include "IoInterface.h"
#include "Buffer.h"

namespace Redmoon
{

const int PIPE_BUF_SIZE = 1024;

/**
@brief ������ ����� ������ Ŭ����. Ŭ���̾�Ʈ/������ ���� ����� ����Ѵ�.
*/
class CPipe : public CIoInterface
{
	friend class CPipeContext;
public:
	///������.
	CPipe(const tstring& strName, CPeer* pcPeer = new CPeer);

	///�Ҹ���
	virtual ~CPipe();

	///Disconnect ���� ���� �Լ�
	virtual void Disconnect() = 0;

	///�ֱ����� �ݺ� ����
	virtual void Act() = 0;

	///������ ����
	virtual bool Send(const unsigned int nLen, const void* pData, const unsigned int nAddLen = 0, const void* pAddData = 0);

	///������ �ڵ� ��ȯ
	const HANDLE& GetPipe(){return m_hPipe;}

	/**
	@brief ���ú� ������ ��� ���� ��ȯ
	@return ���ú� ������ ��� ����
	*/
	char* GetRecvHeadBuffer(){return m_pcRecvBuffer->GetHeadBuffer();}

	/**
	@brief ���� ���� �÷��� ��
	@return ���� ����
	*/
	BOOL isConnected(){return m_bConnected;}

protected:
	/**
	@brief ���� Ʈ���� ������ ����
	@return ���� Ʈ���� �� ����
	*/
	BOOL isRefreshing(){return m_bRefreshing;}

	/**
	@brief ���� �õ� ������ ����
	@return ���� �õ��� ����
	*/
	BOOL isTryConnect(){return m_bTryConnect;}

	/**
	@brief ���� �õ� �÷��װ� ���� �޼ҵ�
	@param bTryConnect ���� �õ� �÷��� ��
	*/
	void SetTryConnect(BOOL bTryConnect){m_bTryConnect = bTryConnect;}

	/**
	@brief ������ �ڵ� ����
	@param hPipe ������ �ڵ�
	*/
	void SetPipe(HANDLE& hPipe){m_hPipe = hPipe;}

	/**
	@brief ������ �̸� ��ȯ
	@return ������ �̸�
	*/
	const tstring& GetName(){return m_strName;}

	/**
	@brief ������ Accept ���ؽ�Ʈ ��ȯ
	@return Accept ���ؽ�Ʈ
	*/
	CPipeContext* GetPipeContextAccept(){return &m_cPipeContextAccept;}

	/**
	@brief ������ Recv ���ؽ�Ʈ ��ȯ
	@return Recv ���ؽ�Ʈ
	*/
	CPipeContext* GetPipeContextRecv(){return &m_cPipeContextRecv;}

	/**
	@brief ������ Send ���ؽ�Ʈ ��ȯ
	@return Send ���ؽ�Ʈ
	*/
	CPipeContext* GetPipeContextSend(){return &m_cPipeContextSend;}

	///�̺�Ʈ ��Ŀ�� ��� ��û
	void Register();

	/// Receive ��� ��û
	bool Receive();

	/// ���� ���ɽ� ȣ���ϴ� �޼ҵ�
	virtual void Reuse();

protected:
	/// ���� �Ϸ�� �̺�Ʈ �޼ҵ�
	virtual void OnConnect();

	/// Receive �Ϸ�� �̺�Ʈ �޼ҵ�
	virtual bool OnReceive(unsigned int nLen, void* pData);

	/// Send �Ϸ�� �̺�Ʈ �޼ҵ�
	virtual void OnSend(unsigned int nLen);

	/// ���� ������ �̺�Ʈ �޼ҵ�
	virtual void OnDisconnect();

private:
	///���� ���� �÷��� ��
	BOOL m_bConnected;

	///�������� �÷��� ��
	BOOL m_bRefreshing;

	///���� �õ� ���� �÷��� ��
	BOOL m_bTryConnect;

	///������ �ڵ�
	HANDLE m_hPipe;

	///������ �̸�
	tstring m_strName;

	///Connect�� ������ ���ؽ�Ʈ
	CPipeContext m_cPipeContextAccept;

	///Recv�� ������ ���ؽ�Ʈ
	CPipeContext m_cPipeContextRecv;

	///Send�� ������ ���ؽ�Ʈ
	CPipeContext m_cPipeContextSend;

	///Send�� ����
	CAutoPtr<CBuffer> m_pcSendBuffer;

	///Recv�� ����
	CAutoPtr<CBuffer> m_pcRecvBuffer;

	///Send ����ȭ�� ���� ũ��Ƽ�� ���� ��ü
	CCriticalSection m_cSendLock;
};

} //namespace Redmoon
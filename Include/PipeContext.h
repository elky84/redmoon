#pragma once

#include "Common.h"

#include "SmartSingleton.h"
#include "EventWorker.h"

namespace Redmoon
{

class CPipe;

/**
@brief Pipe의 비동기 처리를 위한 클래스.
*/
class CPipeContext : public STRUCT_EX<OVERLAPPED>, public CSmartSingleton< CEventWorker<CPipeContext> >
{
public:
	///생성자
	CPipeContext(CPipe* pcPipe, IO_TYPE eIoType);

	///소멸자
	virtual ~CPipeContext();

	/**
	@brief IO타입을 반환한다.
	*/
	IO_TYPE GetIoType(){return m_eIoType;}

	/**
	@brief WSAEvent를 반환하는 메소드
	@return 사용중인 WSAEVENT 핸들
	*/
	HANDLE GetEvent(){return hEvent;}

	///EventWorker에 등록 요청
	void Register();

	/// 신호 왔을때 불려질 메소드
	void Proc();

private:
	///어떤 IO에 대한 요청인지에 대한 값
	IO_TYPE m_eIoType;

	///파이프 객체 포인터
	CPipe* m_pcPipe;
};

} //namespace Redmoon
#pragma once

namespace Redmoon
{

typedef unsigned int BUFFER_SIZE;
typedef unsigned int BUFFER_POSITION;

/**
@brief 선형 버퍼 클래스
*/
class CBuffer
{
public:
	///생성자 버퍼 크기만큼 할당
	CBuffer(BUFFER_SIZE nBufferSize);

	///복사 생성자. 해당 버퍼와 복사
	CBuffer(CBuffer* pcBuffer);

	///복사 생성자. 넘어온 데이터의 크기만큼의 버퍼를 만들고, 넘어온 데이터를 집어넣는다.
	CBuffer(const BUFFER_SIZE nLen, const void* data, const BUFFER_SIZE nAddLen = 0, const void* pAddData = 0);

	///소멸자. 소유한 데이터 해제.
	~CBuffer();

	///초기화 메소드.
	void Init();

	/**
	@brief 버퍼에서 사용중인 데이터 크기 반환
	@return 사용중인 데이터양
	*/
	inline BUFFER_SIZE GetUsingSize(){return GetTail()-GetHead();}

	/**
	@brief 버퍼에서 사용가능한 남은 크기 반환
	@return 사용가능한 남은양
	*/
	inline BUFFER_SIZE GetRemainSize(){return GetSize() - GetUsingSize();}

	/**
	@brief 버퍼에서의 유효한 선두 값 반환
	@return 버퍼에서 유효한 선두 값
	*/
	inline BUFFER_POSITION GetHead(){return m_nHead;}

	/**
	@brief 버퍼에서의 유효한 끝 값 반환
	@return 버퍼에서 유효한 끝 값
	*/
	inline BUFFER_POSITION GetTail(){return m_nTail;}

	/**
	@brief 버퍼의 크기 반환
	@return 버퍼의 크기
	*/
	inline BUFFER_SIZE GetSize(){return m_nBufferSize;}

	/**
	@brief 버퍼 포인터 반환
	@return 버퍼 포인터
	*/
	inline char* GetBuffer(){return m_pcBuffer;}

	/**
	@brief Head버퍼 반환
	@return 현재 사용가능한 버퍼의 Head값 반환
	*/
	inline char* GetHeadBuffer(){return GetBuffer(GetHead());}

	/**
	@brief 버퍼의 Head값 초기화
	*/
	inline char* GetTailBuffer(){return GetBuffer(GetTail());}

	///버퍼에 데이터를 추가하는 메소드
	bool AddData(const BUFFER_SIZE nLen, const void* pData, const BUFFER_SIZE nAddLen = 0, const void* pAddData = 0);
	
	///Head값만큼 사용했다 판단하고, 사용후 처리를 한다.
	bool JumpHead(BUFFER_POSITION nPosition);

	///Tail값만큼 사용했다 판단하고, 사용후 처리를 한다.
	bool JumpTail(BUFFER_POSITION nPosition);

	///데이터를 압축한다.
	void Compress();

private:
	/**
	@brief 버퍼의 특정 위치 포인터를 반환한다.
	@param 시작 위치를 기준으로 해서 at에 해당하는 포인터를 반환
	@return 요청한 버퍼 위치 포인터. 범위 넘어서면 NULL반환
	*/
	inline char* GetBuffer(BUFFER_SIZE at)
	{
		if(m_nBufferSize <= at){
			return NULL;
		}
		return &m_pcBuffer[at];
	}

	/**
	@brief 버퍼의 Head값 증가
	@param nHead 증가 시킬 Head값
	*/
	inline void _AddHead(BUFFER_POSITION nHead){m_nHead += nHead;}

	/**
	@brief 버퍼의 Tail값 증가
	@param nTail 증가 시킬 Tail값
	*/
	inline void _AddTail(BUFFER_SIZE nTail){m_nTail += nTail;}

	/**
	@brief 버퍼의 Head값 초기화
	*/
	inline void _InitHead(){m_nHead = 0;}

	/**
	@brief 버퍼의 Tail값 초기화
	*/
	inline void _InitTail(){m_nTail = 0;}
	
private:
	///시작 위치(=m_pcBuffer)를 기준으로한 현재 유효한 데이터의 맨 앞 위치인 Head값
	BUFFER_POSITION m_nHead;

	///시작 위치(=m_pcBuffer)를 기준으로한 현재 유효한 데이터의 끝 위치인 Tail값
	BUFFER_POSITION m_nTail;

	///버퍼의 전체 크기
	BUFFER_SIZE m_nBufferSize;

	///버퍼 포인터
	char* m_pcBuffer;
}; 

} //namespace Redmoon
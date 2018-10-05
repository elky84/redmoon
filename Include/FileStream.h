#pragma once

#include "Str.h"
#include "FileUtil.h"

namespace Redmoon
{

/**
@brief 파일 제어 기능을 랩핑한 클래스
*/
class CFileStream
{
public:
	/**
	@brief 소멸자. 열린 파일을 닫는다.
	*/
	virtual ~CFileStream()
	{
		Close();
	}

	///파일을 오픈하는 메소드. 순수 가상함수이므로 직접 정의해주어야 함.
	virtual bool Open(const tstring& strFileName) = 0;

	/**
	@brief 파일을 닫는 메소드
	@return 파일 닫기 성공 여부
	*/
	bool Close()
	{
		if(CFileUtil::Close(m_Fp))
		{
			m_nSize = 0;
			return true;
		}
		return false;
	}

	/**
	@brief 재정의 하지 않으면, 동작을 하지 않는 읽기 동작 메소드.
	@param nSize 읽을 크기
	@parm pData 읽어올 데이터 버퍼
	@return 성공 여부
	*/
	virtual bool Read(size_t nSize, void* pData)
	{
		LOG_ERROR(_T("%s Can't use this function. You'll use this function overloaded class."), __TFUNCTION__);
		return false;	
	}

	/**
	@brief 재정의 하지 않으면, 동작을 하지 않는 쓰기 동작 메소드.
	@param nSize 쓸 크기
	@parm pData 쓸 데이터 버퍼
	@return 성공 여부
	*/
	virtual bool Write(size_t nSize, void* pData)
	{
		LOG_ERROR(_T("%s Can't use this function. You'll use this function overloaded class."), __TFUNCTION__);
		return false;
	}

	/**
	@brief IO중인 크기 얻어내기
	@return IO중인 크기
	@desc 파일에서 유효한 데이터의 크기를 의미한다. 읽어내면 감소, 쓰면 증가한다.
	*/
	size_t Size(){return m_nSize;}

protected:
	/**
	@brief 빈 생성자. 상속을 통해서만 구현이 가능하도록, protected로 지정.
	*/
	CFileStream() : m_Fp(NULL), m_nSize(0)
	{
		
	}

	/**
	@brief 파일을 여는 메소드
	@param strFileName 파일명
	@param szMode 파일 오픈 모드 (ex:rt,wt,rb...)
	@return 성공 여부. 
	@desc 이미 사용중인 파일이 있다면, 파일 여는 동작에 실패한다.
	*/
	bool _Open(const tstring& strFileName, const TCHAR* szMode)
	{
		if(m_Fp != NULL)
		{
			LOG_INFO(_T("%s. Already Opened File. [%s]"), __TFUNCTION__, m_strName.c_str());
			return false;
		}

		m_Fp = CFileUtil::Open(strFileName, szMode);
		if(m_Fp == NULL)
		{
			LOG_INFO(_T("%s. Can't open file. [%s, %s]"), __TFUNCTION__, strFileName.c_str(), szMode);
			return false;
		}
		m_strName = strFileName;
		fseek(m_Fp, 0, SEEK_END);
		_IncreaseSize(ftell(m_Fp));
		fseek(m_Fp, 0, SEEK_SET);
		return true;
	}


	/**
	@brief 실질적인 데이터 읽기 처리를 하는 메소드. 각종 예외 처리를 담당한다.
	@param nSize 읽을 크기
	@param pData 읽을 데이터 버퍼
	@return 성공 여부
	*/
	bool _Read(size_t nSize, void* pData)
	{
		if(m_Fp == NULL)
		{
			LOG_INFO(_T("%s. Not Opened File. [%s]"), __TFUNCTION__, m_strName.c_str());
			return false;
		}

		if(m_nSize < nSize) //기억하고 있는 파일 크기보다, 읽으려는 크기가 더 크면 에러.
		{
			LOG_INFO(_T("%s. %d < %d [%s]"), __TFUNCTION__, nSize, m_nSize, m_strName.c_str());
			return false;
		}

		if(fread(pData, nSize, 1, m_Fp) != 1)
		{
			if(nSize <= m_nSize) //사이즈가 더 클때에는, 파일을 닫았다 다시 열면, 파일에 실제 쓰는 동작을 하기 때문에 fread로 읽을 수 있게 된다.
			{
				Close();
				Open(m_strName);
				return _Read(nSize, pData); //그래도 실패할 상황이라면? 크기 검사에서 걸릴것.
			}
			else //이 상황은, 정말 난감한 상황. 라이브러리 코드 자체에 대한 의구심이 필요한 상황.
			{
				LOG_ERROR(_T("%s Error [%d, %d]"), __TFUNCTION__, m_nSize, nSize); 
				Close();
				return false;
			}
		};
		_DecreaseSize(nSize);
		return true;
	}

	/**
	@brief 실질적인 데이터 쓰기 메소드. protected라 상속 받은 클래스가 아니면 사용 불가능하다.
	@param nSize 쓸 크기
	@parm pData 쓸 데이터 버퍼
	@return 성공 여부
	*/
	bool _Write(size_t nSize, void* pData)
	{
		if(m_Fp == NULL)
		{
			LOG_INFO(_T("%s. Not Opened File. [%s]"), __TFUNCTION__, m_strName.c_str());
			return false;
		}

		size_t nWriteSize = fwrite(pData, nSize, 1, m_Fp);
		if(nWriteSize != 1)
		{
			LOG_ERROR(_T("%s nWriteSize != 1 [%d, %d]"), __TFUNCTION__, nWriteSize, nSize);
			Close();
			return false;
		}
		_IncreaseSize(nSize);
		return true;
	}


	/**
	@brief 파일 이름을 알아오기
	@return 파일이름 tstring 참조자
	*/
	inline const tstring& GetName(){return m_strName;}

	/**
	@brief 파일의 IO중인 크기 증가
	@param nSize 변화한 크기
	*/
	void _IncreaseSize(size_t nSize){m_nSize += nSize;}

	/**
	@brief 파일의 IO중인 크기 감소
	@param nSize 변화한 크기
	*/
	void _DecreaseSize(size_t nSize){m_nSize -= nSize;}

private:
	///열려 있는 파일의 포인터
	FILE *m_Fp;

	///파일 길이
	size_t m_nSize;

	///파일 명
	tstring m_strName;
};

/**
@brief 읽기 전용 파일 스트림 클래스
*/
class CFileInStream : public CFileStream
{
public:
	/**
	@brief 빈 생성자. 
	*/
	CFileInStream()
	{
		
	}

	/**
	@brief 파일을 읽기만 가능한 모드로 오픈한다.
	@param strFileName 파일명
	@return 성공 여부. 
	*/
	virtual bool Open(const tstring& strFileName)
	{
		return _Open(strFileName, _T("wb"));
	}


	///파일에 쓰는 메소드
	virtual bool Write(size_t nSize, void* pData){return _Write(nSize, pData);}
};

/**
@brief 쓰기 전용 파일 스트림 클래스
*/
class CFileOutStream : public CFileStream
{
public:
	/**
	@brief 빈 생성자. 
	*/
	CFileOutStream()
	{

	}

	/**
	@brief 파일을 쓰기만 가능한 모드로 오픈한다.
	@param strFileName 파일명
	@return 성공 여부. 
	*/
	virtual bool Open(const tstring& strFileName)
	{
		return _Open(strFileName, _T("rb"));
	}


	///파일을 여는 메소드
	virtual bool Read(size_t nSize, void* pData){return _Read(nSize, pData);}
};

/**
@brief 읽고 쓰기 병행 파일 스트림 클래스
*/
class CFileIOStream : public CFileStream
{
public:
	/**
	@brief 빈 생성자.
	*/
	CFileIOStream()
	{
	}

	/**
	@brief 파일을 IO 둘다 가능한 모드로 오픈한다.
	@param strFileName 파일명
	@return 성공 여부. 
	*/
	virtual bool Open(const tstring& strFileName)
	{
		return _Open(strFileName, _T("ab+"));
	}

	///파일에 쓰는 메소드
	virtual bool Write(size_t nSize, void* pData){return _Write(nSize, pData);}

	///파일을 여는 메소드
	virtual bool Read(size_t nSize, void* pData){return _Read(nSize, pData);}
};

} //namespace Redmoon
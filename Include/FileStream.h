#pragma once

#include "Str.h"
#include "FileUtil.h"

namespace Redmoon
{

/**
@brief ���� ���� ����� ������ Ŭ����
*/
class CFileStream
{
public:
	/**
	@brief �Ҹ���. ���� ������ �ݴ´�.
	*/
	virtual ~CFileStream()
	{
		Close();
	}

	///������ �����ϴ� �޼ҵ�. ���� �����Լ��̹Ƿ� ���� �������־�� ��.
	virtual bool Open(const tstring& strFileName) = 0;

	/**
	@brief ������ �ݴ� �޼ҵ�
	@return ���� �ݱ� ���� ����
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
	@brief ������ ���� ������, ������ ���� �ʴ� �б� ���� �޼ҵ�.
	@param nSize ���� ũ��
	@parm pData �о�� ������ ����
	@return ���� ����
	*/
	virtual bool Read(size_t nSize, void* pData)
	{
		LOG_ERROR(_T("%s Can't use this function. You'll use this function overloaded class."), __TFUNCTION__);
		return false;	
	}

	/**
	@brief ������ ���� ������, ������ ���� �ʴ� ���� ���� �޼ҵ�.
	@param nSize �� ũ��
	@parm pData �� ������ ����
	@return ���� ����
	*/
	virtual bool Write(size_t nSize, void* pData)
	{
		LOG_ERROR(_T("%s Can't use this function. You'll use this function overloaded class."), __TFUNCTION__);
		return false;
	}

	/**
	@brief IO���� ũ�� ����
	@return IO���� ũ��
	@desc ���Ͽ��� ��ȿ�� �������� ũ�⸦ �ǹ��Ѵ�. �о�� ����, ���� �����Ѵ�.
	*/
	size_t Size(){return m_nSize;}

protected:
	/**
	@brief �� ������. ����� ���ؼ��� ������ �����ϵ���, protected�� ����.
	*/
	CFileStream() : m_Fp(NULL), m_nSize(0)
	{
		
	}

	/**
	@brief ������ ���� �޼ҵ�
	@param strFileName ���ϸ�
	@param szMode ���� ���� ��� (ex:rt,wt,rb...)
	@return ���� ����. 
	@desc �̹� ������� ������ �ִٸ�, ���� ���� ���ۿ� �����Ѵ�.
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
	@brief �������� ������ �б� ó���� �ϴ� �޼ҵ�. ���� ���� ó���� ����Ѵ�.
	@param nSize ���� ũ��
	@param pData ���� ������ ����
	@return ���� ����
	*/
	bool _Read(size_t nSize, void* pData)
	{
		if(m_Fp == NULL)
		{
			LOG_INFO(_T("%s. Not Opened File. [%s]"), __TFUNCTION__, m_strName.c_str());
			return false;
		}

		if(m_nSize < nSize) //����ϰ� �ִ� ���� ũ�⺸��, �������� ũ�Ⱑ �� ũ�� ����.
		{
			LOG_INFO(_T("%s. %d < %d [%s]"), __TFUNCTION__, nSize, m_nSize, m_strName.c_str());
			return false;
		}

		if(fread(pData, nSize, 1, m_Fp) != 1)
		{
			if(nSize <= m_nSize) //����� �� Ŭ������, ������ �ݾҴ� �ٽ� ����, ���Ͽ� ���� ���� ������ �ϱ� ������ fread�� ���� �� �ְ� �ȴ�.
			{
				Close();
				Open(m_strName);
				return _Read(nSize, pData); //�׷��� ������ ��Ȳ�̶��? ũ�� �˻翡�� �ɸ���.
			}
			else //�� ��Ȳ��, ���� ������ ��Ȳ. ���̺귯�� �ڵ� ��ü�� ���� �Ǳ����� �ʿ��� ��Ȳ.
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
	@brief �������� ������ ���� �޼ҵ�. protected�� ��� ���� Ŭ������ �ƴϸ� ��� �Ұ����ϴ�.
	@param nSize �� ũ��
	@parm pData �� ������ ����
	@return ���� ����
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
	@brief ���� �̸��� �˾ƿ���
	@return �����̸� tstring ������
	*/
	inline const tstring& GetName(){return m_strName;}

	/**
	@brief ������ IO���� ũ�� ����
	@param nSize ��ȭ�� ũ��
	*/
	void _IncreaseSize(size_t nSize){m_nSize += nSize;}

	/**
	@brief ������ IO���� ũ�� ����
	@param nSize ��ȭ�� ũ��
	*/
	void _DecreaseSize(size_t nSize){m_nSize -= nSize;}

private:
	///���� �ִ� ������ ������
	FILE *m_Fp;

	///���� ����
	size_t m_nSize;

	///���� ��
	tstring m_strName;
};

/**
@brief �б� ���� ���� ��Ʈ�� Ŭ����
*/
class CFileInStream : public CFileStream
{
public:
	/**
	@brief �� ������. 
	*/
	CFileInStream()
	{
		
	}

	/**
	@brief ������ �б⸸ ������ ���� �����Ѵ�.
	@param strFileName ���ϸ�
	@return ���� ����. 
	*/
	virtual bool Open(const tstring& strFileName)
	{
		return _Open(strFileName, _T("wb"));
	}


	///���Ͽ� ���� �޼ҵ�
	virtual bool Write(size_t nSize, void* pData){return _Write(nSize, pData);}
};

/**
@brief ���� ���� ���� ��Ʈ�� Ŭ����
*/
class CFileOutStream : public CFileStream
{
public:
	/**
	@brief �� ������. 
	*/
	CFileOutStream()
	{

	}

	/**
	@brief ������ ���⸸ ������ ���� �����Ѵ�.
	@param strFileName ���ϸ�
	@return ���� ����. 
	*/
	virtual bool Open(const tstring& strFileName)
	{
		return _Open(strFileName, _T("rb"));
	}


	///������ ���� �޼ҵ�
	virtual bool Read(size_t nSize, void* pData){return _Read(nSize, pData);}
};

/**
@brief �а� ���� ���� ���� ��Ʈ�� Ŭ����
*/
class CFileIOStream : public CFileStream
{
public:
	/**
	@brief �� ������.
	*/
	CFileIOStream()
	{
	}

	/**
	@brief ������ IO �Ѵ� ������ ���� �����Ѵ�.
	@param strFileName ���ϸ�
	@return ���� ����. 
	*/
	virtual bool Open(const tstring& strFileName)
	{
		return _Open(strFileName, _T("ab+"));
	}

	///���Ͽ� ���� �޼ҵ�
	virtual bool Write(size_t nSize, void* pData){return _Write(nSize, pData);}

	///������ ���� �޼ҵ�
	virtual bool Read(size_t nSize, void* pData){return _Read(nSize, pData);}
};

} //namespace Redmoon
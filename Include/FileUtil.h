#pragma once

#include "Common.h"
#include <list>

namespace Redmoon
{

/**
@brief ���� �������� ���ϸ��� ������ �޼ҵ�
@return ���� �������� ���ϸ�
*/
inline tstring GetMoudleName()
{
	TCHAR szBuffer[MAX_PATH];
	GetModuleFileName(NULL, szBuffer, MAX_PATH);
	tstring strFileName = szBuffer;
	tstring::size_type nSizeType = strFileName.find_last_of(_T('\\')) + 1;
	strFileName.erase(strFileName.begin(), strFileName.begin() + nSizeType);
	return strFileName;
}

/**
@brief ���� �������� ���Ͽ��� Ȯ���ڸ� ���� ������ �޼ҵ�
@return ���� �������� ���ϸ�. (Ȯ���� ����)
*/
inline tstring GetMoudleNameExclusiveExt()
{
	tstring strFileName = GetMoudleName();
	tstring::size_type nSizeType = strFileName.find_first_of(_T('.'));
	strFileName.at(nSizeType) = _T('\0');
	return strFileName;
}

/**
@brief ���� ���� ��ƿ Ŭ����
*/
class CFileUtil
{
public:	
	/**
	@brief ������ ���� �Ѵ�.
	@param strPath ������ ���ϸ�
	@return ���� ����
	*/
	static bool Delete(const tstring& strFileName)
	{
		return ::DeleteFile(strFileName.c_str()) ? true : false;
	}

	/**
	@brief ������ �ű��
	@param strSrcFileName ���� ���ϸ�
	@param strDestFileName ��� ���ϸ�
	*/
	static bool Move(const tstring& strSrcFileName, const tstring& strDestFileName)
	{
		return ::MoveFile(strSrcFileName.c_str(), strDestFileName.c_str()) ? true : false;
	}

	/**
	@brief ������ �����Ѵ�
	@param strSrcFileName ���� ���ϸ�
	@param strDestFileName ��� ���ϸ�
	*/
	static bool Copy(const tstring& strSrcFileName, const tstring& strDestFileName)
	{
		return ::CopyFile(strSrcFileName.c_str(), strDestFileName.c_str(), FALSE) ? true : false;
	}

	/**
	@brief ������ �ݴ´�.
	@param fp ���� ������ ���� ������
	@return ���� ����
	*/
	static bool Close(FILE*& fp)
	{
		if(fp == NULL)
			return false;

		fclose(fp);
		fp = NULL;
		return true;
	}

	/**
	@brief ���� ���� ���θ� ��ȯ�Ѵ�.
	@param strFileName ���ϸ�
	@return ���� ����
	*/
	static bool isExist(const tstring& strFileName) 
	{
		intptr_t hFile;
		_tfinddatai64_t c_file;
		bool bResult = false;
		if((hFile = _tfindfirsti64(strFileName.c_str(), &c_file)) != -1L)
		{
			if(!(c_file.attrib & _A_SUBDIR))  //���丮�� �ƴϸ� ������.
			{
				bResult = true; 
			}
		}

		_findclose(hFile);
		return bResult;
	}

	/**
	@brief ������ ����.
	@param strFileName ���ϸ�
	@param szMode ���� ���� ��� (ex:rt,wt,rb...)
	@return ���� ������ (FILE*)
	*/
	static FILE* Open(const tstring& strFileName, const TCHAR* szMode)
	{
		FILE *fp = _tfopen(strFileName.c_str(), szMode);
		if(fp == NULL)
		{
			LOG_INFO(_T("%s [fp == NULL]."),__TFUNCTION__);
		}
		return fp;
	}

	///�����̸��� ���� ���
	typedef std::list<tstring> STL_LIST_FILENAME;

	/**
	@brief Ư�� Ȯ������ ������ ã�Ƽ�, ����Ʈ�� ����ش�.
	@param strExtName Ȯ����
	@param stl_list_strFileName �ش� Ȯ������ ���� ���
	*/
	static void Search(const tstring& strExt, STL_LIST_FILENAME& stl_list_strFileName)
	{
		intptr_t hFile;
		_tfinddatai64_t c_file;
		if((hFile = _tfindfirsti64(strExt.c_str(), &c_file)) != -1L)
		{
			do //���� �ϳ��� ó��~!!
			{
				if(!(c_file.attrib & _A_SUBDIR))  //���丮�� �ƴϸ� ������.
				{
					stl_list_strFileName.push_back(CHAR_TO_TCHAR(c_file.name));
				}
			} while(_findnexti64(hFile, &c_file) == 0);
			_findclose(hFile);
		}
	}
};

} //namespace Redmoon
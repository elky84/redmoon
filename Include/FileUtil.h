#pragma once

#include "Common.h"
#include <list>

namespace Redmoon
{

/**
@brief 현재 실행중인 파일명을 얻어오는 메소드
@return 현재 실행중인 파일명
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
@brief 현재 실행중인 파일에서 확장자를 빼고 얻어오는 메소드
@return 현재 실행중인 파일명. (확장자 제외)
*/
inline tstring GetMoudleNameExclusiveExt()
{
	tstring strFileName = GetMoudleName();
	tstring::size_type nSizeType = strFileName.find_first_of(_T('.'));
	strFileName.at(nSizeType) = _T('\0');
	return strFileName;
}

/**
@brief 파일 관련 유틸 클래스
*/
class CFileUtil
{
public:	
	/**
	@brief 파일을 삭제 한다.
	@param strPath 삭제할 파일명
	@return 성공 여부
	*/
	static bool Delete(const tstring& strFileName)
	{
		return ::DeleteFile(strFileName.c_str()) ? true : false;
	}

	/**
	@brief 파일을 옮긴다
	@param strSrcFileName 원본 파일명
	@param strDestFileName 대상 파일명
	*/
	static bool Move(const tstring& strSrcFileName, const tstring& strDestFileName)
	{
		return ::MoveFile(strSrcFileName.c_str(), strDestFileName.c_str()) ? true : false;
	}

	/**
	@brief 파일을 복사한다
	@param strSrcFileName 원본 파일명
	@param strDestFileName 대상 파일명
	*/
	static bool Copy(const tstring& strSrcFileName, const tstring& strDestFileName)
	{
		return ::CopyFile(strSrcFileName.c_str(), strDestFileName.c_str(), FALSE) ? true : false;
	}

	/**
	@brief 파일을 닫는다.
	@param fp 닫을 파일의 이중 포인터
	@return 성공 여부
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
	@brief 파일 존재 여부를 반환한다.
	@param strFileName 파일명
	@return 존재 여부
	*/
	static bool isExist(const tstring& strFileName) 
	{
		intptr_t hFile;
		_tfinddatai64_t c_file;
		bool bResult = false;
		if((hFile = _tfindfirsti64(strFileName.c_str(), &c_file)) != -1L)
		{
			if(!(c_file.attrib & _A_SUBDIR))  //디렉토리가 아니면 파일임.
			{
				bResult = true; 
			}
		}

		_findclose(hFile);
		return bResult;
	}

	/**
	@brief 파일을 연다.
	@param strFileName 파일명
	@param szMode 파일 오픈 모드 (ex:rt,wt,rb...)
	@return 파일 포인터 (FILE*)
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

	///파일이름을 담은 목록
	typedef std::list<tstring> STL_LIST_FILENAME;

	/**
	@brief 특정 확장자의 파일을 찾아서, 리스트에 담아준다.
	@param strExtName 확장자
	@param stl_list_strFileName 해당 확장자의 파일 목록
	*/
	static void Search(const tstring& strExt, STL_LIST_FILENAME& stl_list_strFileName)
	{
		intptr_t hFile;
		_tfinddatai64_t c_file;
		if((hFile = _tfindfirsti64(strExt.c_str(), &c_file)) != -1L)
		{
			do //파일 하나씩 처리~!!
			{
				if(!(c_file.attrib & _A_SUBDIR))  //디렉토리가 아니면 파일임.
				{
					stl_list_strFileName.push_back(CHAR_TO_TCHAR(c_file.name));
				}
			} while(_findnexti64(hFile, &c_file) == 0);
			_findclose(hFile);
		}
	}
};

} //namespace Redmoon
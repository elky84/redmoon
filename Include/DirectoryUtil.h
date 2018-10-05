#pragma once

#include <io.h>
#include "Str.h"
#include <vector>
#include <ShellAPI.h>

namespace Redmoon
{

#define DIRECTORY_CALL_BACK(FunctionName) bool (*FunctionName)(const tstring&)

class CRestorePath;

/**
@brief 현재 실행중인 파일명을 얻어오는 메소드
@return 현재 실행중인 파일명
*/
inline tstring GetMoudlePath()
{
	TCHAR szBuffer[MAX_PATH];
	GetModuleFileName(NULL, szBuffer, MAX_PATH);
	tstring strFileName = szBuffer;
	tstring::size_type nSizeType = strFileName.find_last_of(_T('\\'));
	strFileName.erase(strFileName.begin() + nSizeType, strFileName.end());
	return strFileName;
}


/**
@brief 폴더를 다루는 기능을 모아둔 클래스.
*/
class CDirectoryUtil
{
private:
	/**
	@brief 전달받은 벡터를 기반으로, 해당 폴더를 생성한다.
	@param stl_vector_Folder \\로 구분된 폴더 이름이 기록되어있는 벡터
	*/
	static void _CreateDirectory(const std::vector<tstring> &stl_vector_Folder)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = FALSE;
		sa.lpSecurityDescriptor = NULL;

		///현재 디렉토리 저장
		tstring strCurrentDirectory = GetCurrentDirectory();

		///폴더를 만들때 사용하는 경로를 담은 문자열 생성
		tstring	strPath = stl_vector_Folder[0] + _T("\\");
		for( std::vector< tstring >::size_type s = 1; s < stl_vector_Folder.size(); ++s )
		{
			SetCurrentDirectory(strPath);
			::CreateDirectory(stl_vector_Folder[s].c_str() , &sa);
			strPath += stl_vector_Folder[s] + _T("\\");
		}

		///현재 디렉터리 복구
		SetCurrentDirectory(strCurrentDirectory);
	}

	/**
	@brief 디렉토리 단위로 쪼개기 설정.
	@param strPath 이 디렉토리를 쪼개서 벡터에 담아둔다.
	@param stl_vector_Folder 경로를 구성하는 이름이 \\단위로 쪼개서 담을 stl vector
	*/
	static void _DirectorySplit(const tstring& strPath, std::vector<tstring> &stl_vector_Folder)
	{
		tstring delimiters = _T("\\");
		tstring::size_type nLastPos = strPath.find_first_not_of(delimiters, 0);
		tstring::size_type nPos = strPath.find_first_of(delimiters, nLastPos);
		while(tstring::npos != nPos || tstring::npos != nLastPos)
		{
			stl_vector_Folder.push_back( strPath.substr(nLastPos, nPos - nLastPos) );
			nLastPos = strPath.find_first_not_of(delimiters, nPos);
			nPos = strPath.find_first_of(delimiters, nLastPos);
		}
	}
 
public:
	/**
	@brief 현재 폴더 알아오는 메소드.
	@return 현재 폴더
	*/
	static tstring GetCurrentDirectory()
	{
		TCHAR szCurrentDirectory[MAX_PATH];
		::GetCurrentDirectory(MAX_PATH, szCurrentDirectory);
		return szCurrentDirectory;
	}

	/**
	@brief 현재 폴더 설정하는 메소드.
	@param strDirectory 설정할 폴더
	*/
	static void SetCurrentDirectory(const tstring& strDirectory)
	{
		::SetCurrentDirectory(strDirectory.c_str());
	}



	/**
	@brief 전달받은 벡터를 기반으로, 해당 폴더를 생성한다.
	@param strPath 해당 경로에 폴더를 생성한다.
	*/
	static void CreateDirectory(const tstring& strPath)
	{
		if(isExistDirectory(strPath))
			return;

		std::vector<tstring> stl_vector_Folder;
		_DirectorySplit(strPath, stl_vector_Folder);
		_CreateDirectory(stl_vector_Folder);
	}

	/**
	@brief 폴더 존재 여부를 반환한다.
	@param strPath 폴더명
	@return 존재 여부
	*/
	static bool isExistDirectory(const tstring& strPath)
	{
		_tfinddatai64_t c_file;
		bool bResult = false;
		intptr_t hFile = _tfindfirsti64(const_cast<TCHAR*>(strPath.c_str()), &c_file);
		if (c_file.attrib & _A_SUBDIR ) //폴더라면
			bResult = true; //존재한다고 판단.

		_findclose(hFile);
		return bResult;
	}

	/**
	@brief 폴더를 삭제 한다.
	@param strPath 삭제할 폴더명
	@return 성공 여부
	*/
	static bool DeleteDirectory(const tstring& strPath)
	{
		////상대 경로일때 아닐때를 구분해서 확장하자.
		tstring strCurrentDirectory = GetCurrentDirectory();
		strCurrentDirectory += strPath.substr(strPath.find(_T("\\")));
		strCurrentDirectory.append(1, _T('\0'));

		SHFILEOPSTRUCT shFileOp = {0, }; 
	#ifdef _DEBUG
		shFileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI ;//화면에 메세지 표시 않함
	#else
		shFileOp.fFlags = FOF_NOCONFIRMATION; //묻지않고 삭제
	#endif
		shFileOp.wFunc = FO_DELETE;
		shFileOp.pFrom = strCurrentDirectory.c_str(); //shFileOp.pFrom 에 삭제할 폴더이름을 넣어줍니다. 여러개 폴더를 한번에 삭제할수도 있는데 널문자로 구분합니다. 마지막 폴더이름은 널문자2개로 끝나야 합니다.
		return SHFileOperation(&shFileOp) == S_OK ? true : false;
	}
};


/**
@brief 해당 클래스 생성시 디렉토리로, 변수 소멸시 복구 시키는 유틸 클래스.
*/
class CRestorePath
{
	///저장할 디렉토리
	tstring m_strCurrentDirectory;
public:
	/**
	@brief 생성자. 현재 디렉토리를 저장한다.
	*/
	CRestorePath()
	{
		m_strCurrentDirectory = CDirectoryUtil::GetCurrentDirectory();
	}

	/**
	@brief 소멸자. 저장되어있는 현재 디렉토리로 복구한다.
	*/
	~CRestorePath()
	{
		CDirectoryUtil::SetCurrentDirectory(m_strCurrentDirectory);
	}

	/**
	@brief 저장되어있는 현재 디렉토리를 반환한다
	@return 현재 디렉토리
	*/
	tstring& GetCurrentDirectory()
	{
		return m_strCurrentDirectory;
	}
};

} //namespace Redmoon
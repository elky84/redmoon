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
@brief ���� �������� ���ϸ��� ������ �޼ҵ�
@return ���� �������� ���ϸ�
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
@brief ������ �ٷ�� ����� ��Ƶ� Ŭ����.
*/
class CDirectoryUtil
{
private:
	/**
	@brief ���޹��� ���͸� �������, �ش� ������ �����Ѵ�.
	@param stl_vector_Folder \\�� ���е� ���� �̸��� ��ϵǾ��ִ� ����
	*/
	static void _CreateDirectory(const std::vector<tstring> &stl_vector_Folder)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = FALSE;
		sa.lpSecurityDescriptor = NULL;

		///���� ���丮 ����
		tstring strCurrentDirectory = GetCurrentDirectory();

		///������ ���鶧 ����ϴ� ��θ� ���� ���ڿ� ����
		tstring	strPath = stl_vector_Folder[0] + _T("\\");
		for( std::vector< tstring >::size_type s = 1; s < stl_vector_Folder.size(); ++s )
		{
			SetCurrentDirectory(strPath);
			::CreateDirectory(stl_vector_Folder[s].c_str() , &sa);
			strPath += stl_vector_Folder[s] + _T("\\");
		}

		///���� ���͸� ����
		SetCurrentDirectory(strCurrentDirectory);
	}

	/**
	@brief ���丮 ������ �ɰ��� ����.
	@param strPath �� ���丮�� �ɰ��� ���Ϳ� ��Ƶд�.
	@param stl_vector_Folder ��θ� �����ϴ� �̸��� \\������ �ɰ��� ���� stl vector
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
	@brief ���� ���� �˾ƿ��� �޼ҵ�.
	@return ���� ����
	*/
	static tstring GetCurrentDirectory()
	{
		TCHAR szCurrentDirectory[MAX_PATH];
		::GetCurrentDirectory(MAX_PATH, szCurrentDirectory);
		return szCurrentDirectory;
	}

	/**
	@brief ���� ���� �����ϴ� �޼ҵ�.
	@param strDirectory ������ ����
	*/
	static void SetCurrentDirectory(const tstring& strDirectory)
	{
		::SetCurrentDirectory(strDirectory.c_str());
	}



	/**
	@brief ���޹��� ���͸� �������, �ش� ������ �����Ѵ�.
	@param strPath �ش� ��ο� ������ �����Ѵ�.
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
	@brief ���� ���� ���θ� ��ȯ�Ѵ�.
	@param strPath ������
	@return ���� ����
	*/
	static bool isExistDirectory(const tstring& strPath)
	{
		_tfinddatai64_t c_file;
		bool bResult = false;
		intptr_t hFile = _tfindfirsti64(const_cast<TCHAR*>(strPath.c_str()), &c_file);
		if (c_file.attrib & _A_SUBDIR ) //�������
			bResult = true; //�����Ѵٰ� �Ǵ�.

		_findclose(hFile);
		return bResult;
	}

	/**
	@brief ������ ���� �Ѵ�.
	@param strPath ������ ������
	@return ���� ����
	*/
	static bool DeleteDirectory(const tstring& strPath)
	{
		////��� ����϶� �ƴҶ��� �����ؼ� Ȯ������.
		tstring strCurrentDirectory = GetCurrentDirectory();
		strCurrentDirectory += strPath.substr(strPath.find(_T("\\")));
		strCurrentDirectory.append(1, _T('\0'));

		SHFILEOPSTRUCT shFileOp = {0, }; 
	#ifdef _DEBUG
		shFileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI ;//ȭ�鿡 �޼��� ǥ�� ����
	#else
		shFileOp.fFlags = FOF_NOCONFIRMATION; //�����ʰ� ����
	#endif
		shFileOp.wFunc = FO_DELETE;
		shFileOp.pFrom = strCurrentDirectory.c_str(); //shFileOp.pFrom �� ������ �����̸��� �־��ݴϴ�. ������ ������ �ѹ��� �����Ҽ��� �ִµ� �ι��ڷ� �����մϴ�. ������ �����̸��� �ι���2���� ������ �մϴ�.
		return SHFileOperation(&shFileOp) == S_OK ? true : false;
	}
};


/**
@brief �ش� Ŭ���� ������ ���丮��, ���� �Ҹ�� ���� ��Ű�� ��ƿ Ŭ����.
*/
class CRestorePath
{
	///������ ���丮
	tstring m_strCurrentDirectory;
public:
	/**
	@brief ������. ���� ���丮�� �����Ѵ�.
	*/
	CRestorePath()
	{
		m_strCurrentDirectory = CDirectoryUtil::GetCurrentDirectory();
	}

	/**
	@brief �Ҹ���. ����Ǿ��ִ� ���� ���丮�� �����Ѵ�.
	*/
	~CRestorePath()
	{
		CDirectoryUtil::SetCurrentDirectory(m_strCurrentDirectory);
	}

	/**
	@brief ����Ǿ��ִ� ���� ���丮�� ��ȯ�Ѵ�
	@return ���� ���丮
	*/
	tstring& GetCurrentDirectory()
	{
		return m_strCurrentDirectory;
	}
};

} //namespace Redmoon
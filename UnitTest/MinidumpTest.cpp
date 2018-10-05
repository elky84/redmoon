#include "TestCommon.h"
#include "MiniDump.h"
#include "FileUtil.h"
#include "DirectoryUtil.h"

namespace Redmoon
{

	class CParent
	{
	public:
		virtual void Test()
		{
			LOG_INFO(_T("%s"), __TFUNCTION__);
		}
	};

	class CChild : public CParent
	{
	public:
		virtual void Test()
		{
			LOG_INFO(_T("%s"), __TFUNCTION__);
		}
	};

	class CClearDumpFile
	{
	public:
		CClearDumpFile()
		{

		}

		~CClearDumpFile()
		{
			CRestorePath restorepath;
			CDirectoryUtil::SetCurrentDirectory(GetMoudlePath());
			CFileUtil::Search(_T("*.dmp"), m_stl_list_strFileName);
			for(CFileUtil::STL_LIST_FILENAME::iterator it = m_stl_list_strFileName.begin(); it != m_stl_list_strFileName.end(); ++it)
			{
				CFileUtil::Delete(*it);
			}
		}
	private:
		CFileUtil::STL_LIST_FILENAME m_stl_list_strFileName;
	};

	SUITE(MINIDUMP_TEST_SUITE)
	{
		CClearDumpFile clear;
		TEST(MINIDUMP_TEST)
		{
			__try
			{
				CChild* pcChild = NULL;
				pcChild->Test();
			}
			__except(Redmoon::CMiniDumper::Handler(GetExceptionInformation()))
			{
				
			}
		}
	}
} //namespace Redmoon
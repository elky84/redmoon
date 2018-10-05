#include "TestCommon.h"

#include "FileStream.h"
#include "DirectoryUtil.h"

namespace Redmoon
{

	struct SFileTest
	{
		SFileTest()
		{
			CDirectoryUtil::CreateDirectory(_T(".\\CFG"));	
		}

		~SFileTest()
		{
			CDirectoryUtil::DeleteDirectory(_T(".\\CFG"));
		}
	};

	SUITE(FILE_TEST_SUITE)
	{
		SFileTest sFileTest;
		TEST(FILE_IN_TEST)
		{
			CFileInStream instream;
			CHECK(instream.Open(_T(".\\CFG\\test.cfg")));

			char buffer[2000] = "";
			char ch = 'A';
			for(int i = 0; i < _countof(buffer); i++)
			{
				buffer[i] = ch++;
				if('Z' == ch)
				{
					ch = '\n';
				}
				else if(ch == '\n')
				{
					ch = 'A';
				}
			}

			CHECK(instream.Write(_countof(buffer), &buffer));
			CHECK(instream.Size());
			instream.Close();
		}

		TEST(FILE_OUT_TEST)
		{
			CFileOutStream outstream;
			CHECK(outstream.Open(_T(".\\CFG\\test.cfg")));
			CHECK(outstream.Size()); //읽은 파일 크기
			outstream.Close();
		}

		TEST(FILE_IO_TEST)
		{
			char buffer[4000] = "";

			CFileIOStream iostream;
			iostream.Open(_T(".\\CFG\\test.cfg"));
			iostream.Write(_countof(buffer), &buffer);
			size_t nIOSize = iostream.Size();
			CHECK(nIOSize);

			CHECK(iostream.Read(_countof(buffer), &buffer)); //실패한 테스트

			CHECK(iostream.Size());
			iostream.Close();
		}

	}
}

#include "UnitTest++.h"
#include "TestReporter.h"

class VUTPP_Reporter : public UnitTest::TestReporter
{
private:
	const HANDLE& m_hRefWritePipe;

public:
	VUTPP_Reporter(const HANDLE &hRefWritePipe) : m_hRefWritePipe(hRefWritePipe){}
	~VUTPP_Reporter() {}

	void ReportTestStart(UnitTest::TestDetails const& test) {}
	void ReportFailure(UnitTest::TestDetails const& test, char const* failure)
	{
		char writeBuffer[1024];
		sprintf( writeBuffer, "%d,%s,%s", test.lineNumber, test.filename, failure );
		DWORD dwWrite;
		if( WriteFile( m_hRefWritePipe, writeBuffer, 1024, &dwWrite, NULL ) == false || dwWrite != 1024 )
			exit(-1);
	}
	void ReportTestFinish(UnitTest::TestDetails const& test, float secondsElapsed) {}
	void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed) {}
};

class VUTPP_Runner
{
public:
	VUTPP_Runner(const char* strParam) : m_strVutppParam(strParam)
	{
		Run();
	}

	~VUTPP_Runner()
	{

	}

	bool Run()
	{
		const size_t seperator = m_strVutppParam.find( ',' );
		if( seperator == std::string::npos )
			return false;

		HANDLE readPipe, writePipe;
		sscanf( m_strVutppParam.substr( 0, seperator ).c_str(), "%d", &readPipe );
		sscanf( m_strVutppParam.substr( seperator+1 ).c_str(), "%d", &writePipe );

		char readBuffer[1024], writeBuffer[1024];

		DWORD dwSize = 0;
		strcpy( writeBuffer, "connect" );
		if( WriteFile( writePipe, writeBuffer, 1024, &dwSize, NULL ) == false || dwSize != 1024 )
			return false;

		UnitTest::TestList& rTestList = UnitTest::Test::GetTestList();

		while( true )
		{
			if( ReadFile( readPipe, readBuffer, 1024, &dwSize, NULL ) == false || dwSize != 1024 )
				return false;

			if( strncmp( readBuffer, "__VUTPP_FINISH__", 16 ) == 0 )
				break;

			const char* pSeperator = strchr( readBuffer, ',' );
			std::string suiteName( readBuffer, pSeperator - readBuffer ), testName( pSeperator+1 );

			bool bRun = false;
			UnitTest::Test* pTest = rTestList.GetHead();
			while( pTest != NULL )
			{
				if( pTest->m_details.testName == testName && pTest->m_details.suiteName == suiteName )
				{
					VUTPP_Reporter reporter( writePipe );
					UnitTest::TestResults testResult( &reporter );
					UnitTest::CurrentTest::Results() = &testResult;
					pTest->Run();
					strcpy( writeBuffer, "-1," );
					bRun = true;
					if( WriteFile( writePipe, writeBuffer, 1024, &dwSize, NULL ) == false || dwSize != 1024 )
						return false;

					break;
				}

				pTest = pTest->next;
			}

			if( bRun == false )
			{
				sprintf( writeBuffer, "%d,,%s", -2, "can't find test" );
				if( WriteFile( writePipe, writeBuffer, 1024, &dwSize, NULL ) == false || dwSize != 1024 )
					return false;
			}
		}
		return true;
	}

private:
	std::string m_strVutppParam;
};
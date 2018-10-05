#pragma once

class CDBSPList
{
public:
	TCHAR name[128];

	DEFINE_COMMAND_EX(CDBSPList, LPOLESTR(L"select convert(varchar(256), name) from sys.procedures order by name"))
	BEGIN_COLUMN_MAP(CDBSPList)
		COLUMN_ENTRY(1, name) //
	END_COLUMN_MAP()
};
typedef CCommand< CAccessor<CDBSPList>, CRowset, CNoMultipleResults> QUERY_SPList;
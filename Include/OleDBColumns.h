#pragma once

namespace Redmoon
{

class COleDBColumns
{
public:
	CHAR tableName[256];

	CHAR name[128];
	BYTE system_type_id;
	SHORT max_length;
	SHORT precision;
	SHORT scale;
	VARIANT_BOOL is_identity;

	DEFINE_COMMAND_EX(COleDBColumns, LPOLESTR(L"select convert(varchar(128), name) name ,system_type_id, max_length, precision, scale, is_identity from sys.columns where object_id=object_id(?)"))
		BEGIN_PARAM_MAP(COleDBColumns)
			SET_PARAM_TYPE(DBPARAMIO_INPUT)		
			COLUMN_ENTRY(1, tableName)
		END_PARAM_MAP()
		BEGIN_COLUMN_MAP(COleDBColumns)
			COLUMN_ENTRY(1, name) 
			COLUMN_ENTRY(2, system_type_id) 
			COLUMN_ENTRY(3, max_length) 
			COLUMN_ENTRY(4, precision) 
			COLUMN_ENTRY(5, scale) 
			COLUMN_ENTRY(6, is_identity) 
		END_COLUMN_MAP()
};
typedef CCommand< CAccessor<COleDBColumns>, CRowset, CNoMultipleResults> QUERY_Columns;

} //namespace Redmoon
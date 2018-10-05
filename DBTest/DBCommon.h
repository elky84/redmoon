#pragma once

struct SDBInfo{
	tstring m_strNo;
	tstring m_strIP;		
	tstring m_strDBName;	
	tstring m_strLogin;	
	tstring m_strPWD;		
	tstring m_strPort;		
	tstring m_strSPNamePrefix;		
	tstring m_strSPNameSuffixSelect;		
	tstring m_strSPNameSuffixUpdate;		
	tstring m_strSPNameSuffixInsert;		
	tstring m_strSPNameSuffixDelete;		

	tstring m_strPathCmdClassTable;		
	tstring m_strPathCmdClassProcedure;		
	tstring m_strPathSPSelect;		
	tstring m_strPathSPUpdate;		
	tstring m_strPathSPInsert;		
	tstring m_strPathSPDelete;		
	
	SDBInfo(){}
};
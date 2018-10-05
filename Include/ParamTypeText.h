#pragma once

#include "Singleton.h"
#include "DBCommon.h"

namespace Redmoon
{

/**
@brief 파라미터 타입을 문자열로 바꿔주는 유틸 클래스
*/
class CParamTypeText
{
private:
	///key : 파라미터 타입, value : 대응 문자열
	std::map<INT, TCHAR *>	m_stl_map_ParamTypeText;

private:
	/**
	@brief 생성자. 파라미터 타입에 따른 문자열을 입력해놓는다.
	*/
	CParamTypeText()
	{
		InsertText(DBPARAMTYPE_INPUT,_T("INPUT"));
		InsertText(DBPARAMTYPE_INPUTOUTPUT,_T("INPUTOUTPUT"));
		InsertText(DBPARAMTYPE_OUTPUT,_T("OUTPUT"));
		InsertText(DBPARAMTYPE_RETURNVALUE,_T("RETURNVALUE"));
	}

	/**
	@brief 파라미터 타입에 대응하는 문자열을 맵에 저장하는 메소드
	@param nParamType 파라미터 타입
	@param szStr 대응하는 문자열
	*/
	void InsertText(int nParamType, TCHAR* szStr)
	{
		m_stl_map_ParamTypeText[nParamType] = szStr;
	}

public:
	///싱글턴으로 생성
	STATIC_SINGLETON_PTR(CParamTypeText);

	/**
	@brief 파라미터 타입에대응하는 문자열을 얻어오는 메소드
	@param nParamType 파라미터 타입
	@return 대응하는 문자열 (못찾으면 "None"을 리턴)
	*/
	TCHAR * GetTypeName(int nParamType)
	{
		std::map<INT,TCHAR*>::iterator itr = m_stl_map_ParamTypeText.find(nParamType);
		if(itr != m_stl_map_ParamTypeText.end())
		{
			return itr->second;
		}
		else
		{
			return _T("None");
		}
	}
};

/**
@brief 파라미터 타입을 문자열로 얻어오게 해주는 static 메소드
@param nParamType 파라미터 타입
@return 대응하는 문자열
*/
inline TCHAR* ParamTypeToString(int nParamType)	
{
	return CParamTypeText::InstancePtr()->GetTypeName(nParamType);
}

/**
@brief ParamType에 따라서, 뒤에 붙일 문자열을 결정한다.
@param szStr 파라미터 타입 문자열
@return 뒤에 붙일 문자열
*/
inline 	TCHAR* ParamTypePerAddonString(TCHAR* szStr)
{
	if(_tcsicmp(szStr, _T("INPUTOUTPUT")) == 0 || _tcsicmp(szStr, _T("OUTPUT")) == 0)
	{
		return _T(" OUT");
	}
	return _T("");
}

/**
@brief ParamType에 따라서, ParamIO 문자열을 결정한다.
@param szStr 파라미터 타입 문자열
@return 코드 생성기에 쓰일 ParamIO 문자열
*/
inline TCHAR* ParamTypeToParamIOString(TCHAR * szStr)
{
	if(_tcsicmp(szStr, _T("INPUTOUTPUT")) == 0 || _tcsicmp(szStr, _T("OUTPUT")) == 0 || _tcsicmp(szStr, _T("RETURNVALUE")) == 0)
	{
		return _T("DBPARAMIO_OUTPUT");
	}
	else if(_tcsicmp(szStr, _T("INPUT")) == 0)
	{
		return _T("DBPARAMIO_INPUT");
	}
	return _T("");
}

} //namespace Redmoon
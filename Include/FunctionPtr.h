#pragma once

#include "Macro.h"
#include <map>

namespace Redmoon
{

/**
@brief 파라미터 한개짜리 함수 포인터 클래스
*/
template <typename T>
class CFunctionPtrNoParam
{
public:
	///함수 포인터 typedef형
	typedef void (T::*PFTYPE)();

private:
	///key: command, value : 함수 포인터
	typedef std::map<int, PFTYPE> STL_MAP_FuncPtr;

	///함수 포인터 맵
	STL_MAP_FuncPtr m_stl_map_FuncPtr;

protected:
	/**
	@brief 생성자. 상속으로만 사용가능하게 하기 위해서 protected이다.
	*/
	CFunctionPtrNoParam()
	{
	}

	/**
	@brief 함수 포인터 등록
	@param nCommand 커맨드
	@param pf 함수 포인터
	*/
	inline void Register(int nCommand, PFTYPE pf)
	{
		m_stl_map_FuncPtr[nCommand] = pf;
	}

	/**
	@brief 등록한 함수 포인터 얻어오기. 
	@param nCommand 커맨드
	@return 해당하는 커맨드의 함수 포인터. 없으면 NULL 반환.
	*/
	inline PFTYPE Get(int nCommand)
	{
		return m_stl_map_FuncPtr[nCommand];
	}

	/**
	@brief 함 포인터 호출
	@param pData 파라미터 넘길 데이터
	@param pf 호출하는 데에 이용할 함수 포인터.
	*/
	inline void Call(PFTYPE pf) 
	{
		CALL_MEMBER_FN(*static_cast<T*>(this), pf)();
	}

public:

	/**
	@brief 함수 호출을 편리하게 연결 시킨 메소드
	@param pData 파라미터 넘길 데이터
	@param nCommand 커맨드
	@return 함소 호출 성공 여부
	*/
	inline bool OnProcess(int nCommand)
	{
		PFTYPE pfType = Get(nCommand);
		if(pfType)
		{
			Call(pfType);
			return true;
		}
		return false;
	}
};

/**
@brief 파라미터 한개짜리 함수 포인터 클래스
*/
template <typename T, typename TData>
class CFunctionPtr
{
public:
	///함수 포인터 typedef형
	typedef void (T::*PFTYPE)(TData*);

private:
	///key: command, value : 함수 포인터
	typedef std::map<int, PFTYPE> STL_MAP_FuncPtr;

	///함수 포인터 맵
	STL_MAP_FuncPtr m_stl_map_FuncPtr;

protected:
	/**
	@brief 생성자. 상속으로만 사용가능하게 하기 위해서 protected이다.
	*/
	CFunctionPtr()
	{
	}

	/**
	@brief 함수 포인터 등록
	@param nCommand 커맨드
	@param pf 함수 포인터
	*/
	inline void Register(int nCommand, PFTYPE pf)
	{
		m_stl_map_FuncPtr[nCommand] = pf;
	}

	/**
	@brief 등록한 함수 포인터 얻어오기. 
	@param nCommand 커맨드
	@return 해당하는 커맨드의 함수 포인터. 없으면 NULL 반환.
	*/
	inline PFTYPE Get(int nCommand)
	{
		return m_stl_map_FuncPtr[nCommand];
	}

	/**
	@brief 함 포인터 호출
	@param pData 파라미터 넘길 데이터
	@param pf 호출하는 데에 이용할 함수 포인터.
	*/
	inline void Call(TData* pData, PFTYPE pf) 
	{
		CALL_MEMBER_FN(*static_cast<T*>(this), pf)(pData);
	}

public:

	/**
	@brief 함수 호출을 편리하게 연결 시킨 메소드
	@param pData 파라미터 넘길 데이터
	@param nCommand 커맨드
	@return 함소 호출 성공 여부
	*/
	inline bool OnProcess(TData* pData, int nCommand)
	{
		PFTYPE pfType = Get(nCommand);
		if(pfType)
		{
			Call(pData, pfType);
			return true;
		}
		return false;
	}
};

class CCallBack
{
public:
	virtual bool Interpret(void* pArgument) 
	{
		return false;
	};
};

template <class objType, typename funType>
class CScript : public CCallBack
{
public:
	CScript(objType obj, funType fun)
		: m_obj(obj)
		, m_fun(fun)
	{

	}

	~CScript() 
	{
	}

	bool Interpret(void* pArgument)
	{
		return (m_obj->*m_fun)(pArgument);
	}

private:
	objType m_obj;
	funType m_fun;
};

template <class objType, typename funType>
CScript<objType, funType>* SetProc(objType obj, funType fun)
{
	return new CScript<objType, funType>(obj, fun);
}

} //namespace Redmoon
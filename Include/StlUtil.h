#pragma once

#include <map>
#include <set>
#include <vector>

namespace Redmoon
{

/**
@breif Set 복사 메소드
@param dest 기존 사용중인 객체
@param src 새로 복사할 대상 객체
*/
template <typename T>
inline void Copy(std::set<T>& dest, std::set<T>& src)
{
	dest.clear();
	dest = src;
	src.clear();
}


/**
@breif Vector 복사 메소드
@param dest 기존 사용중인 객체
@param src 새로 복사할 대상 객체
*/
template <typename T>
inline void Copy(std::vector<T>& dest, std::vector<T>& src)
{
	dest.clear();
	dest = src;
	src.clear();
}


/**
@breif Multimap 복사 메소드.
@param dest 기존 사용중인 객체
@param src 새로 복사할 대상 객체
*/
template <typename T1,typename T2>
inline void Copy(std::multimap<T1,T2>&dest, std::multimap<T1,T2>&src)
{
	dest.clear();
	dest = src;
	src.clear();
}

/**
@breif Map 복사 메소드.
@param dest 기존 사용중인 객체
@param src 새로 복사할 대상 객체
*/
template <typename T1,typename T2>
inline void Copy(std::map<T1,T2>&dest, std::map<T1,T2>&src)
{
	dest.clear();
	dest = src;
	src.clear();
}

/**
@breif Map 복사 메소드.
@param dest 기존 사용중인 객체.
@param src 새로 복사할 대상 객체
*/
template <typename T1,typename T2>
inline void CopyPtr(std::map<T1,T2>&dest, std::map<T1,T2>&src)
{
	std::map<T1,T2>::iterator itor ;
	while(!dest.empty())
	{
		itor = dest.begin();
		T2 t = (*itor).second ;
		dest.erase(itor++) ;
		delete t;
	}	

	dest.insert(src.begin(), src.end());
	src.clear();
}

/**
@brief 멀티맵에 담긴 클래스의 멤버 함수 포인터를 넘겨, 호출하도록 하는 메소드.
@param stl_multimap 멀티맵 포인터
@param key 찾을려는 키
@param pFunc 호출할 함수 포인터
*/
template <typename KEY, typename CLASS>
inline void Execute(std::multimap<KEY, CLASS*>& stl_multimap, KEY key, void (CLASS::*pFunc)())
{
	std::multimap<KEY, CLASS*>::iterator it = stl_multimap.find(key);
	while(it != stl_multimap.end())
	{
		if(it->first != key)
			break;

		CLASS* pcObject = it->second;
		(pcObject->*pFunc)();
		++it;
	}
}

/**
@brief 멀티맵에서 특정 키의 멤버를 삭제해주는 메소드. 해당 값을 가진 모든 멤버를 제거한다.
@param stl_multimap 멀티맵 포인터
@param key 찾을려는 키
*/
template <typename KEY, typename CLASS>
inline void Delete(std::multimap<KEY, CLASS*>& stl_multimap, KEY key)
{
	std::multimap<KEY, CLASS*>::iterator it = stl_multimap.find(key);
	while(it != stl_multimap.end())
	{
		if(it->first != key)
			break;

		delete it->second;
		it = stl_multimap.erase(it);
	}
}

} //namespace Redmoon
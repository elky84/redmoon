#pragma once


namespace Redmoon
{

/**
@brief Iocp에 사용되는 싱글턴들의 릴리즈 순서를 맞추기 위해 사용되는 클래스.
*/
class CIocpObject
{
public:
	///생성자. 생성 순서에 맞춰 new.
	CIocpObject();

	///소멸자. 소멸 순서에 맞춰 delete.
	~CIocpObject();
};

} //namespace Redmoon
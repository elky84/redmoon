#pragma once

#include "Common.h"

namespace Redmoon
{

using std::cout;
using std::endl;

namespace con
{
    static const WORD bgMask(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY);
    static const WORD fgMask(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
    
    static const WORD fgBlack    ( 0 ); 
    static const WORD fgLoRed    ( FOREGROUND_RED   ); 
    static const WORD fgLoGreen  ( FOREGROUND_GREEN ); 
    static const WORD fgLoBlue   ( FOREGROUND_BLUE  ); 
    static const WORD fgLoCyan   ( fgLoGreen   | fgLoBlue ); 
    static const WORD fgLoMagenta( fgLoRed     | fgLoBlue ); 
    static const WORD fgLoYellow ( fgLoRed     | fgLoGreen ); 
    static const WORD fgLoWhite  ( fgLoRed     | fgLoGreen | fgLoBlue ); 
    static const WORD fgGray     ( fgBlack     | FOREGROUND_INTENSITY ); 
    static const WORD fgHiWhite  ( fgLoWhite   | FOREGROUND_INTENSITY ); 
    static const WORD fgHiBlue   ( fgLoBlue    | FOREGROUND_INTENSITY ); 
    static const WORD fgHiGreen  ( fgLoGreen   | FOREGROUND_INTENSITY ); 
    static const WORD fgHiRed    ( fgLoRed     | FOREGROUND_INTENSITY ); 
    static const WORD fgHiCyan   ( fgLoCyan    | FOREGROUND_INTENSITY ); 
    static const WORD fgHiMagenta( fgLoMagenta | FOREGROUND_INTENSITY ); 
    static const WORD fgHiYellow ( fgLoYellow  | FOREGROUND_INTENSITY );
    static const WORD bgBlack    ( 0 ); 
    static const WORD bgLoRed    ( BACKGROUND_RED   ); 
    static const WORD bgLoGreen  ( BACKGROUND_GREEN ); 
    static const WORD bgLoBlue   ( BACKGROUND_BLUE  ); 
    static const WORD bgLoCyan   ( bgLoGreen   | bgLoBlue ); 
    static const WORD bgLoMagenta( bgLoRed     | bgLoBlue ); 
    static const WORD bgLoYellow ( bgLoRed     | bgLoGreen ); 
    static const WORD bgLoWhite  ( bgLoRed     | bgLoGreen | bgLoBlue ); 
    static const WORD bgGray     ( bgBlack     | BACKGROUND_INTENSITY ); 
    static const WORD bgHiWhite  ( bgLoWhite   | BACKGROUND_INTENSITY ); 
    static const WORD bgHiBlue   ( bgLoBlue    | BACKGROUND_INTENSITY ); 
    static const WORD bgHiGreen  ( bgLoGreen   | BACKGROUND_INTENSITY ); 
    static const WORD bgHiRed    ( bgLoRed     | BACKGROUND_INTENSITY ); 
    static const WORD bgHiCyan   ( bgLoCyan    | BACKGROUND_INTENSITY ); 
    static const WORD bgHiMagenta( bgLoMagenta | BACKGROUND_INTENSITY ); 
    static const WORD bgHiYellow ( bgLoYellow  | BACKGROUND_INTENSITY );
    
	/**
	@brief 콘솔 모드에 대한 정보를 다루기 쉽게 지정한 클래스.
	*/
	class con_dev
	{
	private:
		///콘솔 핸들
		HANDLE                      hCon;

		///기록한 크기
		DWORD                       dwWritten; 

		///콘솔창 버퍼 정보
		CONSOLE_SCREEN_BUFFER_INFO  csbi; 

		///콘솔창 크기
		DWORD                       dwConSize;

	public:
		/**
		@brief 생성자. 기본 입출력 핸들 정보를 얻어온다.
		*/
		con_dev() 
		{ 
			reset();
		}

		/**
		@brief 초기화 함수. 기보노 입출력 핸들 정보를 얻어온다.
		*/
		void reset() 
		{ 
			hCon = ::GetStdHandle( STD_OUTPUT_HANDLE );
		}
	private:
		/**
		@brief 콘솔창 정보를 읽어, csbi에 기록하는 메소드.
		*/
		void _GetInfo()
		{
			if( NULL == hCon )
				reset();

			::GetConsoleScreenBufferInfo( hCon, &csbi );
			dwConSize = csbi.dwSize.X * csbi.dwSize.Y; 
		}

	public:
		/**
		@brief 화면을 초기화 하는 메소드.
		*/
		void Clear()
		{
			COORD coordScreen = { 0, 0 };
			_GetInfo(); 

			::FillConsoleOutputCharacter( hCon, TEXT(' '), dwConSize, coordScreen, &dwWritten ); 

			_GetInfo(); 

			::FillConsoleOutputAttribute( hCon, csbi.wAttributes, dwConSize, coordScreen, &dwWritten); 

			::SetConsoleCursorPosition( hCon, coordScreen ); 
		}

		/**
		@brief 화면에 색 지정하는 메소드
		*/
		void SetColor( WORD wRGBI, WORD Mask )
		{
			_GetInfo();
			csbi.wAttributes &= Mask; 
			csbi.wAttributes |= wRGBI; 
			::SetConsoleTextAttribute( hCon, csbi.wAttributes );
		}
	};

	///콘솔 환경 오브젝트 스태틱으로 생성
	static con_dev console;

	/**
	@brief 화면 지우는 메소드
	@param os 지울 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& clr( std::ostream& os )
	{
		os.flush();
		console.Clear();
		return os;
	};

	/**
	@brief 글씨를 빨간색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& fg_red( std::ostream& os )
	{
		os.flush();
		console.SetColor( fgHiRed, bgMask );

		return os;
	}

	/**
	@brief 글씨를 초록색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& fg_green( std::ostream& os )
	{
		os.flush();
		console.SetColor( fgHiGreen, bgMask );

		return os;
	}

	/**
	@brief 글씨를 파란색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& fg_blue( std::ostream& os )
	{
		os.flush();
		console.SetColor( fgHiBlue, bgMask );

		return os;
	}

	/**
	@brief 글씨를 하얀색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& fg_white( std::ostream& os )
	{
		os.flush();
		console.SetColor( fgHiWhite, bgMask );

		return os;
	}

	/**
	@brief 글씨를 청록색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& fg_cyan( std::ostream& os )
	{
		os.flush();
		console.SetColor( fgHiCyan, bgMask );

		return os;
	}

	/**
	@brief 글씨를 자홍색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& fg_magenta( std::ostream& os )
	{
		os.flush();
		console.SetColor( fgHiMagenta, bgMask );

		return os;
	}

	/**
	@brief 글씨를 노란색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& fg_yellow( std::ostream& os )
	{
		os.flush();
		console.SetColor( fgHiYellow, bgMask );

		return os;
	}

	/**
	@brief 글씨를 검은색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& fg_black( std::ostream& os )
	{
		os.flush();
		console.SetColor( fgBlack, bgMask );

		return os;
	}

	/**
	@brief 글씨를 회색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& fg_gray( std::ostream& os )
	{
		os.flush();
		console.SetColor( fgGray, bgMask );

		return os;
	}

	/**
	@brief 글씨를 빨간색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& bg_red( std::ostream& os )
	{
		os.flush();
		console.SetColor( bgHiRed, fgMask );

		return os;
	}

	/**
	@brief 배경을 초록색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& bg_green( std::ostream& os )
	{
		os.flush();
		console.SetColor( bgHiGreen, fgMask );

		return os;
	}

	/**
	@brief 배경을 파란색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& bg_blue( std::ostream& os )
	{
		os.flush();
		console.SetColor( bgHiBlue, fgMask );

		return os;
	}

	/**
	@brief 배경을 흰색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& bg_white( std::ostream& os )
	{
		os.flush();
		console.SetColor( bgHiWhite, fgMask );

		return os;
	}

	/**
	@brief 배경을 청록색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& bg_cyan( std::ostream& os )
	{
		os.flush();
		console.SetColor( bgHiCyan, fgMask );

		return os;
	}

	/**
	@brief 배경을 자홍색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& bg_magenta( std::ostream& os )
	{
		os.flush();
		console.SetColor( bgHiMagenta, fgMask );

		return os;
	}

	/**
	@brief 배경을 노란색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& bg_yellow( std::ostream& os )
	{
		os.flush();
		console.SetColor( bgHiYellow, fgMask );

		return os;
	}

	/**
	@brief 배경을 검은색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& bg_black( std::ostream& os )
	{
		os.flush();
		console.SetColor( bgBlack, fgMask );

		return os;
	}

	/**
	@brief 배경을 회색으로 지정
	@param os 지정할 ostream
	@return 사용한 ostream
	*/
	inline std::ostream& bg_gray( std::ostream& os )
	{
		os.flush();
		console.SetColor( bgGray, fgMask );

		return os;
	}
}

/* example
int main()
{
using std::cout;
using std::endl;
using std::setiosflags;
 
cout << con::clr;
cout << con::bg_blue << con::fg_white << setw( 40 )
<< setiosflags( std::ios::left )
<< "Funky colors in a console application" << endl;


cout << con::bg_black << con::fg_white
<< "\nWhite   text on black background\n";

cout << con::fg_gray    << "Gray    text on black background\n"
<< con::fg_red     << "Red     text on black background\n"
<< con::fg_green   << "Green   text on black background\n"
<< con::fg_blue    << "Blue    text on black background\n"
<< con::fg_cyan    << "Cyan    text on black background\n"
<< con::fg_magenta << "Magenta text on black background\n"
<< con::fg_yellow  << "Yellow  text on black background\n";

cout << con::bg_gray << con::fg_yellow << "Press the 'any' key..."
<< con::bg_black << con::fg_white;
int c = _getch();
cout << "\nThe character " << con::bg_yellow << con::fg_black
<< static_cast< char >( c ) << con::bg_black << con::fg_white
<< " could be considered "
<< con::fg_green << "OK" << con::fg_white << " or "
<< con::fg_red   << "WRONG" << con::fg_white
<< " depending on taste\n" << endl;

cout << con::bg_gray    << "White text on gray    background\n"
<< con::bg_red     << "White text on red     background\n"
<< con::bg_green   << "White text on green   background\n"
<< con::bg_blue    << "White text on blue    background\n"
<< con::bg_cyan    << "White text on cyan    background\n"
<< con::bg_magenta << "White text on magenta background\n"
<< con::bg_yellow  << "White text on yellow  background\n";

cout << con::bg_white << con::fg_blue << "Press the 'any' key..."
<< con::bg_black << con::fg_white;
_getch(); 
return 0;
}
*/

} //namespace Redmoon
#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

namespace Redmoon
{
#if _MSC_VER > 1400	
	#define VSTPRINTF(dest, size, src, vl)							_vstprintf_s(dest, size, src, vl)

	#define MBSTOWCS(nConvertedChars, pwszDest, nSrcLen, pszSrc)	mbstowcs_s(nConvertedChar, pwszDest, nSrcLen, pszSrc, _TRUNCATE)
	#define WCSTOMBS(nConvertedChars, pszDest, nSrcLen, pwszSrc)	wcstombs_s(nConvertedChars, pszDest, nSrcLen, pwszSrc, _TRUNCATE)

	#define TFOPEN(fp, szFileName, szMode)							_tfopen_s(&fp, szFileName, szMode)
	#define FTSCANF													_ftscanf_s
	#define FTPRINTF												_ftprintf_s

	#define FOPEN(fp, szFileName, szMode)							fopen_s(&fp, szFileName, szMode)

	#define TCSCAT(szDest, nDestSize, szSrc)						_tcscat_s(szDest, nDestSize, szSrc)
	#define TCSNCPY(szDest, nDestSize, szSrc)						_tcsncpy_s(szDest, nDestSize, szSrc, _TRUNCATE)
	#define TCSCPY(szDest, nDestSize, szSrc)						_tcscpy_s(szDest, nDestSize, szSrc)
	
	#define STRNCPY(szDest, szSrc, szSrcSize)						strncpy_s(szDest, _countof(szDest), szDestSize, szSrc, szSrcSize)
	
	#define SPRINTF													sprintf_s
	#define STRCPY(szDest, nDestSize, szSrc)						strcpy_s(szDest, nDestSize, szSrc)

	#define SNPRINTF												_snprintf
	#define STPRINTF												_stprintf

	#define SSCANF													sscanf_s

	#define ITOA(val, buf, buflen, radix)							_itoa_s(val, buf, buflen, radix)

	#define	TCSTOK(szStr, strDelimit, context)						_tcstok_s(szStr, strDelimit, context)

#else
	#define VSTPRINTF(dest, size, src, vl)							_vstprintf(dest, src, vl)
	#define MBSTOWCS(nConvertedChars, pwszDest, nSrcLen, pszSrc)	mbstowcs(pwszDest, pszSrc, nSrcLen)
	#define WCSTOMBS(nConvertedChars, pszDest, nSrcLen, pwszSrc)	wcstombs(pszDest, pwszSrc, nSrcLen)
	
	#define TFOPEN(fp, szFileName, szMode)							fp = _tfopen(szFileName, szMode)
	#define FTSCANF													_ftscanf
	#define FTPRINTF												_ftprintf

	#define FOPEN(fp, szFileName, szMode)							fp = fopen(szFileName, szMode)

	#define TCSCAT(szDest, nDestSize, szSrc)						_tcscat(szDest, szSrc)
	#define TCSNCPY(szDest, nDestSize, szSrc)						_tcsncpy(szDest, szSrc, nDestSize)
	#define TCSCPY(szDest, nDestSize, szSrc)						_tcscpy(szDest, szSrc)

	#define STRNCPY(szDest, szSrc, szSrcSize)						strncpy(szDest, szSrc, szSrcSize)
	#define SPRINTF													_stprintf
	#define STRCPY(szDest, nDestSize, szSrc)						strcpy(szDest, szSrc)


	#define SNPRINTF												_snprintf
	#define STPRINTF												_stprintf

	#define SSCANF													sscanf

	#define ITOA(val, buf, buflen, radix)							_itoa(val, buf, radix)

	#define	TCSTOK(szStr, strDelimit, context)						_tcstok(szStr, strDelimit)

#endif

} //namespace Redmoon
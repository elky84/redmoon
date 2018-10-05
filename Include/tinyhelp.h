#pragma once

#include "Str.h"

inline long TiAsLong (const TiXmlElement* e, const char* attr)
{	
	assert( e );
	int val = -1;
	if (e) e->Attribute (attr, &val);
	return val;
}

template<class T>
T TiAsNumeric (const TiXmlElement* e, const char* attr)
{	
	assert( e );
	T val = -1;
	if (e) e->Attribute (attr, &val);
	return val;
}

inline const char* TiAsString( const TiXmlElement* e, const char* attr )
{	
	assert( e );
	if( e->Attribute(attr) ) return e->Attribute(attr);
	else return "";
}

inline const TCHAR* TiAsTstring( const TiXmlElement* e, const char* attr )
{	
	return CHAR_TO_TCHAR(const_cast<char*>(TiAsString(e, attr)));
}

template<class T>
long TiChildAsNumeric (const TiXmlElement* e, const char* child, const char* attr)
{	
	assert( e );
	int val = -1;
	if (!e) return val;
	return TiAsNumeric( e->FirstChildElement( child ), attr ) ;
}

inline const char* TiChildAsString( const TiXmlElement* e, const char* child, const char* attr )
{	
	assert( e );
	if (!e) return NULL;
	return TiAsString( e->FirstChildElement( child ), attr ) ;
}

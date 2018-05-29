#include <lang/Character.h>
#include <ctype.h>
#include <config.h>


BEGIN_NAMESPACE(lang) 


bool Character::isDigit( int cp )
{
	if ( cp < int(0x80) )
		return 0 != isdigit( (char)cp );
	else
		return false;
}

bool Character::isLetter( int cp )
{
	if ( cp < int(0x80) )
		return 0 != isalpha( (char)cp );
	else
		return false;
}

bool Character::isLetterOrDigit( int cp )
{
	return isLetter(cp) || isDigit(cp);
}

bool Character::isLowerCase( int cp )
{
	if ( cp < int(0x80) )
		return 0 != islower( (char)cp );
	else
		return false;
}

bool Character::isUpperCase( int cp )
{
	if ( cp < int(0x80) )
		return 0 != isupper( (char)cp );
	else
		return false;
}

bool Character::isWhitespace( int cp )
{
	if ( cp < int(0x80) )
		return 0 != isspace( (char)cp );
	else
		return false;
}


END_NAMESPACE() // lang

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.

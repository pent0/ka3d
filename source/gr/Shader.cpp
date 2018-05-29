#include <gr/Shader.h>
#include <string.h>
#include <math/all.h>
#include <config.h>


BEGIN_NAMESPACE(gr) 


/** 
 * Shader parameter names. 
 */
static const char* const PARAM_NAMES[] =
{
	/** Invalid parameter */
	"NONE",

	/** World space position of camera (dynamic parameter) */
	"CAMERAP",
	/** Local->world transform (dynamic parameter) */
	"WORLDTM",
	/** World->camera transform (dynamic parameter) */
	"VIEWTM",
	/** Camera->world transform (dynamic parameter) */
	"VIEWTMINV",
	/** World->camera->projection transform (dynamic parameter) */
	"VIEWPROJTM",
	/** View->projection transform */
	"PROJTM",
	/** Object->projection transform (dynamic parameter) */
	"TOTALTM",
	/** Local->world transform array for bones (dynamic parameter) */
	"BONEWORLDTM",

	/** World position of light 0 (dynamic parameter) */
	"LIGHTP0",
	/** World position of light 1 (dynamic parameter) */
	"LIGHTP1",
	/** World position of light 2 (dynamic parameter) */
	"LIGHTP2",
	/** World position of light 3 (dynamic parameter) */
	"LIGHTP3",
	/** World position of light 4 (dynamic parameter) */
	"LIGHTP4",
	/** World position of light 5 (dynamic parameter) */
	"LIGHTP5",
	/** World position of light 6 (dynamic parameter) */
	"LIGHTP6",
	/** World position of light 7 (dynamic parameter) */
	"LIGHTP7",
	/** Color of light 0 (dynamic parameter) */
	"LIGHTC0",
	/** Color of light 1 (dynamic parameter) */
	"LIGHTC1",
	/** Color of light 2 (dynamic parameter) */
	"LIGHTC2",
	/** Color of light 3 (dynamic parameter) */
	"LIGHTC3",
	/** Color of light 4 (dynamic parameter) */
	"LIGHTC4",
	/** Color of light 5 (dynamic parameter) */
	"LIGHTC5",
	/** Color of light 6 (dynamic parameter) */
	"LIGHTC6",
	/** Color of light 7 (dynamic parameter) */
	"LIGHTC7",
};


Shader::Shader() : 
	ContextObject(CLASSID_SHADER) 
{
}

const char* Shader::toString( ParamType param )
{
	assert( PARAM_COUNT == sizeof(PARAM_NAMES)/sizeof(PARAM_NAMES[0]) );
	assert( (int)param < (int)(sizeof(PARAM_NAMES)/sizeof(PARAM_NAMES[0])) );

	return PARAM_NAMES[param];
}

Shader::ParamType Shader::toParamType( const char* sz )
{
	assert( PARAM_COUNT == sizeof(PARAM_NAMES)/sizeof(PARAM_NAMES[0]) );

	for ( int i = 0 ; i < PARAM_COUNT ; ++i )
	{
		if ( !strcmp(sz,PARAM_NAMES[i]) )
			return (ParamType)i;
	}
	return PARAM_NONE;
}


END_NAMESPACE() // gr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.

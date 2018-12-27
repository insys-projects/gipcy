
#include "ipclib.h"
#include	<stdio.h>
#include <stdarg.h>

#ifndef __DEBUGIPC_H__
#include "debugipc.h"
#endif

#ifdef _UNICODE
#include <cwchar>

#define IPC_fopen		_wfopen
#define IPC_vsprintf	_vswprintf
#define IPC_fprintf		fwprintf
#else
#define IPC_fopen		fopen
#define IPC_vsprintf	vsprintf
#define IPC_fprintf		fprintf
#endif


static IPC_str _bigbuffer[1024];

static int _screenflags = 0xFF;
static int _fileflags = 0;

static FILE* _f = NULL;

GIPCY_API IPC_handle IPC_InitDebug(const IPC_str *name, int screenflags, int fileflags )
{
	if( name && (fileflags>0) )
#ifndef _WIN64
		_f = IPC_fopen(name, "w+t");
#else
		_f = IPC_fopen(name, L"w+t");
#endif

	return 0;
}

GIPCY_API void IPC_VPrint(int flags,const IPC_str *format, va_list args)
{
	IPC_vsprintf(_bigbuffer, format, args); 
	
	if( _screenflags&flags > 0 )
		IPC_fprintf(stdout, _bigbuffer);
	
	if (stderr != stdout && (flags & IPCD_ERR > 0) )
		IPC_fprintf(stderr, _bigbuffer);

	if( _f && (_fileflags&flags > 0) )
		IPC_fprintf(_f, _bigbuffer);
}

GIPCY_API void IPC_Printf(int flags,const IPC_str *format, ...)
{
	va_list args;
	va_start(args, format);
	IPC_VPrint(flags, _bigbuffer, args);
	va_end(args);
}

GIPCY_API void IPC_Debugf( const IPC_str *format, ...)
{
	va_list args;
	va_start(args, format);
	IPC_VPrint(IPCD_WARN, _bigbuffer, args);
	va_end(args);
}

GIPCY_API void IPC_Errorf(const IPC_str *format, ...)
{
	va_list args;
	va_start(args, format);
	IPC_VPrint(IPCD_ERR, _bigbuffer, args);
	va_end(args);
}

GIPCY_API void IPC_Logf(const IPC_str *format, ...)
{
	va_list args;
	va_start(args, format);
	IPC_VPrint(IPCD_LOG, _bigbuffer, args);
	va_end(args);
}
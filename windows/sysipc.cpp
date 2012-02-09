
#ifdef __IPC_WIN__

#include <stdio.h>

#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __SYSIPC_H__
    #include "sysipc.h"
#endif

//-----------------------------------------------------------------------------
// для совместимости с LINUX
void IPC_initKeyboard(void)
{
}

//-----------------------------------------------------------------------------
// для совместимости с LINUX
void IPC_cleanupKeyboard(void)
{
}

//-----------------------------------------------------------------------------
// для совместимости с LINUX
int IPC_getch(void)
{
    return _getch();
}

//-----------------------------------------------------------------------------
// для совместимости с LINUX
int IPC_kbhit(void)
{
    return _kbhit();
}

//-----------------------------------------------------------------------------
// выполнить задержку при выполнении программы
GIPCY_API void IPC_delay(int ms)
{
    Sleep(ms);
}

//-----------------------------------------------------------------------------
// получить последнюю ошибку системной функции
GIPCY_API int IPC_sysError()
{
	return GetLastError();
}

//-----------------------------------------------------------------------------
GIPCY_API int IPC_getFullPath(const IPC_str *name, IPC_str *path)
{
	IPC_str *FirsChar;
	ULONG res = GetFullPathName(name, MAX_PATH, path, &FirsChar);
	if(!res)
		return IPC_GENERAL_ERROR;
    return IPC_OK;
}

//-----------------------------------------------------------------------------
GIPCY_API const IPC_str* IPC_getCurrentDir(IPC_str *buf, int size)
{
	GetCurrentDirectory(size, buf);
	return buf;

}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_getPrivateProfileString(const IPC_str *lpAppName, const IPC_str *lpKeyName, const IPC_str *lpDefault,
                                IPC_str *lpReturnedString, int nSize, const IPC_str *lpFileName)
{
    return GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_writePrivateProfileString(const IPC_str *lpAppName, const IPC_str *lpKeyName, const IPC_str *lpString, const IPC_str *lpFileName)
{
    return WritePrivateProfileString( lpAppName, lpKeyName, lpString, lpFileName );
}

//-----------------------------------------------------------------------------

GIPCY_API long IPC_interlockedDecrement(volatile long *val )
{
    return InterlockedDecrement(val);
}

//-----------------------------------------------------------------------------

GIPCY_API long IPC_interlockedIncrement(volatile long *val )
{
    return InterlockedIncrement(val);
}

//-----------------------------------------------------------------------------

GIPCY_API long IPC_interlockedCompareExchange(volatile long *dst, long val, long param )
{
    return InterlockedCompareExchange(dst, val, param);
}

//-----------------------------------------------------------------------------

GIPCY_API long IPC_interlockedExchange(volatile long *dst, long val )
{
    return InterlockedExchange(dst, val);
}

//-----------------------------------------------------------------------------

GIPCY_API long IPC_interlockedExchangeAdd(volatile long *dst, long val )
{
    return InterlockedExchangeAdd(dst, val);
}

//-----------------------------------------------------------------------------

#endif //__IPC_WIN__


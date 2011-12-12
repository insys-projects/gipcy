
#ifdef __IPC_WIN__

#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __SYSIPC_H__
    #include "sysipc.h"
#endif

void IPC_delay(int ms)
{
    Sleep(ms);
}

#endif //__IPC_WIN__


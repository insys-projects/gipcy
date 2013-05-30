#ifdef __IPC_WIN__

#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __SHMIPC_H__
    #include "shmipc.h"
#endif

#include "sockipc.h"

#include <winsock.h>

int	IPC_initSocket( )
{
	WSADATA WsaData;
	WORD wVersionRequested = MAKEWORD( 1, 0 );

	if(WSAStartup (wVersionRequested, &WsaData) == SOCKET_ERROR)		
		return 0;

	return 0;
}

SOCKET _IPC_udp()
{
	return 0;
}

SOCKET _IPC_tcp()
{
	return 0;
}

IPC_handle	IPC_openSocket( IPC_proto proto )
{
	ipc_handle_t h = allocate_ipc_object( "socket", IPC_typeSocket );

    if(!h)
        return NULL;

    h->ipc_data = NULL;

	SOCKET s = 0;

	switch( proto )
	{
	case IPC_udp:
		{
			s = _IPC_udp( );
			break;
		}
	case IPC_tcp:
		{
			s = _IPC_tcp( );
			break;
		}
	}

    h->ipc_descr = (HANDLE)s;

    if(h->ipc_descr == NULL)
	{
        delete_ipc_object(h);
        return NULL;
    }

    return h;
}

int IPC_closeSocket( IPC_handle s )
{   
	if(!s)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)s;

    if(h->ipc_type != IPC_typeEvent)
        return IPC_INVALID_HANDLE;

	BOOL ret = closesocket( (SOCKET)h->ipc_descr );
	if(!ret)
	    return IPC_GENERAL_ERROR;
	
    delete_ipc_object(h);
    return IPC_OK;
}

#endif
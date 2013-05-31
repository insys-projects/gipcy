#ifdef __IPC_WIN__

#ifndef __WINIPC_H__
    #include "winipc.h"
#endif


#include "sockipc.h"

#include <string.h>
#include <stdlib.h>     /* atoi */

#include <winsock.h>

int	IPC_initSocket( )
{
	WSADATA WsaData;
	WORD wVersionRequested = MAKEWORD( 1, 0 );

	if(WSAStartup (wVersionRequested, &WsaData) == SOCKET_ERROR)		
		return 0;

	return 0;
}

IPC_sockaddr IPC_resolve( IPC_str* addr )
{
	IPC_sockaddr a;
	
	int port = INADDR_ANY;
	
	//FIXME: small?
	char buffer[256];
	strcpy( buffer, addr );

	char *pp = strstr( buffer, ":" );

	if( pp )
	{
		port = atoi( pp+1 );
		*pp=0;
	}

	unsigned long ip = inet_addr( buffer );

	if( ip == INADDR_NONE )
		ip = INADDR_ANY;

	a.port = port;
	a.addr.ip = ip;

	return a;
}

SOCKET _IPC_udp()
{
	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	
	int uMode=1;
	if(ioctlsocket(s, FIONBIO, (u_long*)&uMode) == SOCKET_ERROR)
		return SOCKET_ERROR;
	
	// make it broadcast capable
	int	i = 1;
	if( setsockopt( s, SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i) ) == SOCKET_ERROR )
		return SOCKET_ERROR;	

	return s;
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

int IPC_bind( IPC_handle s, IPC_sockaddr* ip )
{
	 ipc_handle_t h = (ipc_handle_t)s;
	 
	 {
		sockaddr_in srcAddr;
		srcAddr.sin_family = AF_INET;
		srcAddr.sin_port = htons( ip->port );
		srcAddr.sin_addr.S_un.S_addr = ( ip->addr.ip );

		if(bind( (SOCKET)h->ipc_descr, (sockaddr*)&srcAddr, sizeof(srcAddr)) == SOCKET_ERROR)
			return -1;
	 }

	 return IPC_OK;
}

int IPC_sendTo( IPC_handle s, IPC_sockaddr* ip,char *data, int size, int timeout )
{
	sockaddr_in srcAddr;
	int size_sockaddr = sizeof(srcAddr);

	

	srcAddr.sin_family = AF_INET;
	srcAddr.sin_port = htons( ip->port );
	srcAddr.sin_addr.S_un.S_addr = ( ip->addr.ip );
	

	int cnt;
	
	struct fd_set WriteSet;
    struct timeval tval={0, 100};
	
	ipc_handle_t h = (ipc_handle_t)s;
	
	char *pbuf = data;
	char *pend = data + size;	

	while( true )
	{
		FD_ZERO(&WriteSet);
		FD_SET( (SOCKET)h->ipc_descr, &WriteSet);

		int r = select(1, 0, &WriteSet, 0, &tval);
		
		if(r == 0)
		{
			continue;
		}

		cnt = sendto((SOCKET)h->ipc_descr, pbuf, size, 0,(struct sockaddr*)&srcAddr, size_sockaddr );

		if(cnt == SOCKET_ERROR)
		{
			//printf("error send \n" );
			int err = GetLastError();

			continue;
		}

		break;
	}

	return size;
}

int IPC_recvFrom( IPC_handle s, IPC_sockaddr* ip,char *data, int size, int timeout  )
{
	sockaddr_in srcAddr;
	int size_sockaddr = sizeof(srcAddr);
	
	int cnt;
	
	struct fd_set WriteSet;
    struct timeval tval={0, 100};
	
	ipc_handle_t h = (ipc_handle_t)s;
	
	char *pbuf = data;
	char *pend = data + size;	

	while( true )
	{
		FD_ZERO(&WriteSet);
		FD_SET( (SOCKET)h->ipc_descr, &WriteSet);

		int r = select(1, 0, &WriteSet, 0, &tval);
		
		if(r == 0)
		{
			continue;
		}

		cnt = recvfrom((SOCKET)h->ipc_descr, pbuf, size, 0,(struct sockaddr*)&srcAddr, &size_sockaddr );

		if(cnt == SOCKET_ERROR)
		{
			//printf("error send \n" );
			continue;
		}

		break;
	}

	if( ip )
	{
		ip->port = ntohs( srcAddr.sin_port ); 
		ip->addr.ip = ( srcAddr.sin_addr.S_un.S_addr ); 
	}

	return size;
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
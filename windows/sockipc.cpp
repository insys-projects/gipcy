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
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP );
	
	int uMode=1;
	if(ioctlsocket(s, FIONBIO, (u_long*)&uMode) == SOCKET_ERROR)
		return SOCKET_ERROR;
	
	int yes = 1;
	setsockopt( s, IPPROTO_TCP, TCP_NODELAY, (char *) &yes, sizeof(yes));//NODELAY

	return s;
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

int IPC_listen( IPC_handle s, IPC_sockaddr* ip, int backlog )
{
	 ipc_handle_t h = (ipc_handle_t)s;
	 
	 {
		sockaddr_in srcAddr;
		srcAddr.sin_family = AF_INET;
		srcAddr.sin_port = htons( ip->port );
		srcAddr.sin_addr.S_un.S_addr = ( ip->addr.ip );

		if(bind( (SOCKET)h->ipc_descr, (sockaddr*)&srcAddr, sizeof(srcAddr)) == SOCKET_ERROR)
			return IPC_GENERAL_ERROR;
	 }

	 if( listen( (SOCKET)h->ipc_descr, backlog ) == SOCKET_ERROR )
		return IPC_GENERAL_ERROR;

	 return IPC_OK;
}

IPC_handle IPC_accept( IPC_handle s, IPC_sockaddr* ip, int timeout )
{
	ipc_handle_t h = (ipc_handle_t)s;

	SOCKET _s;
	sockaddr_in anAddr; 

	while( true )
	{
		
		int len = sizeof(anAddr);
		
		_s = accept( (SOCKET)h->ipc_descr, (sockaddr*)&anAddr,  &len );

		if( _s != -1 )
			break;
	}


	if( ip )
	{
		ip->port = ntohs( anAddr.sin_port ); 
		ip->addr.ip = ( anAddr.sin_addr.S_un.S_addr ); 
	}

	ipc_handle_t _h = allocate_ipc_object( "socket", IPC_typeSocket );

	_h->ipc_descr = (HANDLE)_s;

	return _h;
}

int IPC_listen( IPC_handle s, int backlog )
{
	ipc_handle_t h = (ipc_handle_t)s;

	return listen((SOCKET)h->ipc_descr, backlog);
}

int IPC_connect( IPC_handle s, IPC_sockaddr* ip )
{
	sockaddr_in anAddr;    
	
	ipc_handle_t h = (ipc_handle_t)s;
	
	anAddr.sin_family = AF_INET;                   // ������������� ������ � ���������� ���� 
	anAddr.sin_port = htons(ip->port);
	anAddr.sin_addr.S_un.S_addr = ip->addr.ip;
	
	int err = connect( (SOCKET)h->ipc_descr, (struct sockaddr*)&anAddr, sizeof(struct sockaddr));	
	
	return 0;
}

int IPC_send( IPC_handle s, char *data, int size, int timeout )
{
	ipc_handle_t h = (ipc_handle_t)s;
	
	int ret;
	int total = size;
	

	do
	{ 
		struct fd_set WriteSet;
		struct timeval tval={1,0};
	
		FD_ZERO(&WriteSet);	
		FD_SET( (SOCKET)h->ipc_descr,&	WriteSet);
		
		int r = select(0, 0, &WriteSet, 0, &tval);

		if( r <= 0 )
			continue;

		ret = send( (SOCKET)h->ipc_descr, data, size, 0 ); 
		
		if( ret == -1 ) 
			continue; 

		size -= ret;
		data += ret;

		if( size > 0 )
			continue;

		break;

	}while(1);

	return total;
}

int IPC_recv( IPC_handle s, char *data, int size, int timeout  )
{	
	ipc_handle_t h = (ipc_handle_t)s;
	
	int total = size;
	int ret = size;

	fd_set ReadSet;
    timeval tval={0,10};
	
	do
	{ 
		
			
		FD_ZERO(&ReadSet);	
		FD_SET( (SOCKET)h->ipc_descr,&	ReadSet);
		int r = select(0, &ReadSet, 0, 0, &tval);

		if( r <= 0 )
			continue;

		int ret = recv( (SOCKET)h->ipc_descr, data, size, 0 ); 
		
		if( ret == -1 ) 
			continue; 
		
		size -= ret;
		data += ret;

		if( size > 0 )
			continue;

		break;

	}while(1);

	return total;
}

int IPC_closeSocket( IPC_handle s )
{   
	if(!s)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)s;

    if(h->ipc_type != IPC_typeSocket )
        return IPC_INVALID_HANDLE;

	int ret = closesocket( (SOCKET)h->ipc_descr );
	if( ret == SOCKET_ERROR )
	    return IPC_GENERAL_ERROR;
	
    delete_ipc_object(h);
    return IPC_OK;
}

void IPC_FD_ZERO(fd_set *set)
{
	FD_ZERO(set);
}

void IPC_FD_SET(IPC_handle s, fd_set *set)
{
	ipc_handle_t h = (ipc_handle_t)s;

	FD_SET((SOCKET)h->ipc_descr, set);
} 

int IPC_shutdown(IPC_handle s, int how)
{
	ipc_handle_t h = (ipc_handle_t)s;

	return shutdown((SOCKET)h->ipc_descr, how);
}

int IPC_setsockopt(IPC_handle s, int level, int optname, const char *optval, int optlen)
{
	ipc_handle_t h = (ipc_handle_t)s;

	return setsockopt((SOCKET)h->ipc_descr, level, optname, optval, optlen);
}
#endif
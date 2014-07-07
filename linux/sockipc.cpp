#ifdef __IPC_LINUX__

#ifndef __LINIPC_H__
    #include "linipc.h"
#endif
#ifndef __GIPCY_H__
    #include "gipcy.h"
#endif

int	IPC_initSocket( )
{
    return 0;
}

int IPC_cleanupSocket()
{
    return 0;
}

int _IPC_udp()
{
    int s = socket(AF_INET, SOCK_DGRAM, 0 );

    if(fcntl(s, F_SETFL, O_NONBLOCK) == IPC_SOCKET_ERROR)
        return IPC_SOCKET_ERROR;

    // make it broadcast capable
    int	i = 1;
    socklen_t len = sizeof(i);

    if( setsockopt( s, SOL_SOCKET, SO_BROADCAST, (void*)&i, len ) == IPC_SOCKET_ERROR )
        return IPC_SOCKET_ERROR;

    return s;
}

int _IPC_tcp()
{
    int s = socket(AF_INET, SOCK_STREAM, 0 );

    if(fcntl(s, F_SETFL, O_NONBLOCK) == IPC_SOCKET_ERROR)
        return IPC_SOCKET_ERROR;

    int yes = 1;
    socklen_t len = sizeof(yes);

    if( setsockopt( s, IPPROTO_TCP, TCP_NODELAY, &yes, len) == IPC_SOCKET_ERROR )
        return IPC_SOCKET_ERROR;

    return s;
}

IPC_handle	IPC_openSocket( IPC_proto proto )
{
    ipc_handle_t h = allocate_ipc_object( "socket", IPC_typeSocket );

    if(!h)
        return NULL;

    h->ipc_data = NULL;

    int s = 0;

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

    h->ipc_descr.ipc_sock = s;

    return h;
}

int IPC_listen( IPC_handle s, int backlog )
{
    ipc_handle_t h = (ipc_handle_t)s;

    return listen(h->ipc_descr.ipc_sock, backlog);
}

IPC_handle IPC_accept( IPC_handle s, IPC_sockaddr* ip, int timeout )
{
    ipc_handle_t h = (ipc_handle_t)s;

    int _s;
    sockaddr_in anAddr;

    while( true )
    {

        int len = sizeof(anAddr);

        _s = accept( h->ipc_descr.ipc_sock, (sockaddr*)&anAddr,  (socklen_t *)&len );

        if( _s != -1 )
            break;
    }


    if( ip )
    {
        ip->port = ntohs( anAddr.sin_port );
        ip->addr.ip = ( anAddr.sin_addr.s_addr );
    }

    ipc_handle_t _h = allocate_ipc_object( "socket", IPC_typeSocket );

    _h->ipc_descr.ipc_sock = _s;

    return _h;
}

int IPC_bind( IPC_handle s, IPC_sockaddr* ip )
{
     ipc_handle_t h = (ipc_handle_t)s;

     {
        struct sockaddr_in srcAddr;
        srcAddr.sin_family = AF_INET;
        srcAddr.sin_port = htons( ip->port );
        srcAddr.sin_addr.s_addr = ( ip->addr.ip );

        if(bind( h->ipc_descr.ipc_sock, (sockaddr*)&srcAddr, sizeof(srcAddr)) == IPC_SOCKET_ERROR)
            return -1;
     }

     return IPC_OK;
}

int IPC_select( IPC_handle s, fd_set *readfds, fd_set *writefds,
                         fd_set *exceptfds, const struct timeval *timeout )
{
    ipc_handle_t h = (ipc_handle_t)s;

    return select(h->ipc_descr.ipc_sock + 1, readfds, writefds, exceptfds, (timeval*)timeout);
}

int IPC_send( IPC_handle s, char *data, int size, int timeout )
{
    ipc_handle_t h = (ipc_handle_t)s;

    return send( h->ipc_descr.ipc_sock, data, size, 0 );
}

int IPC_recv( IPC_handle s, char *data, int size, int timeout  )
{
    ipc_handle_t h = (ipc_handle_t)s;

    return recv( h->ipc_descr.ipc_sock, data, size, 0 );
}

int IPC_sendTo( IPC_handle s, IPC_sockaddr* ip,char *data, int size, int timeout )
{
    sockaddr_in srcAddr;
    int size_sockaddr = sizeof(srcAddr);

    srcAddr.sin_family = AF_INET;
    srcAddr.sin_port = htons( ip->port );
    srcAddr.sin_addr.s_addr = ( ip->addr.ip );

    int cnt;

    fd_set WriteSet;
    struct timeval tval={1, 0};

    ipc_handle_t h = (ipc_handle_t)s;

    char *pbuf = data;
    char *pend = data + size;

    while( true )
    {
        FD_ZERO(&WriteSet);
        FD_SET( h->ipc_descr.ipc_sock, &WriteSet);

        int r = select(h->ipc_descr.ipc_sock + 1, 0, &WriteSet, 0, &tval);

		tval.tv_sec = 1;
		tval.tv_usec = 0;

		if(r == 0)
			return 0;
		else if(r == -1)
			return -1;
			
        cnt = sendto(h->ipc_descr.ipc_sock, pbuf, size, 0,(struct sockaddr*)&srcAddr, size_sockaddr );

        if(cnt == -1)
            return -1;

        break;
    }

    return size;
}

int IPC_recvFrom( IPC_handle s, IPC_sockaddr* ip,char *data, int size, int timeout  )
{
    sockaddr_in srcAddr;
    socklen_t fromlen = sizeof(srcAddr);

    srcAddr.sin_family = AF_INET;
    srcAddr.sin_port = htons( ip->port );
    srcAddr.sin_addr.s_addr = ( ip->addr.ip );

    int cnt;

    fd_set ReadSet;
    struct timeval tval={1, 0};

    ipc_handle_t h = (ipc_handle_t)s;

    char *pbuf = data;
    char *pend = data + size;

    while( true )
    {
        FD_ZERO(&ReadSet);
        FD_SET( h->ipc_descr.ipc_sock, &ReadSet);

        int r = select(h->ipc_descr.ipc_sock + 1, &ReadSet, 0, 0, &tval);

		tval.tv_sec = 1;
		tval.tv_usec = 0;

		if(r == 0)
			return 0;
		else if(r == -1)
			return -1;

        cnt = recvfrom(h->ipc_descr.ipc_sock, pbuf, size, 0,(struct sockaddr*)&srcAddr, &fromlen );

        if(cnt == -1)
            return -1;

        break;
    }

    return size;
}

int IPC_closeSocket( IPC_handle s )
{
    if(!s)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)s;

    if(h->ipc_type != IPC_typeSocket )
        return IPC_INVALID_HANDLE;

    int ret = close( h->ipc_descr.ipc_sock );
    if( ret == IPC_SOCKET_ERROR )
        return IPC_GENERAL_ERROR;

    return IPC_OK;
}

void IPC_FD_ZERO(fd_set *set)
{
    FD_ZERO(set);
}

void IPC_FD_SET(IPC_handle s, fd_set *set)
{
    ipc_handle_t h = (ipc_handle_t)s;

    FD_SET(h->ipc_descr.ipc_sock, set);
}

void IPC_FD_CLR(IPC_handle s, fd_set *set)
{
    ipc_handle_t h = (ipc_handle_t)s;

    FD_CLR(h->ipc_descr.ipc_sock, set);
}

int IPC_shutdown(IPC_handle s, int how)
{
    ipc_handle_t h = (ipc_handle_t)s;

    return shutdown(h->ipc_descr.ipc_sock, how);
}

int IPC_setsockopt(IPC_handle s, int level, int optname, const char *optval, int optlen)
{
    ipc_handle_t h = (ipc_handle_t)s;

    return setsockopt(h->ipc_descr.ipc_sock, level, optname, optval, optlen);
}

unsigned int IPC_ntohl(unsigned int netlong)
{
    return ntohl(netlong);
}

char *IPC_inet_ntoa(unsigned long addr)
{
    in_addr rAddr;

    rAddr.s_addr = addr;

    return inet_ntoa(rAddr);
}

#endif //__IPC_LINUX__

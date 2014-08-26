#ifndef __SOCKIPC_H__
#define __SOCKIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif

#ifdef WIN32
#define IPC_SOCKET_ERROR SOCKET_ERROR
#define IPC_EWOULDBLOCK WSAEWOULDBLOCK
#endif // WIN32

#if defined(__linux__) || defined(__QNX__)
#define IPC_SOCKET_ERROR -1
#define IPC_EWOULDBLOCK EWOULDBLOCK

#include <sys/select.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <Winsock.h>
#endif // __linux__

#ifdef __cplusplus
extern "C" {
#endif

#define IPC_SD_RECEIVE 0
#define IPC_SD_SEND 1
#define IPC_SD_BOTH 2

#pragma pack(push, 4)

typedef struct 
{
        unsigned short port;
		union {
			char    data[14];
			unsigned long ip;
		} addr;
} IPC_sockaddr;

#pragma pack(pop)

enum IPC_proto {

    IPC_udp,
    IPC_tcp
};

GIPCY_API int	IPC_initSocket( );
GIPCY_API int   IPC_cleanupSocket();

GIPCY_API IPC_sockaddr IPC_resolve( IPC_str* addr );

GIPCY_API IPC_handle	IPC_openSocket( IPC_proto proto );

GIPCY_API int IPC_listen( IPC_handle s, int backlog );
GIPCY_API IPC_handle IPC_accept( IPC_handle s, IPC_sockaddr* ip, int timeout );

GIPCY_API int IPC_connect( IPC_handle s, IPC_sockaddr* ip );
GIPCY_API int IPC_bind( IPC_handle s, IPC_sockaddr* ip );

GIPCY_API int IPC_select( IPC_handle s, fd_set *readfds, fd_set *writefds, 
						 fd_set *exceptfds, const struct timeval *timeout );

GIPCY_API int IPC_send( IPC_handle s, char *data, int size, int timeout );
GIPCY_API int IPC_recv( IPC_handle s, char *data, int size, int timeout  );

GIPCY_API int IPC_sendTo( IPC_handle s, IPC_sockaddr* ip,char *data, int size, int timeout );
GIPCY_API int IPC_sendto( IPC_handle s, char *data, int size, int flags, IPC_sockaddr* ip );
GIPCY_API int IPC_recvFrom( IPC_handle s, IPC_sockaddr* ip,char *data, int size, int timeout  );
GIPCY_API int IPC_recvfrom( IPC_handle s, char *data, int size, int flags, IPC_sockaddr* ip );

GIPCY_API int IPC_closeSocket( IPC_handle s );

GIPCY_API void IPC_FD_ZERO(fd_set *set);
GIPCY_API void IPC_FD_SET(IPC_handle s, fd_set *set);
GIPCY_API void IPC_FD_CLR(IPC_handle s, fd_set *set);

GIPCY_API int IPC_shutdown(IPC_handle s, int how);

GIPCY_API int IPC_setsockopt(IPC_handle s, int level, int optname, const char *optval, int optlen);

GIPCY_API unsigned int IPC_ntohl(unsigned int netlong);
GIPCY_API unsigned int IPC_htonl(unsigned int hostlong);

GIPCY_API char *IPC_inet_ntoa(unsigned long addr);

#ifdef __cplusplus
};
#endif

#endif

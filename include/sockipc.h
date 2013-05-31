#ifndef __SOCKIPC_H__
#define __SOCKIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
        unsigned short port;
		union {
			char    data[14];
			unsigned long ip;
		} addr;
} IPC_sockaddr;
	
enum IPC_proto {

    IPC_udp,
    IPC_tcp
};

int	IPC_initSocket( );

IPC_sockaddr IPC_resolve( IPC_str* addr );

IPC_handle	IPC_openSocket( IPC_proto proto );

int IPC_listen( IPC_handle s, IPC_sockaddr* ip, int backlog );
int IPC_connect( IPC_handle s, IPC_sockaddr* ip );
int IPC_bind( IPC_handle s, IPC_sockaddr* ip );

int IPC_select( IPC_handle s, int timeout );

int IPC_send( IPC_handle s, char *data, int size, int timeout );
int IPC_recv( IPC_handle s, char *data, int size, int timeout  );

int IPC_sendTo( IPC_handle s, IPC_sockaddr* ip,char *data, int size, int timeout );
int IPC_recvFrom( IPC_handle s, IPC_sockaddr* ip,char *data, int size, int timeout  );

int IPC_closeSocket( IPC_handle s );

#ifdef __cplusplus
};
#endif

#endif

#ifdef __IPC_LINUX__

#ifndef __LINIPC_H__
    #include "linipc.h"
#endif
#ifndef __SYSIPC_H__
    #include "sysipc.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <time.h>
#include <sys/ioctl.h>

//-----------------------------------------------------------------------------

void IPC_delay(int ms)
{
    struct timeval tv = {0, 0};
    tv.tv_usec = 1000*ms;

    select(0,NULL,NULL,NULL,&tv);
}

//-----------------------------------------------------------------------------

int IPC_sysError()
{
    return errno;
}

//-----------------------------------------------------------------------------
int IPC_getFullPath(const IPC_str *name, IPC_str *path)
{
    char* retpath = realpath(name, path);
    if(!retpath)
        return errno;
    return 0;
}

//-----------------------------------------------------------------------------

static int convert_ipc_flags(IPC_flags ipc_flags)
{
    int flags = 0;

    if(ipc_flags & IPC_CREATE_FILE) {
        flags |= (O_CREAT | O_TRUNC);
    }

    if(ipc_flags & IPC_OPEN_FILE) {
        flags |= O_RDONLY;
    }


    if(ipc_flags & IPC_FILE_RDONLY) {
        flags |= O_RDONLY;
    }

    if(ipc_flags & IPC_FILE_WRONLY) {
        flags |= O_WRONLY;
    }

    if(ipc_flags & IPC_FILE_RDWR) {
        flags |= O_RDWR;
    }

    return flags;
}

//-----------------------------------------------------------------------------

IPC_handle IPC_openFile(const IPC_str *name, IPC_flags flags)
{
    DEBUG_PRINT("%s(%s)\n", __FUNCTION__, name );

    if(!name) return NULL;

    ipc_handle_t h = allocate_ipc_object(name, IPC_typeFile);
    if(!h) return NULL;

    h->ipc_size = 0;

    int sysflag = convert_ipc_flags(flags);

    h->ipc_descr.ipc_file = open(name, sysflag, 0666);
    if(h->ipc_descr.ipc_file < 0) {

            DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
            return NULL;
    }

    DEBUG_PRINT("%s(): open file - %s\n", __FUNCTION__, name );

    return h;
}

//-----------------------------------------------------------------------------

IPC_handle IPC_openFileEx(const IPC_str *name, IPC_flags flags, ...)
{
    DEBUG_PRINT("%s(%s)\n", __FUNCTION__, name );

    if(!name) return NULL;

    ipc_handle_t h = allocate_ipc_object(name, IPC_typeFile);
    if(!h) return NULL;

    h->ipc_size = 0;

    int sysflag = convert_ipc_flags(flags);

    h->ipc_descr.ipc_file = open(name, sysflag, 0666);
    if(h->ipc_descr.ipc_file < 0) {

            DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
            return NULL;
    }

    DEBUG_PRINT("%s(): open file - %s\n", __FUNCTION__, name );

    return h;
}

//-----------------------------------------------------------------------------

int IPC_closeFile(IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    int res = close(h->ipc_descr.ipc_file);
    if(res < 0) {
            DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
            return -1;
    }

    DEBUG_PRINT("%s(): close file - %s\n", __FUNCTION__, h->ipc_name );

    delete_ipc_object(h);

    return 0;
}

//-----------------------------------------------------------------------------

int IPC_readFile(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    int res = read(h->ipc_descr.ipc_file,data,size);
    if(res < 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return -1;
    }

    return res;
}

//-----------------------------------------------------------------------------

int IPC_writeFile(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    int res = write(h->ipc_descr.ipc_file,data,size);
    if(res < 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return -1;
    }

    return res;
}

//-----------------------------------------------------------------------------

IPC_handle IPC_openDevice(IPC_str *devname, const IPC_str *mainname, int devnum)
{
    DEBUG_PRINT("%s(%s)\n", __FUNCTION__, mainname);

    if(!mainname || !devname) return NULL;

    snprintf( devname, 128, "%s%s%d", "/dev/", mainname, devnum);
    ipc_handle_t h = allocate_ipc_object(devname, IPC_typeFile);
    if(!h) return NULL;

    h->ipc_size = 0;

    h->ipc_descr.ipc_file = open(devname, S_IROTH | S_IWOTH );
    if(h->ipc_descr.ipc_file < 0) {

            DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
            return NULL;
    }

    DEBUG_PRINT("%s(): open file - %s\n", __FUNCTION__, devname );

    return h;
}

//-----------------------------------------------------------------------------

int IPC_closeDevice(IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    int res = close(h->ipc_descr.ipc_file);
    if(res < 0) {
            DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
            return -1;
    }

    DEBUG_PRINT("%s(): close file - %s\n", __FUNCTION__, h->ipc_name );

    delete_ipc_object(h);

    return 0;
}

//-----------------------------------------------------------------------------

int IPC_readDevice(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    int res = read(h->ipc_descr.ipc_file,data,size);
    if(res < 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return -1;
    }

    return res;
}

//-----------------------------------------------------------------------------

int IPC_writeDevice(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    int res = write(h->ipc_descr.ipc_file,data,size);
    if(res < 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return -1;
    }

    return res;
}

//-----------------------------------------------------------------------------

int IPC_ioctlDevice(IPC_handle handle, unsigned long cmd, void *param)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    int res = ioctl(h->ipc_descr.ipc_file,cmd,param);
    if(res < 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return -1;
    }

    return res;
}

//-----------------------------------------------------------------------------

static int FindSection(const char* src, const char* section)
{
    int key_size = strlen(section);

    for(uint i = 0; i < (strlen(src) - key_size); i++)
    {
        const char *psubstr = &src[i];

        if(psubstr[i] == ';')
            return -1;
        if(psubstr[i] == '\0')
            return -2;

        if(psubstr[i] == '[') //begin section
        {
            char name[PATH_MAX] = {0};
            snprintf(name, key_size+1, "%s\n", &psubstr[i+1] );

            if(!strcmp(name, section))
            {
                DEBUG_PRINT("Section: < %s > was found in the string < %s >\n", section, src);
                return 0;
            }
            else
            {
                DEBUG_PRINT("Section: < %s > was not found\n", section);
                return -3;
            }
        }
    }
    return -4;
}

//-----------------------------------------------------------------------------

static int FindOption(const char* src, const char* option, char *Buffer, int BufferSize, int *set_default)
{
    int key_size = strlen(option);

    if(!set_default)
        return -1;

    *set_default = 1;

    for(uint i = 0; i < (strlen(src) - key_size); i++)
    {
        const char *psubstr = &src[i];

        if(psubstr[i] == ';')
            return -1;
        if(psubstr[i] == '\0')
            return -2;

        if(psubstr[i] == '[')
            return -3;

        char name[128] = {0};
        snprintf(name, key_size+1, "%s\n", &psubstr[i] );

        if(!strcmp(name, option))
        {
            DEBUG_PRINT("Option: < %s > was found in the string < %s >\n", option, src);

            char *val = (char*)strstr(src, "=");

            val++;
/*
            if( strstr(val, ".") ) {
                DEBUG_PRINT(stderr, "Val = %f\n", atof(val));
            } else {
                DEBUG_PRINT(stderr, "Val = %d\n", atoi(val));
            }
*/
            if(BufferSize >= (int)(strlen(val)+1)) {
                strcpy( Buffer, val);
            } else {
                DEBUG_PRINT("Option: < %s > was found in the string < %s >. But buffer to small\n", option, src);
                return -3;
            }

            *set_default = 0;

            return 0;
        }
        else
        {
            DEBUG_PRINT("Option: < %s > was not found\n", option);
            return -4;
        }
    }
    return -5;
}

//-----------------------------------------------------------------------------
#include <fstream>
using namespace std;
//-----------------------------------------------------------------------------
//const char *Section, const char *Option, const char *Default,
//char *Buffer, int BufferSize, const char *FileName

int IPC_getPrivateProfileString( const IPC_str *lpAppName, const IPC_str *lpKeyName, const IPC_str *lpDefault,
                             IPC_str *lpReturnedString, int nSize, const IPC_str *lpFileName )
{
    char str[PATH_MAX];
    ifstream ifs;
    int set_default = 1;

    ifs.open(lpFileName, ios::in);
    if( !ifs.is_open() ) {
        DEBUG_PRINT("Can't open file: %s. %s\n", FileName, strerror(errno));
        return -1;
    }

    while( !ifs.eof() ) {

        ifs.getline(str, sizeof(str), '\n');

        if( FindSection(str, lpAppName) == 0 ) {

            //DEBUG_PRINT("inside cycle\n");

            while( !ifs.eof() ) {

                ifs.getline(str, sizeof(str), '\n');

                if( FindOption(str, lpKeyName, lpReturnedString, nSize, &set_default) == 0) {
                    break;
                }
            }

            if(set_default) {
                memcpy(lpReturnedString, lpDefault, strlen(lpDefault)+1);
            }
        }
    }

    ifs.close();

    return 0;
}

//-----------------------------------------------------------------------------

int IPC_writePrivateProfileString( const IPC_str *lpAppName, const IPC_str *lpKeyName, const IPC_str *lpString, const IPC_str *lpFileName )
{
    return 0;
}

//-----------------------------------------------------------------------------

long IPC_interlockedDecrement( volatile long *val )
{
    long tmp = *val;
    *val = --tmp;
    return tmp;
}

//-----------------------------------------------------------------------------

long IPC_interlockedIncrement( volatile long *val )
{
    long tmp = *val;
    *val = ++tmp;
    return tmp;
}

//-----------------------------------------------------------------------------

long IPC_interlockedCompareExchange( volatile long *dst, long val, long param )
{
    long tmp = *dst;

    if(*dst == param) {
        *dst = val;
    }

    return tmp;
}

//-----------------------------------------------------------------------------

long IPC_interlockedExchange( volatile long *dst, long val )
{
    long tmp = *dst;
    *dst = val;
    return tmp;
}

//-----------------------------------------------------------------------------

long IPC_interlockedExchangeAdd( volatile long *dst, long val )
{
    long tmp = *dst;
    *dst += val;
    return tmp;
}

//-----------------------------------------------------------------------------

IPC_tls_key IPC_createTlsKey(void)
{
    pthread_key_t key;

    int res = pthread_key_create(&key, NULL);
    if(res != 0) {
        return (pthread_key_t)0;
    }

    return key;
}

//-----------------------------------------------------------------------------

void* IPC_tlsGetValue(IPC_tls_key key)
{
    return pthread_getspecific(key);
}

//-----------------------------------------------------------------------------

int IPC_tlsSetValue(IPC_tls_key key, void *ptr)
{
    return pthread_setspecific(key, ptr);
}

//-----------------------------------------------------------------------------

int IPC_deleteTlsKey(IPC_tls_key key)
{
     return pthread_key_delete(key);
}

//-----------------------------------------------------------------------------

const IPC_str* IPC_getCurrentDir(IPC_str *buf, int size)
{
    return getcwd(buf, size);
}

//-----------------------------------------------------------------------------

#endif //__IPC_LINUX__

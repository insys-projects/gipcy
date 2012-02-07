
#ifdef __IPC_LINUX__

#ifndef __LINIPC_H__
    #include "linipc.h"
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

static struct termios oldt;
static struct termios newt;

//-----------------------------------------------------------------------------

void IPC_initKeyboard(void)
{
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO | ISIG );
    newt.c_cc[VMIN]=0;
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
}

//-----------------------------------------------------------------------------

void IPC_cleanupKeyboard(void)
{
    oldt.c_lflag |= ( ICANON | ECHO | ISIG );
    oldt.c_cc[VMIN]=0;
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
}

//-----------------------------------------------------------------------------

int IPC_getch(void)
{
    int ch;

    ch = getchar();

    DEBUG_PRINT("%s(): ch = %d\n", __FUNCTION__, ch );

    return ch;
}

//-----------------------------------------------------------------------------

int IPC_kbhit(void)
{
    size_t size = 0;
    if (ioctl(STDIN_FILENO, FIONREAD, &size) == -1)
        return 0;

    DEBUG_PRINT("%s(): size = %d\n", __FUNCTION__, size );

    return size;
}

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
        return IPC_GENERAL_ERROR;
    return IPC_OK;
}

//-----------------------------------------------------------------------------

const IPC_str* IPC_getCurrentDir(IPC_str *buf, int size)
{
    return getcwd(buf, size);
}

//-----------------------------------------------------------------------------

static int convert_ipc_flags(int ipc_flags)
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

IPC_handle IPC_openFile(const IPC_str *name, int flags)
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

IPC_handle IPC_openFileEx(const IPC_str *name, int flags, int attr)
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
    if(!h) return IPC_INVALID_HANDLE;

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
    if(!h) return IPC_INVALID_HANDLE;

    int res = read(h->ipc_descr.ipc_file,data,size);
    if(res <= 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return -1;
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_writeFile(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

    int res = write(h->ipc_descr.ipc_file,data,size);
    if(res <= 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return -1;
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_setPosFile(IPC_handle handle, int pos, int method)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

	int res = lseek(h->ipc_descr.ipc_file,pos,method);

    return res;
}

//-----------------------------------------------------------------------------

int IPC_handleToFile(IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return -1;
    return h->ipc_descr.ipc_file;
}

//-----------------------------------------------------------------------------

int IPC_handleToDevice(IPC_handle handle)
{
    return IPC_handleToFile(handle);
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
                //DEBUG_PRINT("Section: < %s > was found in the string < %s >\n", section, src);
                return 0;
            }
            else
            {
                //DEBUG_PRINT("Section: < %s > was not found\n", section);
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
            //DEBUG_PRINT("Option: < %s > was found in the string < %s >\n", option, src);

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
                //DEBUG_PRINT("Option: < %s > was found in the string < %s >. But buffer to small\n", option, src);
                return -3;
            }

            *set_default = 0;

            return 0;
        }
        else
        {
            //DEBUG_PRINT("Option: < %s > was not found\n", option);
            return -4;
        }
    }
    return -5;
}

//-----------------------------------------------------------------------------
#include <fstream>
using namespace std;
//-----------------------------------------------------------------------------

int IPC_getPrivateProfileString( const IPC_str *lpAppName, const IPC_str *lpKeyName, const IPC_str *lpDefault,
                             IPC_str *lpReturnedString, int nSize, const IPC_str *lpFileName )
{
    char str[PATH_MAX];
    ifstream ifs;
    int set_default = 1;

    ifs.open(lpFileName, ios::in);
    if( !ifs.is_open() ) {
        DEBUG_PRINT("Can't open file: %s. %s\n", lpFileName, strerror(errno));
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

#endif //__IPC_LINUX__


#ifdef __IPC_QNX__

#ifndef __QNXIPC_H__
    #include "qnxipc.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>

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

    while(1)
    {
        size_t size = 0;
	if (ioctl(STDIN_FILENO, FIONREAD, &size) == -1)
    	    size = 0;
        if(size)
	{
	    ch = getchar();
		//read(0, &ch, 1);
	    DEBUG_PRINT("%s(): ch = %d\n", __FUNCTION__, ch );
	    break;
	}
    }
    return ch;
}

//-----------------------------------------------------------------------------

int IPC_getche(void)
{
    return IPC_getch();
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
/*
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
*/
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
            IPC_strlwr(name);

            char section_tmp[1024];
            strcpy(section_tmp, section);
            IPC_strlwr(section_tmp);

            if(!strcmp(name, section_tmp))
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

static int IsSection(const char* src)
{
    int nSize;

    if((src == 0) || (src[0] != '['))
        return 0;

    nSize = strlen(src);

    if(src[nSize - 1] != ']')
        return 0;

    return 1;
}

//-----------------------------------------------------------------------------

static int IsOptionName(const char* src)
{
    char *pStr;

    pStr = strstr((char *)src, "=");

    if(pStr == 0)
        return 0;

    *pStr = '\0';

    return 1;
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
        IPC_strlwr(name);

        char option_tmp[1024];
        strcpy(option_tmp, option);
        IPC_strlwr(option_tmp);

        if(!strcmp(name, option_tmp))
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

            char aSymb[6][3] = {"\n", "\r", "\t", ";", "#", "//"};

            for(int i = 0; i < 6; i++)
            {
                char *tmp;

                tmp = strstr(val, aSymb[i]);

                if(tmp)
                    tmp[0] = '\0';
            }

            if(BufferSize >= (int)(strlen(val)+1)) {
                strcpy( Buffer, val);
            } else {
                //DEBUG_PRINT("Option: < %s > was found in the string < %s >. But buffer to small\n", option, src);
                return -3;
            }

            while((Buffer[strlen(Buffer) - 1] == '\r') ||
                  (Buffer[strlen(Buffer) - 1] == '\n'))
                Buffer[strlen(Buffer) - 1] = '\0';

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
    int size;
    int sectionsSize = 0;
    int found = 0;

    ifs.open(lpFileName, ios::in);
    if( !ifs.is_open() ) {
        DEBUG_PRINT("Can't open file: %s. %s\n", lpFileName, strerror(errno));
        return IPC_BAD_INI_FILE;
    }

    while( !ifs.eof() && (!found)) {
        ifs.getline(str, sizeof(str), '\n');

        if(lpAppName == 0) {
            size = strlen(str);
            str[size] = '\0';

            if(IsSection(str)) {
                size --;
                str[size] = '\0';
                sectionsSize += size;

                if(sectionsSize > (nSize - 1))
                    break;

                memcpy(lpReturnedString, str + 1, size);
                lpReturnedString += size;
            }
        }
        else if( FindSection(str, lpAppName) == 0 ) {

            //DEBUG_PRINT("inside cycle\n");

            while( !ifs.eof() ) {

                ifs.getline(str, sizeof(str), '\n');

                if(lpKeyName == 0) {
                    size = strlen(str);
                    str[size] = '\0';

                    if(IsSection(str))
                        break;

                    if(IsOptionName(str)) {
                        size = strlen(str) + 1;
                        sectionsSize += size;

                        if(sectionsSize > (nSize - 1))
                            break;

                        memcpy(lpReturnedString, str, size);
                        lpReturnedString += size;
                    }
                }
                else if( FindOption(str, lpKeyName, lpReturnedString, nSize, &set_default) == 0) {
                    found = 1;
                    break;
                } else {
                    found = 0;
                }
            }

            if(set_default) {
                if(lpDefault) {
                    memcpy(lpReturnedString, lpDefault, strlen(lpDefault)+1);
                }
            }
        }
    }

    if((lpAppName == 0) || (lpKeyName == 0))
        *lpReturnedString = '\0';

    ifs.close();

    return found;
}

//-----------------------------------------------------------------------------

int IPC_writePrivateProfileString( const IPC_str *lpAppName, const IPC_str *lpKeyName, const IPC_str *lpString, const IPC_str *lpFileName )
{
    int nStart, nEnd, nSize;
    int isFindSection = 0;
    //int isFindParam = 0;
    char *pBuf, *pCur;
    char sLine[1024];
    char sParamName[1024];
    char sNextSection[1024];
    FILE *pFile;

    strcpy(sNextSection, "");

    pFile = fopen(lpFileName, "r");

    if(pFile == 0)
        return -1;

    // Вычисление размера буфера
    fseek(pFile, 0, SEEK_SET);
    nStart = ftell(pFile);
    fseek(pFile, 0, SEEK_END);
    nEnd = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);
    nSize = nEnd - nStart + 1024;

    pBuf = new char[nSize];
    pCur = pBuf;

    nSize = 0;

    // Поиск секции
    while(!feof(pFile))
    {
        strcpy(sLine, "");
        fgets(sLine, 1024, pFile);

        if(strlen(sLine) == 0)
            continue;

        strcpy(pCur, sLine);

        pCur += strlen(sLine);

        nSize += strlen(sLine);

        sLine[strlen(sLine) - 1] = '\0';

        if(!IsSection(sLine))
            continue;

        sLine[strlen(sLine) - 1] = '\0';
        strcpy(sLine, sLine + 1);

       if(strcmp(sLine, lpAppName) == 0)
       {
           isFindSection = 1;
           break;
       }
    }

    if(isFindSection)
    {   // Секция найдена, поиск параметра
        while(!feof(pFile))
        {
            strcpy(sLine, "");
            fgets(sLine, 1024, pFile);

            strcpy(sParamName, sLine);

            if(IsOptionName(sParamName))
            {   // Параметр
                if(strcmp(sParamName, lpKeyName) == 0)
                {   // Найден нужный параметр
                    //isFindParam = 1;
                    break;
                }
            }

            strcpy(sNextSection, sLine);
            sNextSection[strlen(sLine) - 1] = '\0';

            if(IsSection(sNextSection))
            {   // Следующая секция
                strcpy(sNextSection, sLine);
                break;
            }
            else
                strcpy(sNextSection, "");

            strcpy(pCur, sLine);

            pCur += strlen(sLine);

            nSize += strlen(sLine);
        }
    }

    if(!isFindSection)
    {   // Секция не найдена
        strcpy(sLine, "");
        strcpy(sLine, "[");
        strcat(sLine, lpAppName);
        strcat(sLine, "]\n");
        strcat(pCur, sLine);

        pCur += strlen(sLine);
        nSize += strlen(sLine);
    }

    // Добавление параметра
    strcpy(sLine, "");
    strcpy(sLine, lpKeyName);
    strcat(sLine, "=");
    strcat(sLine, lpString);
    strcat(sLine, "\n");
    strcat(sLine, sNextSection);
    strcat(pCur, sLine);

    pCur += strlen(sLine);
    nSize += strlen(sLine);

    // Чтение оставшихся строк
    while(!feof(pFile))
    {
        strcpy(sLine, "");
        fgets(sLine, 1024, pFile);

        strcpy(pCur, sLine);

        pCur += strlen(sLine);

        nSize += strlen(sLine);
    }

    fclose(pFile);

    // Запись в файл
    pFile = fopen(lpFileName, "w");

    if(pFile == 0)
        return -1;

    fwrite(pBuf, nSize, 1, pFile);
    fclose(pFile);

    // Удаление буфера
    delete [] pBuf;

    return 0;
}

//-----------------------------------------------------------------------------

void* IPC_heapAlloc(int nSize)
{
    void *ptr     = NULL;
    long pageSize = sysconf(_SC_PAGESIZE);
    int  res      = posix_memalign(&ptr, pageSize, nSize);

    if((res != 0) || !ptr)
        return 0;

    return ptr;
}

//-----------------------------------------------------------------------------

int IPC_heapFree(void *ptr)
{
    free(ptr);

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

IPC_str* IPC_itoa(int value, IPC_str* result, int base)
{
    if (base < 2 || base > 36) {
        *result = '\0'; return result;
    }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );


    if (tmp_value < 0)
        *ptr++ = '-';

    *ptr-- = '\0';

    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }

    return result;
}

//-----------------------------------------------------------------------------

GIPCY_API   int IPC_strlwr(char *str)
{
    int i;
    int nSize;

    if(str == 0)
        return 0;

    nSize = strlen(str);

    for(i = 0; i < nSize; i++)
        str[i] = tolower(str[i]);

    return 0;
}

//-----------------------------------------------------------------------------

unsigned int IPC_flushall()
{
    return  fflush(NULL);
}

//-----------------------------------------------------------------------------

long IPC_getTickCount()
{
    return time(NULL);
}

//-----------------------------------------------------------------------------

long IPC_getTickPerSec()
{
    return CLOCKS_PER_SEC;
}

#endif //__IPC_QNX__

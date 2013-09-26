#ifdef __IPC_QNX__

#ifndef __QNXIPC_H__
	#include "qnxipc.h"
#endif

#include <diripc.h>
#include <fnmatch.h>
#include <dirent.h>

typedef struct
{
	char sFind[256];
	DIR  *pDir;
} TIpcDir;

#include <string.h>

IPC_handle IPC_OpenDir(const IPC_str *sNameFilter, const IPC_str *sDirPath)
{
    ipc_handle_t h = allocate_ipc_object("", IPC_typeDir);
    DIR     *pDir;
	TIpcDir *pIpcDir;

    pDir = opendir(sDirPath);

    if(!pDir)
        return 0;

    pIpcDir = new TIpcDir;

     pIpcDir->pDir = pDir;
 
     strcpy(pIpcDir->sFind, sNameFilter);
 
 	h->ipc_data = (void *)pIpcDir;

    return h;
}

void IPC_CloseDir(IPC_handle handle)
{
	ipc_handle_t h = (ipc_handle_t)handle;
	TIpcDir *pIpcDir = (TIpcDir *)h->ipc_data;

    closedir(pIpcDir->pDir);

    delete pIpcDir;
}

// Поиск файла
int IPC_FindFile(IPC_handle handle, const IPC_str *sFindFile)
{
	ipc_handle_t h = (ipc_handle_t)handle;
	TIpcDir *pIpcDir = (TIpcDir *)h->ipc_data;
    dirent *pDirEnt;
    int isFindFile = 0;

    pDirEnt = readdir(pIpcDir->pDir);

    while(pDirEnt)
    {
 #warning QNX attention
/*    
        if((pDirEnt->d_type == DT_DIR) || (pDirEnt->d_type == DT_LNK))
        {   // Запись не является файлом
            pDirEnt = readdir(pIpcDir->pDir);
		
            continue;
        }
*/
        if(fnmatch(pIpcDir->sFind, pDirEnt->d_name, 0))
        {
            pDirEnt = readdir(pIpcDir->pDir);

            continue;
        }

        // Файл найден
        isFindFile = 1;

        break;
    }

    if(!isFindFile)
        // Файл не найден
        return -1;

    strcpy((char *)sFindFile, pDirEnt->d_name);

    return 0;
}

// Поиск файлов
// sNameFilter	- маска поиска
// sDirPath		- путь к папке, в которой будет осуществляться поиск
// asEntries	- указатель на массив найденных папок и файлов
// nEntrCount	- размер массива asEntries
// pAllCount	- параметр возвращает фактическое число найденных папок и файлов
int IPC_FindFiles(const IPC_str *sNameFilter, const IPC_str *sDirPath, const IPC_str (*asEntries)[256], int nEntrCount, int *pAllCount)
{
    int i = 0;
    IPC_handle handle;
    char sFindFile[256];

    handle = IPC_OpenDir(sNameFilter, sDirPath);

    if(handle == 0)
        return -1;

    while(!IPC_FindFile(handle, sFindFile))
    {
        strcpy((char *)asEntries[i], sFindFile);
        i++;
    }

    *pAllCount = i;

    IPC_CloseDir(handle);

    return 0;
}

#endif // __IPC_LINUX__

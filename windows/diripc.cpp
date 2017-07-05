#ifdef __IPC_WIN__

#ifndef __WINIPC_H__
	#include "winipc.h"
#endif
#ifndef __DIRIPC_H__
	#include "diripc.h"
#endif

// Является ли запись символьной ссылкой
#ifdef _UNICODE
int IsSymLinks(const IPC_str *sName)
{
	IPC_str *p = (IPC_str *)wcsstr(sName, L".lnk"); 

	if(!p)
		return 0;

	if(wcslen(p) == 4)
		return 1;

	return 0;
}
#else
int IsSymLinks(const IPC_str *sName)
{
	IPC_str *p = (IPC_str *)strstr(sName, ".lnk"); 
	
	if(!p)
		return 0;

	if(strlen(p) == 4)
		return 1;

	return 0;
}
#endif

#ifdef _UNICODE
GIPCY_API	IPC_handle IPC_OpenDir(const IPC_str *sNameFilter, const IPC_str *sDirPath)
{
	IPC_str sPath[1024];
	WIN32_FIND_DATA *pFindData;
	ipc_handle_t h = allocate_ipc_object(L"", IPC_typeDir);

	if(!h)
		return NULL;

	wcscpy(sPath, sDirPath);
	wcscat(sPath, L"\\");
	wcscat(sPath, sNameFilter);

	h->ipc_descr = FindFirstFile(sPath, pFindData);

	if(h->ipc_descr == INVALID_HANDLE_VALUE)
		return NULL;

	h->ipc_data = pFindData;

	return h;
}
#else
GIPCY_API	IPC_handle IPC_OpenDir(const IPC_str *sNameFilter, const IPC_str *sDirPath)
{
	IPC_str sPath[1024];
	WIN32_FIND_DATA *pFindData = new WIN32_FIND_DATA;
	ipc_handle_t h = allocate_ipc_object("", IPC_typeDir);

	if(!h)
		return NULL;

	strcpy(sPath, sDirPath);
	strcat(sPath, "\\");
	strcat(sPath, sNameFilter);

	h->ipc_descr = FindFirstFile(sPath, pFindData);

	if(h->ipc_descr == INVALID_HANDLE_VALUE)
		return NULL;

	h->ipc_data = pFindData;

	return h;
}
#endif

GIPCY_API	void IPC_CloseDir(IPC_handle handle)
{
	ipc_handle_t h = (ipc_handle_t)handle;

	FindClose(h->ipc_descr);
}

#ifdef _UNICODE
GIPCY_API	int IPC_FindFile(IPC_handle handle, const IPC_str *sFindFile)
{
	ipc_handle_t h = (ipc_handle_t)handle;
	WIN32_FIND_DATA *pFindData = (WIN32_FIND_DATA *)h->ipc_data;
	int isFindFile = 0;

	do
	{
		if(pFindData == 0)
		{
			pFindData = new WIN32_FIND_DATA;
			continue;
		}

		if(pFindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			// Запись не является файлом
			continue;

		if(IsSymLinks(pFindData->cFileName))
			// Игнорируем символьные ссылки
			continue;

		// Файл найден
		isFindFile = 1;

		break;
	} while(FindNextFile(h->ipc_descr, pFindData));

	if(h->ipc_data)
		h->ipc_data = 0;

	if(!isFindFile)
	{	// Файл не найден	
		delete pFindData;
		return -1;
	}

	wcscpy((IPC_str*)sFindFile, pFindData->cFileName);

	delete pFindData;

	return 0;
}
#else
GIPCY_API	int IPC_FindFile(IPC_handle handle, const IPC_str *sFindFile)
{
	ipc_handle_t h = (ipc_handle_t)handle;
	WIN32_FIND_DATA *pFindData = (WIN32_FIND_DATA *)h->ipc_data;
	int isFindFile = 0;

	do
	{
		if(pFindData == 0)
		{
			pFindData = new WIN32_FIND_DATA;
			continue;
		}

		if(pFindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			// Запись не является файлом
			continue;

		if(IsSymLinks(pFindData->cFileName))
			// Игнорируем символьные ссылки
			continue;

		// Файл найден
		isFindFile = 1;

		break;
	} while(FindNextFile(h->ipc_descr, pFindData));

	if(!isFindFile)
	{	// Файл не найден	
		delete pFindData;

		if(h->ipc_data)
			h->ipc_data = 0;

		return -1;
	}

	strcpy((char *)sFindFile, pFindData->cFileName);

	delete pFindData;

	if(h->ipc_data)
		h->ipc_data = 0;

	return 0;
}
#endif

#ifdef _UNICODE
GIPCY_API	int IPC_FindFiles(const IPC_str *sNameFilter, const IPC_str *sDirPath, const IPC_str (*asEntries)[256], int nEntrCount, int *pAllCount)
{
	int i = 0;
	IPC_handle h;
	IPC_str sFindFile[256];

	h = IPC_OpenDir(sNameFilter, sDirPath);

	if(h == NULL)
		return -1;

	while(!IPC_FindFile(h, sFindFile))
	{
		wcscpy((IPC_str*)asEntries[i], sFindFile);
		i++;
	}

	*pAllCount = i;

	IPC_CloseDir(h);

	return 0;
}
#else
GIPCY_API	int IPC_FindFiles(const IPC_str *sNameFilter, const IPC_str *sDirPath, const IPC_str (*asEntries)[256], int nEntrCount, int *pAllCount)
{
	int i = 0;
	IPC_handle h;
	IPC_str sFindFile[256];

	h = IPC_OpenDir(sNameFilter, sDirPath);

	if(h == NULL)
		return -1;

	while(!IPC_FindFile(h, sFindFile))
	{
		strcpy((char *)asEntries[i], sFindFile);
		i++;
	}

	*pAllCount = i;

	IPC_CloseDir(h);
	
	return 0;
}
#endif

#endif //__IPC_WIN__
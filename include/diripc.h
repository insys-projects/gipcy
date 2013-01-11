#ifndef __DIRIPC_H__
#define __DIRIPC_H__

#ifndef __IPCLIB_H__
	#include "ipclib.h"
#endif

//----------------------------------------------------------------------
// Функции для поиска файлов 
//----------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

	//! Функция открывает разделяемую библиотеку c дополнительными параметрами
    /*!
    \sNameFilter - маска поиска
    \sDirPath - путь к папке, в которой будет осуществляться поиск
	\return в случае ошибки возвращает NULL
    */
GIPCY_API	IPC_handle IPC_OpenDir(const IPC_str *sNameFilter, const IPC_str *sDirPath);

	//! Функция открывает разделяемую библиотеку c дополнительными параметрами
    /*!
    \handle - дескриптор директории
    */
GIPCY_API	void IPC_CloseDir(IPC_handle handle);

	//! Функция осуществляет поиск файла
    /*!
    \handle - дескриптор директории
	\sFindFile - найденный файл
	\return в случае ошибки возвращает -1
    */
GIPCY_API	int IPC_FindFile(IPC_handle handle, const IPC_str *sFindFile);

	//! Функция осуществляет поиск файлов
    /*!
	\sNameFilter - маска поиска
	\sDirPath - путь к папке, в которой будет осуществляться поиск
    \asEntries - массив найденных файлов
	\nEntrCount - размер массива asEntries
	\pAllCount - количество найденных файлов
	\return в случае ошибки возвращает -1
    */
GIPCY_API	int IPC_FindFiles(const IPC_str *sNameFilter, const IPC_str *sDirPath, const IPC_str (*asEntries)[256], int nEntrCount, int *pAllCount);

#ifdef __cplusplus
};
#endif

#endif //__DIRIPC_H__

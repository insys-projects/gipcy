
#ifndef __STRIPC_H__
#define __STRIPC_H__

//----------------------------------------------------------------------
// Функции и типы для работы со строками
//----------------------------------------------------------------------

//! Определим тип указателя на строку,
//! чтобы исключить директивы препроцессора
//! в интерфейсных функциях

#ifdef __linux__
    typedef char           IPC_str;
    #define DIR_DELIMITER "/"
#else
    #ifdef _WIN64
        typedef wchar_t    IPC_str;
        #define DIR_DELIMITER L"\\"
    #else
        typedef char       IPC_str;
        #define DIR_DELIMITER "\\"
    #endif
#endif

#endif //__STRIPC_H__


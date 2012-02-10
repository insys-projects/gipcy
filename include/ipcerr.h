// IPC Error Code Definitions

#ifndef __IPCERR_H__
#define __IPCERR_H__

//----------------------------------------------------------------------
//
//	IPC_OK	
//
//  No Error
//
#define	IPC_OK						0x00000000L


//======================================================================
// Errors !
//======================================================================
//
//	IPC_GENERAL_ERROR	
//
//  Wrong Handle
//
#define	IPC_GENERAL_ERROR			0xC0000100L

//----------------------------------------------------------------------
//
//	IPC_INVALID_HANDLE	
//
//  Wrong Handle
//
#define	IPC_INVALID_HANDLE			0xC0000101L

//----------------------------------------------------------------------
//
//	IPC_WAIT_TIMEOUT	
//
//  WaitForXXXObject returned WAIT_TIMEOUT
//
#define	IPC_WAIT_TIMEOUT			0xC0000102L

//----------------------------------------------------------------------
//
//	IPC_NOT_ENOUGH_MEMORY
//
//  Not enough storage is available to process this command.
//
#define	IPC_NOT_ENOUGH_MEMORY		0xC0000103L

//----------------------------------------------------------------------
//
//	IPC_BAD_INI_FILE	
//
//  Error: Can't open INI file
//
#define	IPC_BAD_INI_FILE			0xC0000104L

//======================================================================
// Warnings !
//======================================================================
//
//	IPC_WAIT_ABANDONED	
//
//  WaitForXXXObject returned WAIT_ABANDONED
//
#define	IPC_WAIT_ABANDONED			0x40000400L

//----------------------------------------------------------------------

//void IPC_errorString(IPC_error err);

//----------------------------------------------------------------------

#endif // __IPCERR_H__

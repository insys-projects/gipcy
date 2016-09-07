// IPC Error Code Definitions

#ifndef __IPCERR_H__
#define __IPCERR_H__

//----------------------------------------------------------------------
//
//	IPC_OK	
//
//  No Error
//
#define	IPC_OK						0x00000000


//======================================================================
// Errors !
//======================================================================
//
//	IPC_GENERAL_ERROR	
//
//  Wrong Handle
//
#define	IPC_GENERAL_ERROR			0xC0000100

//----------------------------------------------------------------------
//
//	IPC_INVALID_HANDLE	
//
//  Wrong Handle
//
#define	IPC_INVALID_HANDLE			0xC0000101

//----------------------------------------------------------------------
//
//	IPC_WAIT_TIMEOUT	
//
//  WaitForXXXObject returned WAIT_TIMEOUT
//
#define	IPC_WAIT_TIMEOUT			0xC0000102

//----------------------------------------------------------------------
//
//	IPC_NOT_ENOUGH_MEMORY
//
//  Not enough storage is available to process this command.
//
#define	IPC_NOT_ENOUGH_MEMORY			0xC0000103

//----------------------------------------------------------------------
//
//	IPC_BAD_INI_FILE	
//
//  Error: Can't open INI file
//
#define	IPC_BAD_INI_FILE			0xC0000104

//----------------------------------------------------------------------
//
//	IPC_DRIVER_NOT_LOADED	
//
//  Error: Can't open /dev/ipc_driver file
//
#define	IPC_DRIVER_NOT_LOADED			0xC0000105

//======================================================================
// Warnings !
//======================================================================
//
//	IPC_WAIT_ABANDONED	
//
//  WaitForXXXObject returned WAIT_ABANDONED
//
#define	IPC_WAIT_ABANDONED			0x40000400

//----------------------------------------------------------------------

//void IPC_errorString(IPC_error err);

//----------------------------------------------------------------------

#endif // __IPCERR_H__

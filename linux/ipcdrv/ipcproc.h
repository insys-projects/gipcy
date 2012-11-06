
#ifndef __IPCPROC_H__
#define __IPCPROC_H__

int ipc_proc_info(char *buf,
                  char **start,
                  off_t off,
                  int count,
                  int *eof,
                  void *data );
void ipc_register_proc(char *name, void *fptr, void *data);
void ipc_remove_proc(char *name);

#endif

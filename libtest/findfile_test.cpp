#include "gipcy.h"
#include <stdio.h>

int main()
{
	char asEntries[100][256];
	char asEntries1[100][256];
	int nAllCount = 0;

	IPC_handle handle;

	handle = IPC_OpenDir("*find*", ".");

	int i = 0;

	printf("Find File:\n");

	while(IPC_FindFile(handle, asEntries[i]) != -1)
	{
		printf("%s\n", asEntries[i]);
		i++;
	}

	IPC_CloseDir(handle);

	printf("\nFind Files:\n");

	IPC_FindFiles("*find*", ".", asEntries1, 100, &nAllCount);

	for(i = 0; i < nAllCount; i++)
		printf("%s\n", asEntries1[i]);

	getchar();

	return 0;
}
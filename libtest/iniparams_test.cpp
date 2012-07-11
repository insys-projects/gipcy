#include "gipcy.h"
#include <stdio.h>
#include <string.h>

#define BUFSIZE 16384

int main()
{
    IPC_str sSections[BUFSIZE];
	IPC_str sParams[BUFSIZE];
	IPC_str sValue[BUFSIZE];
	IPC_str *pSections;
	IPC_str *pParams;
	IPC_str sIniFile[4096];

	IPC_getCurrentDir(sIniFile, 4096);
    strcat(sIniFile, "/iniparams_test.ini");

    IPC_getPrivateProfileString(0, 0, "", sSections, BUFSIZE, sIniFile);

	pSections = sSections;

	while(strlen(pSections) != 0)
	{
		printf("%s\n", pSections);
		
        strcpy(sParams, "");
        IPC_getPrivateProfileString(pSections, 0, "", sParams, BUFSIZE, sIniFile);
        pParams = sParams;

        while(strlen(pParams) != 0)
        {
            strcpy(sValue, "");
            IPC_getPrivateProfileString(pSections, pParams, "", sValue, BUFSIZE, sIniFile);
            printf("%s = %s\n", pParams, sValue);
            pParams += strlen(pParams) + 1;
        }

		printf("\n");

		pSections += strlen(pSections) + 1;
	}

	return 0;
}

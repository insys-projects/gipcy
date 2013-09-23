#include "gipcy.h"
#include <stdio.h>
#include <string.h>

#define BUFSIZE 16384

IPC_str g_sIniFile[4096];

// Вывод содержимого ini-файла
void PrintfIniFile();

int main()
{
    IPC_getCurrentDir(g_sIniFile, 4096);
    strcat(g_sIniFile, "/iniparams_test.ini");

    printf("Before change.\n\n");
    PrintfIniFile();

    IPC_writePrivateProfileString("Section1", "D1", "14", g_sIniFile);
    IPC_writePrivateProfileString("Section2", "D2", "24", g_sIniFile);
    IPC_writePrivateProfileString("Section3", "D3", "34", g_sIniFile);
    IPC_writePrivateProfileString("Section4", "E",  "55", g_sIniFile);

    printf("After change.\n\n");
    PrintfIniFile();

	return 0;
}

void PrintfIniFile()
{
    IPC_str sSections[BUFSIZE];
    IPC_str sParams[BUFSIZE];
    IPC_str sValue[BUFSIZE];
    IPC_str *pSections;
    IPC_str *pParams;

    IPC_getPrivateProfileString(0, 0, "", sSections, BUFSIZE, g_sIniFile);

    pSections = sSections;

    while(strlen(pSections) != 0)
    {
        printf("%s\n", pSections);

        strcpy(sParams, "");
        IPC_getPrivateProfileString(pSections, 0, "", sParams, BUFSIZE, g_sIniFile);
        pParams = sParams;

        while(strlen(pParams) != 0)
        {
            strcpy(sValue, "");
            IPC_getPrivateProfileString(pSections, pParams, "", sValue, BUFSIZE, g_sIniFile);
            printf("%s = %s\n", pParams, sValue);
            pParams += strlen(pParams) + 1;
        }

        printf("\n");

        pSections += strlen(pSections) + 1;
    }

    strcpy(sSections, "SECTION3");
    printf("%s\n", sSections);

    strcpy(sParams, "a3");
    strcpy(sValue, "");
    IPC_getPrivateProfileString(sSections, sParams, "", sValue, BUFSIZE, g_sIniFile);
    printf("%s = %s\n", sParams, sValue);

    strcpy(sParams, "b3");
    strcpy(sValue, "");
    IPC_getPrivateProfileString(sSections, sParams, "", sValue, BUFSIZE, g_sIniFile);
    printf("%s = %s\n", sParams, sValue);

    strcpy(sParams, "c3");
    strcpy(sValue, "");
    IPC_getPrivateProfileString(sSections, sParams, "", sValue, BUFSIZE, g_sIniFile);
    printf("%s = %s\n\n", sParams, sValue);
}

// chgdt.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>

int showUsage(char * pgm) {
    printf("Milliseconds indicate or modify the system time.\n\n");
    printf("%s [/t | time]\n\n", pgm);
    printf("When no parameter is specified, only the system time is displayed.\n\n");
    printf("/tModify the system time.\n\n");
    printf("YYYY/MM/DD HH:MI:SS.sss。\n\n");
    printf("Administrator authority is required to modify the system time.\n\n");
    return -1;
}

int main(int argc, char * argv[])
{
    /*
     * 0:只是显示chgdt
     * 1:只变更日期chgdt 2020/05/22
     * 2:只变更时刻chgdt 11:50:40.999
     * 3:日期时刻同时变更chgdt "2020/05/22 12:51:00.998"
     */
    int disFlag = 0;
    int i = 0;
#ifdef _DEBUG
    printf("[DEBUG]%d\n", argc);
#endif
    if (argc >= 2) {
        for (i = 1; i < argc; i ++) {
            int iLen = strlen(argv[i]);
            if (iLen > 2) {
                iLen = 2;
            }
            if (memcmp("/t", argv[i], iLen) == 0 || memcmp("/T", argv[i], iLen) == 0) {
                disFlag = 1;
                break;
            }
            if (memcmp("/?", argv[i], iLen) == 0) {
                return showUsage(argv[0]);
            }
        }
    } // if
    SYSTEMTIME newst;
    ZeroMemory(&newst, sizeof(SYSTEMTIME));
    char tmp[5];
    int x = 0;
    ZeroMemory(tmp, sizeof(tmp));
    int type = -1; // -1:init,0:year,1:month,2:day,3:hour,4:minute,5:second,6:millisecond
    if (disFlag == 0 && argc >= 2) {
        // date
        x = 0;
        for (i = 1; i < argc; i ++) {
#ifdef _DEBUG
            printf("[DEBUG]argv[%d]=%s\n", i, argv[i]);
#endif
            int j = 0;
            while (argv[i][j] != '\0') {
#ifdef _DEBUG
                printf("[DEBUG]type=%d.i=%d,j=%d,x=%d.\n", type, i, j, x);
#endif
                if (argv[i][j] == '"') {
                    j++;
                    continue;
                }
                if (type == 4 && argv[i][j] == '.') {
                    type = 5;
                    tmp[x] = '\0';
#ifdef _DEBUG
                    printf("[DEBUG]wSecondtmp=%s.\n", tmp);
#endif
                    newst.wSecond = (WORD)atol(tmp);
                    x = 0;
                    ZeroMemory(tmp, sizeof(tmp));
                    j++;
                    continue;
                }
                if (type == 3 && argv[i][j] == ':') {
                    type = 4;
                    tmp[x] = '\0';
#ifdef _DEBUG
                    printf("[DEBUG]wMinutetmp=%s.\n", tmp);
#endif
                    newst.wMinute = (WORD)atol(tmp);
                    x = 0;
                    ZeroMemory(tmp, sizeof(tmp));
                    j++;
                    continue;
                }
                if ((type == -1 || type == 2) && argv[i][j] == ':') {
                    type = 3;
                    tmp[x] = '\0';
#ifdef _DEBUG
                    printf("[DEBUG]wHourtmp=%s.\n", tmp);
#endif
                    newst.wHour = (WORD)atol(tmp);
                    x = 0;
                    ZeroMemory(tmp, sizeof(tmp));
                    disFlag = disFlag | 0x02;
                    j++;
                    continue;
                }
                if (type == 1 && argv[i][j] == ' ') {
                    type = 2;
                    tmp[x] = '\0';
#ifdef _DEBUG
                    printf("[DEBUG]wDaytmp=%s.\n", tmp);
#endif
                    newst.wDay = (WORD)atol(tmp);
                    x = 0;
                    ZeroMemory(tmp, sizeof(tmp));
                    j++;
                    continue;
                }
                if (type == 0 && argv[i][j] == '/') {
                    type = 1;
                    tmp[x] = '\0';
#ifdef _DEBUG
                    printf("[DEBUG]wMonthtmp=%s.\n", tmp);
#endif
                    newst.wMonth = (WORD)atol(tmp);
                    x = 0;
                    ZeroMemory(tmp, sizeof(tmp));
                    j++;
                    continue;
                }
                if (type == -1 && argv[i][j] == '/') {
                    type = 0;
                    tmp[x] = '\0';
#ifdef _DEBUG
                    printf("[DEBUG]wYeartmp=%s.\n", tmp);
#endif
                    newst.wYear = (WORD)atol(tmp);
                    x = 0;
                    ZeroMemory(tmp, sizeof(tmp));
                    disFlag = disFlag | 0x01;
                    j++;
                    continue;
                }
                if (x < 4) {
                    tmp[x] = argv[i][j];
                }
                x++;
                j++;
            }
#ifdef _DEBUG
            printf("[DEBUG]type=%d,i=%d,j=%d,x=%d.\n", type, i, j, x);
#endif
            if (type == 1) {
                type = 2;
                tmp[x] = '\0';
#ifdef _DEBUG
                printf("[DEBUG]wDaytmp=%s.\n", tmp);
#endif
                newst.wDay = (WORD)atol(tmp);
                x = 0;
                ZeroMemory(tmp, sizeof(tmp));
            }
            if (type == 5) {
                type = 6;
                tmp[x] = '\0';
#ifdef _DEBUG
                printf("[DEBUGwMillisecondstmp=%s.\n", tmp);
#endif
                newst.wMilliseconds = (WORD)atol(tmp);
                x = 0;
                ZeroMemory(tmp, sizeof(tmp));
            }
        }
    }
#ifdef _DEBUG
    printf("[DEBUG]:disFlag=%d.\n", disFlag);
#endif
    SYSTEMTIME st;
    ZeroMemory(&st, sizeof(SYSTEMTIME));
    GetLocalTime(&st);

    if (disFlag != 0) {
        printf("System time before change： %04d/%02d/%02d %02d:%02d:%02d.%03d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
        DWORD errCd = 0;
        if (disFlag == 1) {
            newst.wHour = st.wHour;
            newst.wMinute = st.wMinute;
            newst.wSecond = st.wSecond;
            newst.wMilliseconds = st.wMilliseconds;
        }
        if (disFlag == 2) {
            newst.wYear = st.wYear;
            newst.wMonth = st.wMonth;
            newst.wDay = st.wDay;
        }
#ifdef _DEBUG
        printf("[DEBUG]:%04d/%02d/%02d %02d:%02d:%02d.%03d\n", newst.wYear, newst.wMonth, newst.wDay, newst.wHour, newst.wMinute, newst.wSecond, newst.wMilliseconds);
#endif
        if (SetLocalTime(&newst) == 0) {
            errCd = GetLastError();
            LPVOID lpMsgBuf = NULL;
            FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, errCd, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR)&lpMsgBuf, 0, NULL);
            printf("%sIn the process of setting the system time, an error occurred.\n error %d:%s\n", argv[0], errCd, (lpMsgBuf == NULL ? "" : (LPSTR) lpMsgBuf));
            LocalFree(lpMsgBuf);
            return -1;
        }
        ZeroMemory(&st, sizeof(SYSTEMTIME));
        GetLocalTime(&st);
        printf("System time after change： %04d/%02d/%02d %02d:%02d:%02d.%03d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    }
    else {
        printf("Current system time：%04d/%02d/%02d %02d:%02d:%02d.%03d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    }
	return 0;
}


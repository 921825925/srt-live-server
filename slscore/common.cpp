/*
 * This file is part of SLS Live Server.
 *
 * SLS Live Server is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SLS Live Server is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SLS Live Server;
 * if not, please contact with the author: Edward.Wu(edward_email@126.com)
 */

#include <string>
#include <vector>
#include <cstdarg>
#include <sys/time.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>


#include "common.hpp"

/**
* sls_format
 */
std::string sls_format(const char *pszFmt, ...)
{
    std::string str;
    /*
    va_list args;
    va_start(args, pszFmt);
    {
        int nLength = vscprintf(pszFmt, args);
        nLength += 1;  //include \0
        std::vector<char> vectorChars(nLength);
        vsnprintf(vectorChars.data(), nLength, pszFmt, args);
        str.assign(vectorChars.data());
    }
    va_end(args);
    */
    return str;
}

#define HAVE_GETTIMEOFDAY 1

int64_t sls_gettime(void)//rturn micro-second
{
#if HAVE_GETTIMEOFDAY
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
#elif HAVE_GETSYSTEMTIMEASFILETIME
    FILETIME ft;
    int64_t t;
    GetSystemTimeAsFileTime(&ft);
    t = (int64_t)ft.dwHighDateTime << 32 | ft.dwLowDateTime;
    return t / 10 - 11644473600000000; /* Jan 1, 1601 */
#else
    return -1;
#endif
}

int64_t sls_gettime_relative(void)
{
#if HAVE_CLOCK_GETTIME && defined(CLOCK_MONOTONIC)
#ifdef __APPLE__
    if (clock_gettime)
#endif
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (int64_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
    }
#endif
    return sls_gettime() + 42 * 60 * 60 * INT64_C(1000000);
}


void sls_gettime_fmt(char *dst, int64_t cur_time_sec, char *fmt)
{
    time_t rawtime;
    struct tm * timeinfo;
    char timef[32] = {0};

    time (&rawtime);
    rawtime = (time_t)cur_time_sec;
    timeinfo = localtime (&rawtime);
    strftime(timef, sizeof(timef), fmt, timeinfo);
    strcpy(dst, timef);
    return ;
}

char * sls_strupper(char * str)
{
    char *orign=str;
    for (; *str!='\0'; str++)
        *str = toupper(*str);
    return orign;
}

#define sls_hash(key, c)   ((uint32_t) key * 31 + c)
uint32_t sls_hash_key(const char *data, int len)
{
	//copy form ngx
    uint32_t  i, key;

    key = 0;

    for (i = 0; i < len; i++) {
        key = sls_hash(key, data[i]);
    }
    return key;
}

int sls_gethostbyname(const char *hostname, char *ip)
{
    char   *ptr, **pptr;
    struct hostent *hptr;
    char   str[32];
    ptr = (char *)hostname;

    if((hptr = gethostbyname(ptr)) == NULL)
     {
         printf("sls_gethostbyname: gethostbyname error for host:%s\n", ptr);
         return 0;
     }

/*
    printf("official hostname:%s\n",hptr->h_name);
     for(pptr = hptr->h_aliases; *pptr != NULL; pptr++)
         printf(" alias:%s\n",*pptr);
*/

     switch(hptr->h_addrtype)
     {
         case AF_INET:
         case AF_INET6:
             //pptr=hptr->h_addr_list;
             //for(; *pptr!=NULL; pptr++)
             //    printf(" address:%s\n",
             //           inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));

        	 //copy the 1st ip
             strcpy(ip, inet_ntop(hptr->h_addrtype, hptr->h_addr, str, sizeof(str)));
         break;
         default:
             printf("sls_gethostbyname: unknown address type\n");
         break;
     }

     return 0;
 }

#pragma once

#include <stdio.h>  
#include <time.h>  
#include <stdint.h>  

#ifdef _WIN32  
#include <Windows.h>  
#else  
#include <unistd.h>  
#endif  

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS) || defined(__WATCOMC__)
#define DELTA_EPOCH_IN_USEC	11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_USEC	11644473600000000ULL
#endif

typedef unsigned __int64 u_int64_t;

int gettimeofday(struct timeval *tv, void *tz);
long long tvdiff_us(timeval * tv1, timeval * tv2);
static u_int64_t filetime_to_unix_epoch(const FILETIME *ft);


#pragma once

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <x86intrin.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "slassert.h"
typedef void (*caller)();
typedef float                   FLOAT32;
typedef double                  FLOAT64;
typedef signed char             INT8;
typedef signed short            INT16;
typedef signed int              INT32;
typedef signed long long int    INT64;
typedef unsigned char           BYTE;
typedef unsigned short          WORD;
typedef unsigned long long      QWORD;

#ifndef _WINDOWS_
typedef uint32_t  DWORD;
#endif /* _WINDOWS_ */
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

typedef size_t          *GNERICPTR, GNERICPTR_NORMALIZED;
typedef char Bool;

typedef struct _slBIT8 {
    BYTE b0 : 1;
    BYTE b1 : 1;
    BYTE b2 : 1;
    BYTE b3 : 1;
    BYTE b4 : 1;
    BYTE b5 : 1;
    BYTE b6 : 1;
    BYTE b7 : 1;
} bit8, BIT8;

typedef struct _slRATIONAL {
    DWORD numerator;
    DWORD denominator; 
} RATIONAL;

typedef struct _slSegmentationFault {
    DWORD *exception;
} SegmentationFault;
#define CallSegmentationFault() ((SegmentationFault *)0x0)->exception++

#if defined( DEBUG ) || defined(_DEBUG)
#define CRC32_GENERATOR_POLYNOMIAL 0x04C11DB7
DWORD slCyclicRedundanceCheck32(const BYTE *message, INT32 length) {
    INT32 i, j;
    DWORD CRC, MSB;

    CRC  =  ~0;
    for (i = 0; i < length; i++)
    {
        CRC ^= (((DWORD)message[i]) << 24);
        for (j = 0; j < 8; j++)
        {
            MSB = CRC >> 31;
            CRC <<= 1;
            CRC ^= (0 - MSB) & CRC32_GENERATOR_POLYNOMIAL;
        }
    }
    
    return CRC;
}

#define _slAllocatedMemoryDebug(ptr, _Type, size) do {\
    (ptr) = (_Type)malloc((size) + (sizeof(DWORD) * 2));\
    *((DWORD *)ptr) = (size);\
    ptr = (_Type)(((DWORD *)ptr) + 1);\
    *((DWORD *)(ptr + size)) = slCyclicRedundanceCheck32((const BYTE *)(((DWORD *)ptr) - 1), sizeof(DWORD));\
} while (0)

#define _slReleaseAllocatedMemoryDebug(ptr) do {\
    if ((slCyclicRedundanceCheck32((const BYTE *)(((DWORD *)ptr) - 1), sizeof(DWORD)) ^ *((DWORD *)(ptr + *(((DWORD *)ptr) - 1)))))\
    {\
        slLogMessage(__FILE__, __LINE__, __DATE__, __TIME__, Memory Overflow);\
        CallSegmentationFault();\
    }\
    free (((DWORD *)ptr) - 1);\
} while (0)

#endif

#define slLogMessage(FILE, LINE, DATE, TIME, MESSAGE) fprintf(stderr,\
    "\033[0;31mError:\033[0m \n"\
    "File Name: %s\n"\
    "Line Number: %d\n"\
    "Date: %s %s\n"\
    "Exception Details: \033[0;31m"\
    #MESSAGE "\033[0m\n",\
    (FILE),\
    (LINE),\
    (DATE),\
    (TIME))

#define slAddressOfPointer(ptr) *(void **)(&(ptr))
#define slAddressOfValue(variable) (void *)(&(variable))

#define _slAllocateMemory(ptr, _Type, size) do { (ptr) = (_Type)malloc((size)); } while (0)
#define _slReleaseAllocatedMemory(ptr) do { if ((ptr)) { free(ptr); (ptr) = NULL; } } while (0)

#if !defined( DEBUG ) && !defined(_DEBUG)
#define slAllocateMemory(ptr, _Type, size) _slAllocateMemory(ptr, _Type, size)
#define slReleaseAllocatedMemory(ptr) _slReleaseAllocatedMemory(ptr)
#else
#define slAllocateMemory(ptr, _Type, size) _slAllocatedMemoryDebug(ptr, _Type, size)
#define slReleaseAllocatedMemory(ptr) _slReleaseAllocatedMemoryDebug(ptr)
#endif /* DEBUG*/

#endif

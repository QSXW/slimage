#pragma once

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <x86intrin.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "slassert.h"

typedef void (*caller)();
typedef int8_t INT8;
typedef int16_t INT16;
typedef int32_t INT32;
typedef int64_t INT64;
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint64_t QWORD;
typedef size_t *GNERICPTR;
typedef size_t GNERICPTR_NORMALIZED;
typedef char Bool;

#ifndef _WINDOWS_
typedef uint32_t DWORD;
#endif /* _WINDOWS_ */
#if !defined( FALSE )
#define FALSE 0
#endif
#if !defined( TRUE )
#define TRUE 1
#endif

typedef struct _slBIT8 {
    uint8_t b0 : 1;
    uint8_t b1 : 1;
    uint8_t b2 : 1;
    uint8_t b3 : 1;
    uint8_t b4 : 1;
    uint8_t b5 : 1;
    uint8_t b6 : 1;
    uint8_t b7 : 1;
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
    if ((ptr)) {\
        *((DWORD *)ptr) = (size);\
        ptr = (_Type)(((DWORD *)ptr) + 1);\
        *((DWORD *)(((BYTE *)ptr) + size)) = slCyclicRedundanceCheck32((const BYTE *)(((DWORD *)ptr) - 1), sizeof(DWORD));\
    }\
} while (0)

#define _slReleaseAllocatedMemoryDebug(ptr) do {\
    if ((ptr) && (slCyclicRedundanceCheck32((const BYTE *)(((DWORD *)ptr) - 1), sizeof(DWORD)) ^ *((DWORD *)(((BYTE *)ptr) + *(((DWORD *)ptr) - 1)))))\
    {\
        slLogMessage(__FILE__, __LINE__, __DATE__, __TIME__, Memory Overflow);\
        CallSegmentationFault();\
    }\
    else if ((ptr))\
    {\
        free (((DWORD *)ptr) - 1);\
        ptr = NULL;\
    }\
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

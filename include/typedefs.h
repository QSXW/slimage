#pragma once

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#if defined( _MSC_VER )
#pragma warning(disable: 4200) /* for flexible array used in struct and union */
#pragma warning(disable: 4255)
#pragma warning(disable: 4305) /* initializing */
#pragma warning(disable: 4706) /* assignment within conditional expression */
#pragma warning(disable: 4820) /* padding for struct */
#pragma warning(disable: 4920)
#pragma warning(disable: 4996)
#include <intrin.h>
#define _mm256_ref_ps(ymm0) ymm0.m256_f32
#else
#include <x86intrin.h>
#define _mm256_ref_ps(ymm0) ymm0
#endif

#include "slassert.h"
#include "slchecksum.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef void (*caller)(void) ;
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

#define SLEXIT_FAILURE -1
#define SLEXIT_SUCCEED 0

typedef struct _slBIT8 {
    int b0 : 1;
    int b1 : 1;
    int b2 : 1;
    int b3 : 1;
    int b4 : 1;
    int b5 : 1;
    int b6 : 1;
    int b7 : 1;
} bit8, BIT8;

typedef struct _slRATIONAL {
    DWORD numerator;
    DWORD denominator; 
} RATIONAL;

typedef struct _slSegmentationFault {
    DWORD *exception;
} SegmentationFault;
#define CallSegmentationFault() ((SegmentationFault *)0x0)->exception++

/*  @checking of memory overflow within allocating and deallocating */
#if defined( DEBUG ) || defined(_DEBUG)
#define _alloc_type size_t
#define _slAllocatedMemoryDebug(ptr, _Type, size) do {\
    (ptr) = (_Type)malloc((_alloc_type)(size) + (sizeof(_alloc_type) * 2));\
    if ((ptr)) {\
        *((_alloc_type *)ptr) = (size);\
        ptr = (_Type)(((_alloc_type *)ptr) + 1);\
        *((_alloc_type *)(((BYTE *)ptr) + size)) = slCyclicRedundanceCheck32((const BYTE *)(((_alloc_type *)ptr) - 1), sizeof(_alloc_type));\
    }\
} while (0)

#define _slReleaseAllocatedMemoryDebug(ptr) do {\
    if ((ptr) && (slCyclicRedundanceCheck32((const BYTE *)(((_alloc_type *)ptr) - 1), sizeof(_alloc_type)) ^ *((_alloc_type *)(((BYTE *)ptr) + *(((_alloc_type *)ptr) - 1)))))\
    {\
        slLogMessage(__FILE__, __LINE__, __DATE__, __TIME__, Memory Overflow);\
        CallSegmentationFault();\
    }\
    else if ((ptr))\
    {\
        free (((_alloc_type *)ptr) - 1);\
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

#if defined( __cplusplus )
}
#endif /* __cplusplus */

#endif

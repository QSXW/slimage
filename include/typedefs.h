#pragma once

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <x86intrin.h>

#include <stdint.h>
#include <stddef.h>

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
typedef unsigned long  DWORD;
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

#define slAddressOf(variable) (size_t)*(void **)(&(variable))
#define slReleaseAllocatedMemory(ptr) do { if ((ptr)) { free(ptr); (ptr) = NULL; } } while (0)

#endif

#pragma once

#ifndef __SLFRAME_H__
#define __SLFRAME_H__

#include <slassert.h>
#include <typedefs.h>

#include <stdio.h>
#include <malloc.h>
#include <memory.h>

#define SLFRAME_DTYPE_BYTE     0x1u
#define SLFRAME_DTYPE_INT8     0x2u
#define SLFRAME_DTYPE_INT16    0x3u
#define SLFRAME_DTYPE_WORD     0x4u
#define SLFRAME_DTYPE_INT32    0x5u
#define SLFRAME_DTYPE_INT64    0x6u
#define SLFRAME_DTYPE_DWORD    0x7u
#define SLFRAME_DTYPE_QWORD    0x8u
#define SLFRAME_DTYPE_FLOAT32  0x9u
#define SLFRAME_DTYPE_FLOAT64  0x10u

typedef struct _slFrame  {
    size_t      size;
    INT32       colorSpace;
    INT32       dims;
    INT32       row;
    INT32       cols;
    INT32       dtype;
    size_t      dsize;
    BYTE        data[0];
} *Frame, slFrame;

Frame
slFrameAllocator(
    INT32 x,
    INT32 y,
    INT32 z,
    INT32 dtype,
    void *data
    );
size_t
slCompareFrame(
    Frame frameSt,
    Frame frameNd
    );

static const size_t slFrameDataTypeSize[]= {
        0,
        sizeof(BYTE),
        sizeof(INT8),
        sizeof(INT16),
        sizeof(WORD),
        sizeof(INT32),
        sizeof(INT64),
        sizeof(DWORD),
        sizeof(QWORD),
        sizeof(FLOAT32),
        sizeof(FLOAT64),
};

#define slDataTypeSize(dtype) slFrameDataTypeSize[(dtype)]
#define slFrameDeallocator(frame) slReleaseAllocatedMemory(frame)
#define slRGBToFrameBinder(R, G, B, FRAME, offset) do { R = ((FRAME)->data); G = (R) + (((FRAME)->size) * offset); B = (G) + (((FRAME)->size) * offset); } while (0)

#endif /* __SLFRAME_H__ */

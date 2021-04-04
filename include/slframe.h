#pragma once

#ifndef __SLFRAME_H__
#define __SLFRAME_H__

#include <slassert.h>
#include <typedefs.h>

#include <stdio.h>
#include <malloc.h>
#include <memory.h>

enum FrameDataType {
    SLFRAME_DTYPE_BYTE =  0x1u,
    SLFRAME_DTYPE_INT8,
    SLFRAME_DTYPE_INT16,
    SLFRAME_DTYPE_WORD,
    SLFRAME_DTYPE_INT32,
    SLFRAME_DTYPE_INT64,
    SLFRAME_DTYPE_DWORD,
    SLFRAME_DTYPE_QWORD,
    SLFRAME_DTYPE_float,
    SLFRAME_DTYPE_double
};

typedef struct _slFrame  {
    size_t      size;
    size_t      dsize;
    INT32       colorSpace;
    INT32       dims;
    INT32       row;
    INT32       cols;
    INT32       dtype;
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
        sizeof(float),
        sizeof(double),
};

#define slDataTypeSize(dtype) slFrameDataTypeSize[(dtype)]
#define slFrameDeallocator(frame) slReleaseAllocatedMemory(frame)
#define slRGBToFrameBinder(R, G, B, FRAME, offset) do { R = ((FRAME)->data); G = (R) + (((FRAME)->size) * offset); B = (G) + (((FRAME)->size) * offset); } while (0)

#endif /* __SLFRAME_H__ */

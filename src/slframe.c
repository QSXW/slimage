#include "slframe.h"

Frame slFrameAllocator(INT32 x, INT32 y, INT32 z, INT32 dtype, void *data)
{
    Frame frame;
    size_t size;
    size_t mallocSize;

    x  = x > 0 ? x : 0;
    y  = y > 0 ? y : 0;
    z  = z > 0 ? z : 0;

    size = (size_t)x * (size_t)y;
    size += slMod4(size);
    mallocSize = size * z * slDataTypeSize(dtype);
    mallocSize += slMod8(mallocSize);

    slAllocateMemory(frame, Frame, sizeof(slFrame) + mallocSize);
    slAssert (
        frame,
        SLEXCEPTION_MALLOC_FAILED,
        NULL
    );
    if (!mallocSize) {
        memset(frame, 0x0, sizeof(struct _slFrame));
    } else {
        frame->size  = size;
        frame->dims  = z;
        frame->row   = y;
        frame->cols  = x;
        frame->dsize = mallocSize;
        frame->dtype = dtype;
        if (data) {
            memcpy(frame->data, data, mallocSize);
        } else {
            memset(frame->data, 0x0, mallocSize);
        }
    }

    return frame;
}

size_t slCompareFrame(Frame frameSt, Frame frameNd)
{
    register BYTE *ptrst;
    register BYTE *ptrnd;
    size_t i, dataSize;

    if (
           (frameSt->row == frameNd->row)
        && (frameSt->cols == frameNd->cols)
        && (frameSt->dims == frameNd->dims)
        && (frameSt->dtype == frameNd->dtype)
    ){
        ptrst = frameSt->data;
        ptrnd = frameNd->data;
        for (
            i = 0, dataSize = (size_t)frameSt->size;
            i < dataSize; i++
        ) {
            if (*ptrst++ != *ptrnd++) {
                return i;
            }
        }
    } else {
        return -1;
    }

    return i;
}

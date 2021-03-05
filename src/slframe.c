#include <slframe.h>

INT32 slDataTypeSize(INT32 flag)
{
    INT32 dataTypeSize[]= {
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
    return dataTypeSize[flag];
}

Frame slFrameAllocator(INT32 x, INT32 y, INT32 z, INT32 dtype, void *data)
{
    Frame frame;
    size_t dataSize;
    size_t size;
    size_t mallocSize;

    x  = x > 0 ? x : 0;
    y  = y > 0 ? y : 0;
    z  = z > 0 ? z : 0;

    size = (size_t)x * (size_t)y;
    dataSize = size * z;
    dataSize = dataSize * slDataTypeSize(dtype);
    mallocSize = dataSize + ((dataSize & 0x7) ? (8 - (dataSize & 0x7)) : 0);

    slAssert (
        (frame = (Frame)malloc(sizeof(slFrame) + mallocSize)),
        SLEXCEPTION_MALLOC_FAILED,
        NULL
    );
    if (!dataSize)
    {
        memset(frame, 0x0, sizeof(slFrame));
    }
    else
    {
        frame->size  = size;
        frame->dims  = z;
        frame->row   = y;
        frame->cols  = x;
        frame->dsize = mallocSize;
        frame->dtype = dtype;
        if (data)
        {
            memcpy(frame->data, data, dataSize);
        }
        else
        {
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

    if ((frameSt->row == frameNd->row)
         && (frameSt->cols == frameNd->cols)
         && (frameSt->dims == frameNd->dims)
         && (frameSt->dtype == frameNd->dtype))
    {
        ptrst  = frameSt->data;
        ptrnd = frameNd->data;
        
        for (i = 0, dataSize = (size_t)frameSt->size; i < dataSize; i++)
        {
            if (*ptrst++ != *ptrnd++)
            {
                return i;
            }
        }
    }
    else
    {
        return -1;
    }

    return i;
}

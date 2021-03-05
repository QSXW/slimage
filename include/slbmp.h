#pragma once

#ifndef __SLBMP_H__
#define __SLBMP_H__

#include <typedefs.h>
#include <slassert.h>
#include <slframe.h>
#include <stream.h>
#include <slendian.h>

#ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */

#define SLBMPCOMPRESSION_NONE    0x00000000
#define SLBMPCOMPRESSION_EIGHT   0x00000001
#define SLBMPCOMPRESSION_FOUR    0x00000002
#define SLBMPCOMPRESSION_RGB     0x00000003

#pragma pack(push, 1)
typedef struct  _SLBMP {
    WORD    identifer;
    DWORD   fileSize;
    WORD    RESERVER_ST;
    WORD    RESERVER_ND;
    DWORD   offset;
    DWORD   informationSize;
    INT32   width;
    INT32   height;
    WORD    planes;
    WORD    bitsPerPixel;
    DWORD   compressionType;
    DWORD   imageSize;
    INT32   horizontalResolution;
    INT32   verticalResolution;
    DWORD   coloursNum;
    DWORD   importantColours;
    BYTE    data[0];
} BMP;
#pragma pack(pop)

typedef struct  _BMPRGB {
    BYTE B;
    BYTE G;
    BYTE R;
} BMPRGB;

Frame
BMPRead(
    const Stream stream
    );
INT32
BMPDecoder(
    const struct _SLBMP *bmp,
    Frame *frame
    );

INT32 BMPEncoder(
    Frame frame,
    CharSequence **charSequence
    );
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BMP_H__ */

#pragma once
#ifndef __SLIMAGE_H__
#define __SLIMAGE_H__

#include <math.h>

#include "typedefs.h"
#include "sljpeg.h"
#include "slbmp.h"
#include "sequence.h"
#include "slframe.h"
#include "slresize.h"
#include "slcolorspace.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SLIMAGE_FORMAT_APNG            0x00000001
#define SLIMAGE_FORMAT_BMP             0x00000002
#define SLIMAGE_FORMAT_CDR             0x00000003
#define SLIMAGE_FORMAT_CMX             0x00000004
#define SLIMAGE_FORMAT_DIB             0x00000005
#define SLIMAGE_FORMAT_DICOM           0x00000006
#define SLIMAGE_FORMAT_DJVU            0x00000007
#define SLIMAGE_FORMAT_BMPDNG          0x00000008
#define SLIMAGE_FORMAT_EMF             0x00000009
#define SLIMAGE_FORMAT_EMZ             0x0000000A
#define SLIMAGE_FORMAT_EXIF            0x0000000B
#define SLIMAGE_FORMAT_GIF             0x0000000C
#define SLIMAGE_FORMAT_ICO             0x0000000D
#define SLIMAGE_FORMAT_J2K             0x0000000E
#define SLIMAGE_FORMAT_JP2             0x0000000F
#define SLIMAGE_FORMAT_JPEG            0x00000010
#define SLIMAGE_FORMAT_ODG             0x00000011
#define SLIMAGE_FORMAT_OTG             0x00000012
#define SLIMAGE_FORMAT_PNG             0x00000013
#define SLIMAGE_FORMAT_PSB             0x00000014
#define SLIMAGE_FORMAT_PSD             0x00000015
#define SLIMAGE_FORMAT_SVGZ            0x00000016
#define SLIMAGE_FORMAT_TGA             0x00000017
#define SLIMAGE_FORMAT_TIFF            0x00000018
#define SLIMAGE_FORMAT_VDX             0x00000019
#define SLIMAGE_FORMAT_VSDM            0x00000020
#define SLIMAGE_FORMAT_VSDX            0x00000021
#define SLIMAGE_FORMAT_VSS             0x00000022
#define SLIMAGE_FORMAT_VSSM            0x00000023
#define SLIMAGE_FORMAT_VSSX            0x00000024
#define SLIMAGE_FORMAT_VST             0x00000025
#define SLIMAGE_FORMAT_VSTM            0x00000026
#define SLIMAGE_FORMAT_VSTX            0x00000027
#define SLIMAGE_FORMAT_VSX             0x00000028
#define SLIMAGE_FORMAT_WEMP            0x00000029
#define SLIMAGE_FORMAT_WMF             0x0000002A
#define SLIMAGE_FORMAT_WMZ             0x0000002B
#define SLIMAGE_FORMAT_UNRECOGNIZED    -1

#define SLIMAGE_ID_BMP_BEG             0x42
#define SLIMAGE_ID_BMP_END             0x4D
#define SLIMAGE_ID_JPEG_BEG            0xFF
#define SLIMAGE_ID_JPEG_END            0xD8
#define SLIMAGE_ID_PNG_BEG             0x89
#define SLIMAGE_ID_PGN_END             ((BYTE *)"\211\120\116\107\015\012\032\012") /* 0x0A1A0A0D474E5089 */

#define SLIMAGE_BICUBIC 1
#define SLIMAGE_NEAREST 2

INT32
ImageFormat(
    const DWORD *formatID
    );
Frame
ImageRead(
    const char *filename
);
INT32
ImageWrite(
    const char *filePath,
    Frame frame,
    INT8 monochorome,
    INT32 format
    );
Frame ImageResize(
    Frame src,
    float widthRatio,
    float heightRatio,
    INT32 width,
    INT32 height,
    INT32 method
    );
INT32 ImageShow(
    Frame frame
    );

#ifndef __STDIMG_MARCRO_FUNCTION__
#define __STDIMG_MARCRO_FUNCTION__
#define ASSERT_IMAGE_ID(type, ID, sign, FORMAT) do { if (*(++type) == ID) { sign = FORMAT; break; } } while (0)
#endif/* __STDIMG_MARCRO_FUNCTION__ */

#define DisplayRGBMatrix(r, g, b, size, title, flag) {\
    if (title) { printf(title); }\
    for (int i = 0; i < size; i++) { printf("0x%x%x%x  ", (r)[i], (g)[i], (b)[i]); if (flag && ((i + 1) % flag == 0)) { putchar(10); } }\
    putchar(10);\
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SLIMAGE_H__ */

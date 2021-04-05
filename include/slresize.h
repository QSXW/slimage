#pragma once
#ifndef __SLRESIZE_H__
#define __SLRESIZE_H__

#include <math.h>

#include "slframe.h"
#include "slcolorspace.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define slBicubicConvolutionKey (-0.5f)
#define slNearestSample(sample, channel, width, x, y) do { sample = *(channel + (size_t)y * (size_t)width + x); } while (0)

INT32
slNearestInterpolateRGB(
    Frame src,
    Frame dst,
    float widthRatio,
    float heightRatio
    );
INT32
slBicubicConvolutionInterpolateRGB(
    Frame src,
    Frame dst,
    float widthRatio,
    float heightRatio
    );
INT32
slBicubicConvolutionInterpolateByteChannel(
    BYTE *src,
    BYTE *dst,
    float widthRatio,
    float heightRatio,
    INT32 srcWidth,
    INT32 srcHeight,
    INT32 dstWidth,
    INT32 dstHeight
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SLRESIZE_H__ */

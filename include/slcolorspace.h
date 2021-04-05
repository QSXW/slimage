#pragma once

#ifndef __SLCOLORSPACE_H__
#define __SLCOLORSPACE_H__

#include "typedefs.h"
#include "slassert.h"
#include "slframe.h"
#include "generic.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum SLColorSpace {
    SLCOLORSPACE_RGB = 0,
    SLCOLORSPACE_YCC,
    SLCOLORSPACE_CMYK,
    SLCOLORSPACE_HSI,
    SLCOLORSPACE_HSV,
    SLCOLORSPACE_LAB
};

// #define __CCIR_RECOMMEMDATION_709
#ifndef __CCIR_RECOMMEMDATION_709
#define __CCIR_RECOMMEMDATION_601_1
#endif

#ifdef __CCIR_RECOMMEMDATION_601_1
#define SLCOLORSPACE_LUMINANCE_RED   0.299
#define SLCOLORSPACE_LUMINANCE_GREEN 0.587
#define SLCOLORSPACE_LUMINANCE_BLUE  0.114
#endif /* __CCIR_RECOMMEMDATION_601_1 */

#ifdef __CCIR_RECOMMEMDATION_709
#define SLCOLORSPACE_LUMINANCE_RED   0.2125
#define SLCOLORSPACE_LUMINANCE_GREEN 0.7157
#define SLCOLORSPACE_LUMINANCE_BLUE  0.721
#endif /* __CCIR_RECOMMEMDATION_709 */

#define slColourSampleClamp(sample, min, max) (((sample) < (min) ? (min) : (((sample) > max) ? (max) : (sample))))

static const float slcolorspace_luminance_table[] = {
    (2.0 - 2.0 * SLCOLORSPACE_LUMINANCE_RED),
    (2.0 - 2.0 * SLCOLORSPACE_LUMINANCE_BLUE),
    1.0 / SLCOLORSPACE_LUMINANCE_GREEN,
    SLCOLORSPACE_LUMINANCE_RED,
    SLCOLORSPACE_LUMINANCE_BLUE
};

#define slcolorspace_luminamce_table_length sizeof(slcolorspace_luminance_table) / sizeof(slcolorspace_luminance_table[0])

typedef struct _RGBA {
    BYTE r;
    BYTE g;
    BYTE b;
    BYTE a;
} slRGBA;

#define RGBTOYCBCR( R, G, B, Y, CB, CR, startIndex, endIndex, offset) while (startIndex < endIndex) {\
    *(Y) = SLCOLORSPACE_LUMINANCE_RED * (*(R)) +  SLCOLORSPACE_LUMINANCE_GREEN * (*(G)) + SLCOLORSPACE_LUMINANCE_BLUE * (*(B));\
    *(CB) = 128 + ( ((*(B)) - (*Y)) / (2 - 2 * SLCOLORSPACE_LUMINANCE_BLUE) );\
    *(CR) = 128 + ( ((*(R)) - (*Y)) / (2 - 2 * SLCOLORSPACE_LUMINANCE_RED) );\
    Y += offset; CB += offset; CR += offset; R += offset; G += offset; B += offset; startIndex++;\
}

#define YCBCRTORGB( Y, CB, CR, R, G, B, startIndex, endIndex, offset) while (startIndex < endIndex) {\
    *(R) = slColourSampleClamp( (*CR - 128) * (2 - 2 * SLCOLORSPACE_LUMINANCE_RED) + (*Y), 0, 255 );\
    *(B) = slColourSampleClamp( (*CB - 128) * (2 - 2 * SLCOLORSPACE_LUMINANCE_BLUE) + (*Y), 0, 255);\
    *(G) = slColourSampleClamp( ((*Y) - SLCOLORSPACE_LUMINANCE_BLUE * *(B) - SLCOLORSPACE_LUMINANCE_RED * (*(R))) / SLCOLORSPACE_LUMINANCE_GREEN, 0, 255);\
     Y += offset; CB += offset; CR += offset; R += offset; G += offset; B += offset; startIndex++;\
}

Frame slRGBToYCbCr(
    Frame rgb
    );
Frame slYCbCrToRGB(
    Frame ycbcr
    );

INT32
slChromaUpsampling422(
    Frame frame,
    float *Cb,
    float *Cr
    );

INT32
slChromaUpsampling420(
    Frame frame,
    float *Cb,
    float *Cr
    );

Frame
slDefaultToRGBA(
    Frame frame
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SLCOLORSPACE_H__ */

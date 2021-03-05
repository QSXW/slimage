#pragma once

#ifndef __SLCOLORSPACE_H__
#define __SLCOLORSPACE_H__

#include <typedefs.h>
#include <slassert.h>
#include <slframe.h>
#include <generic.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum SLColorSpace {
    SLCOLORSPACE_RGB = 0,
    SLCOLORSPACE_YCC,
    SLCOLORSPACE_CMYK 
};

#define SL_COLOR_RGB  0x01
#define SL_COLOR_YUV  0x02
#define SL_COLOR_CMYK 0x03

// #define __CCIR_RECOMMEMDATION_709
#define __CCIR_RECOMMEMDATION_601_1

#ifdef __CCIR_RECOMMEMDATION_709
#undef __CCIR_RECOMMEMDATION_601_1
#endif

#ifdef __CCIR_RECOMMEMDATION_601_1
#define SLIMG_LUMARED   0.299
#define SLIMG_LUMAGREEN 0.587
#define SLIMG_LUMABLUE  0.114
#endif /* __CCIR_RECOMMEMDATION_601_1 */

#ifdef __CCIR_RECOMMEMDATION_709
#define SLIMG_LUMARED   0.2125
#define SLIMG_LUMAGREEN 0.7157
#define SLIMG_LUMABLUE  0.721
#endif /* __CCIR_RECOMMEMDATION_709 */

#define slColourSampleClamp(sample, min, max) (((sample) < (min) ? (min) : (((sample) > max) ? (max) : (sample))))

typedef struct _RGBA {
    BYTE r;
    BYTE g;
    BYTE b;
    BYTE a;
} slRGBA;

#if 0
#define RGBTOYCVCR(DATATYPE, R, G, B, Y, CB, CR, beg, end, offset) while (beg < end) {\
    *(Y) = SLIMG_LUMARED * (*((DATATYPE*)R)) +  SLIMG_LUMAGREEN * (*((DATATYPE*)G)) + SLIMG_LUMABLUE * (*((DATATYPE*)B));\
    *(CB) = ((*((DATATYPE*)B)) - (*Y)) / (2 - 2 * SLIMG_LUMABLUE);\
    *(CR) = ((*((DATATYPE*)R)) - (*Y)) / (2 - 2 * SLIMG_LUMARED);\
    Y += offset; CB += offset; CR += offset; R += offset; G += offset; B += offset; beg++;\
}

#define YCVCRTORGB(DATATYPE, Y, CB, CR, R, G, B, beg, end, offset) while (beg < end) {\
    *((DATATYPE*)R) = (*CR) * (2 - 2 * SLIMG_LUMARED) + (*Y);\
    *((DATATYPE*)B) = (*CB) * (2 - 2 * SLIMG_LUMABLUE) + (*Y);\
    *((DATATYPE*)G) = ((*Y) - SLIMG_LUMABLUE * (*((DATATYPE*)B)) - SLIMG_LUMARED * (*((DATATYPE*)R))) / SLIMG_LUMAGREEN;\
     Y += offset; CB += offset; CR += offset; R += offset; G += offset; B += offset; beg++;\
}
#endif

#define RGBTOYCBCR( R, G, B, Y, CB, CR, startIndex, endIndex, offset) while (startIndex < endIndex) {\
    *(Y) = SLIMG_LUMARED * (*(R)) +  SLIMG_LUMAGREEN * (*(G)) + SLIMG_LUMABLUE * (*(B));\
    *(CB) = 128 + ( ((*(B)) - (*Y)) / (2 - 2 * SLIMG_LUMABLUE) );\
    *(CR) = 128 + ( ((*(R)) - (*Y)) / (2 - 2 * SLIMG_LUMARED) );\
    Y += offset; CB += offset; CR += offset; R += offset; G += offset; B += offset; startIndex++;\
}

#define YCBCRTORGB( Y, CB, CR, R, G, B, startIndex, endIndex, offset) while (startIndex < endIndex) {\
    *(R) = slColourSampleClamp( (*CR - 128) * (2 - 2 * SLIMG_LUMARED) + (*Y), 0, 255 );\
    *(B) = slColourSampleClamp( (*CB - 128) * (2 - 2 * SLIMG_LUMABLUE) + (*Y), 0, 255);\
    *(G) = slColourSampleClamp( ((*Y) - SLIMG_LUMABLUE * *(B) - SLIMG_LUMARED * (*(R))) / SLIMG_LUMAGREEN, 0, 255);\
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
    BYTE *Cb,
    BYTE *Cr
    );

INT32
slChromaUpsampling420(
    Frame frame,
    BYTE *Cb,
    BYTE *Cr
    );

Frame
slDefaultToRGBA(
    Frame frame
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SLCOLORSPACE_H__ */
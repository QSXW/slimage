#include "slresize.h"

float slBicubicConvolutionKernal(float x)
{
    float absx;
    float weight;

    absx = slAbsolute(x);
    if (absx <= 1) {
        weight = (float)((slBicubicConvolutionKey + 2.0) * pow(absx, 3.0) - (slBicubicConvolutionKey + 3.0) * pow(absx, 2.0) + 1.0);
    }
    else if (absx < 2)
    {
        weight = (float)(slBicubicConvolutionKey * pow(absx, 3.0) - (slBicubicConvolutionKey * 5.0) * pow(absx, 2.0) + (slBicubicConvolutionKey * 8.0) * absx - (slBicubicConvolutionKey * 4.0));
    }
    else if (absx == 0)
    {
        weight = 1.0;
    }
    else
    {
        weight = 0;
    }

    return weight;
}

INT32 slBicubicConvolutionInterpolateByteChannel(BYTE *src, BYTE *dst, float widthRatio, float heightRatio, INT32 srcWidth, INT32 srcHeight, INT32 dstWidth, INT32 dstHeight)
{
    float x, y;
    INT32 ix, iy;

    INT32 x0, x1, x2, x3;
    INT32 y0, y1, y2, y3;
    float cx0, cx1, cx2, cx3;
    float cy0, cy1, cy2, cy3;

    INT32 scanLine;
    float sample;

    for (iy = 0, scanLine = 0; iy < dstHeight; iy++)
    {
        for (ix = 0; ix < dstWidth; ix++)
        {
            x = ix / widthRatio;
            y = iy / heightRatio;

            x1 = ((INT32)x);
            x0 = x1 - 1;
            x2 = x1 + 1;
            x3 = x2 + 1;
            y1 = ((INT32)y);
            y0 = y1 - 1;
            y2 = y1 + 1;
            y3 = y2 + 1; 

            cx0 = slBicubicConvolutionKernal(x - x0);
            cx1 = slBicubicConvolutionKernal(x - x1);
            cx2 = slBicubicConvolutionKernal(x - x2);
            cx3 = slBicubicConvolutionKernal(x - x3);
            cy0 = slBicubicConvolutionKernal(y - y0);
            cy1 = slBicubicConvolutionKernal(y - y1);
            cy2 = slBicubicConvolutionKernal(y - y2);
            cy3 = slBicubicConvolutionKernal(y - y3);

            x0 = x0 < 0 ? x1 : x0;
            x3 = x3 >= srcWidth ? x2 : x3;
            y0 = y0 < 0 ? y1 : y0;
            y2 = y2 >= srcHeight ? y1 : y2;
            y3 = y3 >= srcHeight ? y2 : y3;

            sample  = *(src + (size_t)y0 * (size_t)srcWidth + x0) * cx0 * cy0
                    + *(src + (size_t)y1 * (size_t)srcWidth + x0) * cx0 * cy1
                    + *(src + (size_t)y2 * (size_t)srcWidth + x0) * cx0 * cy2
                    + *(src + (size_t)y3 * (size_t)srcWidth + x0) * cx0 * cy3
                    + *(src + (size_t)y0 * (size_t)srcWidth + x1) * cx1 * cy0
                    + *(src + (size_t)y1 * (size_t)srcWidth + x1) * cx1 * cy1
                    + *(src + (size_t)y2 * (size_t)srcWidth + x1) * cx1 * cy2
                    + *(src + (size_t)y3 * (size_t)srcWidth + x1) * cx1 * cy3
                    + *(src + (size_t)y0 * (size_t)srcWidth + x2) * cx2 * cy0
                    + *(src + (size_t)y1 * (size_t)srcWidth + x2) * cx2 * cy1
                    + *(src + (size_t)y2 * (size_t)srcWidth + x2) * cx2 * cy2
                    + *(src + (size_t)y3 * (size_t)srcWidth + x2) * cx2 * cy3
                    + *(src + (size_t)y0 * (size_t)srcWidth + x3) * cx3 * cy0
                    + *(src + (size_t)y1 * (size_t)srcWidth + x3) * cx3 * cy1
                    + *(src + (size_t)y2 * (size_t)srcWidth + x3) * cx3 * cy2
                    + *(src + (size_t)y3 * (size_t)srcWidth + x3) * cx3 * cy3;

            *(dst   + scanLine + ix) = (BYTE)slColourSampleClamp(sample,   0, 255);
        }
        scanLine += dstWidth;
    } 

    return 0;
}

INT32 slNearestInterpolateRGB(Frame src, Frame dst, float widthRatio, float heightRatio)
{
    INT32 ix, iy;
    INT32 x, y;

    DWORD scanLine;
    INT32 srcWidth;
    INT32 dstWidth, dstHeight;
    INT32 redSample, greenSample, blueSample;
    BYTE *inputRedChannel,  *inputGreenChannel,  *inputBlueChannel;
    BYTE *outputRedChannel, *outputGreenChannel, *outputBlueChannel;

    srcWidth  = src->cols;
    dstWidth  = dst->cols;
    dstHeight = dst->row;

    slRGBToFrameBinder(inputRedChannel,  inputGreenChannel,  inputBlueChannel,  src, 0x1);
    slRGBToFrameBinder(outputRedChannel, outputGreenChannel, outputBlueChannel, dst, 0x1);
        
    for ( iy = 0, scanLine = 0; iy < dstHeight; iy++ )
    {
        for ( ix = 0; ix < dstWidth; ix++ )
        {
            x = (INT32)(ix / widthRatio);
            y = (INT32)(iy / heightRatio);

            slNearestSample(redSample,   inputRedChannel,   srcWidth, x, y);
            slNearestSample(greenSample, inputGreenChannel, srcWidth, x, y);
            slNearestSample(blueSample,  inputBlueChannel,  srcWidth, x, y);

            *(outputRedChannel   + scanLine + iy) = (BYTE)slColourSampleClamp(redSample,   0, 255);
            *(outputGreenChannel + scanLine + iy) = (BYTE)slColourSampleClamp(greenSample, 0, 255);
            *(outputBlueChannel  + scanLine + iy) = (BYTE)slColourSampleClamp(blueSample,  0, 255);
        }
        scanLine += dstWidth;
    } 
    
    return 0;
}

INT32 slBicubicConvolutionInterpolateRGB(Frame src, Frame dst, float widthRatio, float heightRatio)
{
    INT32 ix, iy;
    float x, y;

    INT32 x0, x1, x2, x3;
    INT32 y0, y1, y2, y3;
    float cx0, cx1, cx2, cx3;
    float cy0, cy1, cy2, cy3;

    DWORD scanLine;
    INT32 srcWidth, srcHeight;
    INT32 dstWidth, dstHeight;
    float redSample, greenSample, blueSample;
    BYTE *inputRedChannel,  *inputGreenChannel,  *inputBlueChannel;
    BYTE *outputRedChannel, *outputGreenChannel, *outputBlueChannel;

    srcWidth   = src->cols;
    srcHeight  = src->row;
    dstWidth   = dst->cols;
    dstHeight  = dst->row;

    slRGBToFrameBinder(inputRedChannel,  inputGreenChannel,  inputBlueChannel,  src, 0x1);
    slRGBToFrameBinder(outputRedChannel, outputGreenChannel, outputBlueChannel, dst, 0x1);
    
    for (iy = 0, scanLine = 0; iy < dstHeight; iy++)
    {
        for (ix = 0; ix < dstWidth; ix++)
        {
            x = ix / widthRatio;
            y = iy / heightRatio;

            x1 = ((INT32)x);
            x0 = x1 - 1;
            x2 = x1 + 1;
            x3 = x2 + 1;
            y1 = ((INT32)y);
            y0 = y1 - 1;
            y2 = y1 + 1;
            y3 = y2 + 1; 

            cx0 = slBicubicConvolutionKernal(x - x0);
            cx1 = slBicubicConvolutionKernal(x - x1);
            cx2 = slBicubicConvolutionKernal(x - x2);
            cx3 = slBicubicConvolutionKernal(x - x3);
            cy0 = slBicubicConvolutionKernal(y - y0);
            cy1 = slBicubicConvolutionKernal(y - y1);
            cy2 = slBicubicConvolutionKernal(y - y2);
            cy3 = slBicubicConvolutionKernal(y - y3);

            x0 = x0 < 0 ? x1 : x0;
            x3 = x3 >= srcWidth ? x2 : x3;
            y0 = y0 < 0 ? y1 : y0;
            y2 = y2 >= srcHeight ? y1 : y2;
            y3 = y3 >= srcHeight ? y2 : y3;

            redSample   = *(inputRedChannel + y0 * srcWidth + x0) * cx0 * cy0
                        + *(inputRedChannel + y1 * srcWidth + x0) * cx0 * cy1
                        + *(inputRedChannel + y2 * srcWidth + x0) * cx0 * cy2
                        + *(inputRedChannel + y3 * srcWidth + x0) * cx0 * cy3
                        + *(inputRedChannel + y0 * srcWidth + x1) * cx1 * cy0
                        + *(inputRedChannel + y1 * srcWidth + x1) * cx1 * cy1
                        + *(inputRedChannel + y2 * srcWidth + x1) * cx1 * cy2
                        + *(inputRedChannel + y3 * srcWidth + x1) * cx1 * cy3
                        + *(inputRedChannel + y0 * srcWidth + x2) * cx2 * cy0
                        + *(inputRedChannel + y1 * srcWidth + x2) * cx2 * cy1
                        + *(inputRedChannel + y2 * srcWidth + x2) * cx2 * cy2
                        + *(inputRedChannel + y3 * srcWidth + x2) * cx2 * cy3
                        + *(inputRedChannel + y0 * srcWidth + x3) * cx3 * cy0
                        + *(inputRedChannel + y1 * srcWidth + x3) * cx3 * cy1
                        + *(inputRedChannel + y2 * srcWidth + x3) * cx3 * cy2
                        + *(inputRedChannel + y3 * srcWidth + x3) * cx3 * cy3;

            greenSample = *(inputGreenChannel + y0 * srcWidth + x0) * cx0 * cy0
                        + *(inputGreenChannel + y1 * srcWidth + x0) * cx0 * cy1
                        + *(inputGreenChannel + y2 * srcWidth + x0) * cx0 * cy2
                        + *(inputGreenChannel + y3 * srcWidth + x0) * cx0 * cy3
                        + *(inputGreenChannel + y0 * srcWidth + x1) * cx1 * cy0
                        + *(inputGreenChannel + y1 * srcWidth + x1) * cx1 * cy1
                        + *(inputGreenChannel + y2 * srcWidth + x1) * cx1 * cy2
                        + *(inputGreenChannel + y3 * srcWidth + x1) * cx1 * cy3
                        + *(inputGreenChannel + y0 * srcWidth + x2) * cx2 * cy0
                        + *(inputGreenChannel + y1 * srcWidth + x2) * cx2 * cy1
                        + *(inputGreenChannel + y2 * srcWidth + x2) * cx2 * cy2
                        + *(inputGreenChannel + y3 * srcWidth + x2) * cx2 * cy3
                        + *(inputGreenChannel + y0 * srcWidth + x3) * cx3 * cy0
                        + *(inputGreenChannel + y1 * srcWidth + x3) * cx3 * cy1
                        + *(inputGreenChannel + y2 * srcWidth + x3) * cx3 * cy2
                        + *(inputGreenChannel + y3 * srcWidth + x3) * cx3 * cy3;

            blueSample  = *(inputBlueChannel + y0 * srcWidth + x0) * cx0 * cy0
                        + *(inputBlueChannel + y1 * srcWidth + x0) * cx0 * cy1
                        + *(inputBlueChannel + y2 * srcWidth + x0) * cx0 * cy2
                        + *(inputBlueChannel + y3 * srcWidth + x0) * cx0 * cy3
                        + *(inputBlueChannel + y0 * srcWidth + x1) * cx1 * cy0
                        + *(inputBlueChannel + y1 * srcWidth + x1) * cx1 * cy1
                        + *(inputBlueChannel + y2 * srcWidth + x1) * cx1 * cy2
                        + *(inputBlueChannel + y3 * srcWidth + x1) * cx1 * cy3
                        + *(inputBlueChannel + y0 * srcWidth + x2) * cx2 * cy0
                        + *(inputBlueChannel + y1 * srcWidth + x2) * cx2 * cy1
                        + *(inputBlueChannel + y2 * srcWidth + x2) * cx2 * cy2
                        + *(inputBlueChannel + y3 * srcWidth + x2) * cx2 * cy3
                        + *(inputBlueChannel + y0 * srcWidth + x3) * cx3 * cy0
                        + *(inputBlueChannel + y1 * srcWidth + x3) * cx3 * cy1
                        + *(inputBlueChannel + y2 * srcWidth + x3) * cx3 * cy2
                        + *(inputBlueChannel + y3 * srcWidth + x3) * cx3 * cy3;

            *(outputRedChannel   + scanLine + ix) = (BYTE)slColourSampleClamp(redSample,   0, 255);
            *(outputGreenChannel + scanLine + ix) = (BYTE)slColourSampleClamp(greenSample, 0, 255);
            *(outputBlueChannel  + scanLine + ix) = (BYTE)slColourSampleClamp(blueSample,  0, 255);
        }
        scanLine += dstWidth;
    } 

    return 0;
}

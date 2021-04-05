#include "slcolorspace.h"

Frame slRGBToYCbCr(Frame rgb)
{
    Frame ycbcr;
    BYTE *R, *G,  *B;
    BYTE *Y, *CB, *CR;
    size_t ci, size, offset;

    if ((offset = slDataTypeSize(rgb->dtype)) != SLFRAME_DTYPE_BYTE)
    {
        // Write Error: The data type was wrong.
    }
    if (rgb->dims == 0x3)
    {
        slAssert(
            ycbcr = slFrameAllocator(rgb->cols, rgb->row, rgb->dims, rgb->dtype, NULL),
            SLEXCEPTION_NULLPOINTER_REFERENCE,
            NULL
        );
        ci = 0;
        size = rgb->size;
        slRGBToFrameBinder(R, G,  B,  rgb,   sizeof(BYTE));
        slRGBToFrameBinder(Y, CB, CR, ycbcr, sizeof(BYTE));
        RGBTOYCBCR(R, G, B, Y, CB, CR, ci, size, offset);
    }
    else 
    {
        // Write error: Not a standard RGB Frame
        return NULL;
    }

    return ycbcr;
}

void slYCbCrToRGB_AVX(float **ycbcr, float **rgb, size_t size)
{
    const float constants[] = { 128.0, 1.0 / 255.0 };

    size_t i, j;
    __m256 ycbcr_register[3];
    __m256 rgb_register[3];
    __m256 luminance_portion[slcolorspace_luminamce_table_length];
    __m256 constants_register[sizeof(constants) / sizeof(constants[0])];

    constants_register[0] = _mm256_broadcast_ss(constants + 0);
    constants_register[1] = _mm256_broadcast_ss(constants + 1);
    for (i = 0; i < slcolorspace_luminamce_table_length; i++)
    {
        luminance_portion[i] = _mm256_broadcast_ss(slcolorspace_luminance_table + i);
    }
    for (i = 0; i < size; i += (sizeof(__m256) / sizeof(float)))
    {
        for (j = 0; j < 3; j++)
        {
            ycbcr_register[j] = _mm256_loadu_ps(ycbcr[j] + i);
            rgb_register[j] = _mm256_loadu_ps(rgb[j] + i);
        }
        rgb_register[0] = _mm256_add_ps(ycbcr_register[0], _mm256_mul_ps(_mm256_sub_ps(ycbcr_register[2], constants_register[0]), luminance_portion[0]));
        rgb_register[2] = _mm256_add_ps(ycbcr_register[0], _mm256_mul_ps(_mm256_sub_ps(ycbcr_register[1], constants_register[0]), luminance_portion[1]));
        rgb_register[1] = _mm256_mul_ps(_mm256_sub_ps(_mm256_sub_ps(ycbcr_register[0], _mm256_mul_ps(luminance_portion[4], rgb_register[2])), _mm256_mul_ps(luminance_portion[3], rgb_register[0])), luminance_portion[2]);
        for (j = 0; j < 3; j++)
        {
            _mm256_storeu_ps(rgb[j] + i, _mm256_mul_ps(rgb_register[j], constants_register[1]));
        }
    }
}

Frame slYCbCrToRGB(Frame ycbcr)
{
    Frame rgb;
    float *srcptr[3];
    float *dstptr[3];
    INT32 i;

    slAssert(
        (rgb = slFrameAllocator(ycbcr->cols, ycbcr->row, ycbcr->dims, ycbcr->dtype, NULL)),
        SLEXCEPTION_NULLPOINTER_REFERENCE,
        NULL
    );
    for (i = 0; i < rgb->dims; i++)
    {
        srcptr[i] = ((float *)(ycbcr->data)) + ycbcr->size * i;
        dstptr[i] = ((float *)(rgb->data)) + ycbcr->size * i;
    }
    slYCbCrToRGB_AVX(srcptr, dstptr, ycbcr->size);
    return rgb;
}

float *slUpSamplingBlurFilter(float *img, INT32 width, INT32 height) {
    
    INT32 y0, y1;
    INT32 x0, x1;
    INT32 dx, dy;
    INT32 ix, iy;
    
    float *outputImage = NULL;

    INT32 imageSize = width * height;
    slAllocateMemory(outputImage, float*, imageSize);
    slAssert(outputImage, SLEXCEPTION_MALLOC_FAILED, NULL);
    for (y0 = 0; y0 < height; y0 += 2)
    {
        for (x0 = 0; x0 < width; x0 += 2)
        {
            for (iy = 0; iy < 2 && y0 + iy < height; ++iy)
            {
                for (ix = 0; ix < 2 && x0 + ix < width; ++ix)
                {
                    dy = 4 * iy - 2;
                    dx = 4 * ix - 2;
                    x1 = slMin(width - 1,  slMax(0, x0 + dx));
                    y1 = slMin(height - 1, slMax(0, y0 + dy));
                    outputImage[(y0 + iy) * width + x0 + ix] = (float)(slColourSampleClamp(((9.0f * img[y0 * width + x0] + 3.0f * img[y0 * width + x1] + 3.0f * img[y1 * width + x0] + 1.0f * img[y1 * width + x1]) / 16.0f), 0, 255));
                }
            }
        }
    }

    return outputImage;
}

INT32 slChromaUpsampling420(Frame frame, float *Cb, float *Cr)
{
    float *srcCb, *srcCr;
    float *dstCb, *dstCr;

    INT32 iy, ix;
    INT32 height;
    INT32 width;
    INT32 horizontalPadding;

    width  = frame->cols;
    height = frame->row;

    srcCb = Cb;
    srcCr = Cr;
    dstCb = (float *)(frame->data) + frame->size;
    dstCr = dstCb + frame->size;

    horizontalPadding = slIsEven(width) ? 0 : 1;
    for (iy = 0; iy < height; )
    {
        for (ix = 0; ix < width; ix++)
        {
            *dstCb++ = *srcCb;
            *dstCr++ = *srcCr;
            if (slIsOdd(ix))
            {
                srcCb++;
                srcCr++;
            }
        }
        if (++iy < height && slIsOdd(iy))
        {
            memcpy(dstCb, dstCb - width, width);
            memcpy(dstCr, dstCr - width, width);
            if (++iy < height)
            {
                dstCb += width;
                dstCr += width;
            }
        }
        else
        {
            break;
        }
        
        srcCb += horizontalPadding;
        srcCr += horizontalPadding;
    }

    dstCb = slUpSamplingBlurFilter(((float *)frame->data) + 0x1 * frame->size, frame->cols, frame->row);
    dstCr = slUpSamplingBlurFilter(((float *)frame->data) + 0x2 * frame->size, frame->cols, frame->row);

    memcpy(frame->data + 0x1 * frame->size, dstCb, frame->size);
    memcpy(frame->data + 0x2 * frame->size, dstCr, frame->size);

    slReleaseAllocatedMemory(dstCb);
    slReleaseAllocatedMemory(dstCr);

    return 0;
}

INT32 slChromaUpsampling422(Frame frame, float *Cb, float *Cr)
{
    float *srcCb, *srcCr;
    float *dstCb, *dstCr;

    INT32 iy, ix;
    INT32 height;
    INT32 width;
    INT32 horizontalPadding;

    width  = frame->cols;
    height = frame->row;

    srcCb = Cb;
    srcCr = Cr;
    dstCb = (float *)(frame->data) + frame->size;
    dstCr = dstCb + frame->size;

    horizontalPadding = slIsEven(width) ? 0 : 1;
    for (iy = 0; iy < height; iy++)
    {
        for (ix = 0; ix < width;ix++)
        {
            *dstCb++ = *srcCb;
            *dstCr++ = *srcCr;
            if (slIsOdd(ix))
            {
                srcCb++;
                srcCr++;
            }
        }
        srcCb += horizontalPadding;
        srcCr += horizontalPadding;
    }

    dstCb = slUpSamplingBlurFilter(((float *)frame->data) + 0x1 * frame->size, frame->cols, frame->row);
    dstCr = slUpSamplingBlurFilter(((float *)frame->data) + 0x2 * frame->size, frame->cols, frame->row);

    memcpy(frame->data + 0x1 * frame->size, dstCb, frame->size);
    memcpy(frame->data + 0x2 * frame->size, dstCr, frame->size);
    slReleaseAllocatedMemory(dstCb);
    slReleaseAllocatedMemory(dstCr);

    return 0;
}

Frame slFlip(Frame src)
{
    Frame dst;
    DWORD *srcptr, *dstptr;
    int i;

    slAssert(
        (dst = slFrameAllocator(src->cols, src->row, src->dims, src->dtype, NULL)),
        SLEXCEPTION_MALLOC_FAILED,
        NULL
    );

    for (
        i = 0, srcptr = (DWORD *)src->data, dstptr = ((DWORD *)dst->data) + (dst->size - dst->cols);
        i < src->row;
        i++, srcptr += src->cols, dstptr -= src->cols
    )
    {
        memcpy(dstptr, srcptr, src->cols * sizeof(DWORD));
    }

    return dst;
}

Frame slDefaultToRGBA(Frame frame)
{
    Frame ret;
    Frame dst;
    size_t i;
    size_t size;

    slAssert(
        (dst = slFrameAllocator(frame->cols, frame->row, 4, SLFRAME_DTYPE_BYTE, NULL)),
        SLEXCEPTION_MALLOC_FAILED,
        NULL
    );
    slRGBA *color = (slRGBA *)dst->data;

    for (i = 0, size = frame->size; i < size; i++, color++)
    {
        color->r = (BYTE)slColourSampleClamp(((float *)frame->data)[i] * 255, 0, 255);
        color->g = (BYTE)slColourSampleClamp(((float *)frame->data)[i + size * 1] * 255, 0, 255);
        color->b = (BYTE)slColourSampleClamp(((float *)frame->data)[i + size * 2] * 255, 0, 255);
        color->a = ~0;
    }
    ret = slFlip(dst);
    slFrameDeallocator(dst);

    return ret;
}

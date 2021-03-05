#include <slcolorspace.h>

Frame slRGBToYCbCr(Frame rgb)
{
    Frame ycbcr;
    INT32 offset;
    BYTE *R, *G,  *B;
    BYTE *Y, *CB, *CR;
    DWORD ci, size;

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

Frame slYCbCrToRGB(Frame ycbcr)
{
    Frame rgb;
    INT32 offset;
    BYTE *R, *G,  *B;
    BYTE *Y, *CB, *CR;
    DWORD i, size;

    slAssert(
        (ycbcr) &&
        (ycbcr->dims == 0x3) &&
        (rgb = slFrameAllocator(ycbcr->cols, ycbcr->row, ycbcr->dims, SLFRAME_DTYPE_BYTE, NULL)),
        SLEXCEPTION_NULLPOINTER_REFERENCE,
        NULL
    );
    offset = slDataTypeSize(SLFRAME_DTYPE_BYTE);
    i = 0;
    size = rgb->size;

    slRGBToFrameBinder(R, G,  B, rgb,   offset);
    slRGBToFrameBinder(Y, CB, CR, ycbcr, offset);
    YCBCRTORGB(Y, CB, CR, R, G, B,  i, size, offset);

    return rgb;
}

BYTE *slUpSamplingBlurFilter(BYTE *img, INT32 width, INT32 height) {
    
    INT32 y0, y1;
    INT32 x0, x1;
    INT32 dx, dy;
    INT32 ix, iy;
    
    BYTE *imgOut = NULL;

    size_t imageSize = width * height;
    slAssert(
        imgOut = (BYTE *)malloc(imageSize),
        SLEXCEPTION_MALLOC_FAILED,
        NULL
    );

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
                    imgOut[(y0 + iy) * width + x0 + ix] = (9.0f * img[y0 * width + x0] 
                                                        +  3.0f * img[y0 * width + x1]
                                                        +  3.0f * img[y1 * width + x0]
                                                        +  1.0f * img[y1 * width + x1])
                                                        /  16.0f;
                }
            }
        }
    }

  return imgOut;
}

INT32 slChromaUpsampling420(Frame frame, BYTE *Cb, BYTE *Cr)
{
    BYTE *srcCb, *srcCr;
    BYTE *dstCb, *dstCr;

    INT32 iy, ix;
    INT32 height;
    INT32 width;
    INT32 horizontalPadding;

    width  = frame->cols;
    height = frame->row;

    srcCb = Cb;
    srcCr = Cr;
    dstCb = frame->data + frame->size;
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

    dstCb = slUpSamplingBlurFilter(frame->data + 0x1 * frame->size, frame->cols, frame->row);
    dstCr = slUpSamplingBlurFilter(frame->data + 0x2 * frame->size, frame->cols, frame->row);

    memcpy(frame->data + 0x1 * frame->size, dstCb, frame->size);
    memcpy(frame->data + 0x2 * frame->size, dstCr, frame->size);

    slReleaseAllocatedMemory(dstCb);
    slReleaseAllocatedMemory(dstCr);

    return 0;
}

INT32 slChromaUpsampling422(Frame frame, BYTE *Cb, BYTE *Cr)
{
    BYTE *srcCb, *srcCr;
    BYTE *dstCb, *dstCr;

    INT32 iy, ix;
    INT32 height;
    INT32 width;
    INT32 horizontalPadding;

    width  = frame->cols;
    height = frame->row;

    srcCb = Cb;
    srcCr = Cr;
    dstCb = frame->data + frame->size;
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

    dstCb = slUpSamplingBlurFilter(frame->data + 0x1 * frame->size, frame->cols, frame->row);
    dstCr = slUpSamplingBlurFilter(frame->data + 0x2 * frame->size, frame->cols, frame->row);

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
        (dst = slFrameAllocator(frame->cols, frame->row, 4, frame->dtype, NULL)),
        SLEXCEPTION_MALLOC_FAILED,
        NULL
    );
    slRGBA *color = (slRGBA *)dst->data;

    for (i = 0, size = frame->size; i < size; i++, color++)
    {
        color->r = frame->data[i];
        color->g = frame->data[i + size * 1];
        color->b = frame->data[i + size * 2];
        color->a = ~0;
    }
    ret = slFlip(dst);
    slFrameDeallocator(dst);

    return ret;
}

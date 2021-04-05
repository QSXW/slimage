#include "sldct.h"

#if defined( _X86INTRIN_H_INCLUDED ) || defined ( _MSC_VER )
float *slLevelDown(float *sequence, size_t size, int precision)
{
    __m256 ymm0;
    __m256 ymm1;
    __m256 ymm2;

    float *sequenceptr;
    float level;
    size_t iter0;
    
    level =  (float)slPower2(precision - 1);
    ymm2 = _mm256_broadcast_ss(&level);
    for (iter0 = 0, sequenceptr = sequence; iter0 < size; iter0 += SLJPEG_LENGTH_PER_M256)
    {
        ymm0 = _mm256_loadu_ps(sequence + iter0);
        ymm1 = _mm256_sub_ps(ymm0, ymm2);
        _mm256_storeu_ps(sequenceptr, ymm1);
        sequenceptr += SLJPEG_LENGTH_PER_M256;
    }

    return sequence;
}

float *slLevelUp(float *sequence, size_t size, int precision)
{
    __m256 ymm0;
    __m256 ymm1;
    __m256 ymm2;

    float  *sequenceptr;
    float  level;
    size_t iter0;
    size_t  iter1;
    
    level =  (float)slPower2(precision - 1);
    ymm2 = _mm256_broadcast_ss(&level);
    for (iter0 = 0, iter1 = 0, sequenceptr = sequence; iter0 < size; iter0 += SLJPEG_LENGTH_PER_M256, iter1 = 0)
    {
        ymm0 = _mm256_loadu_ps(sequence + iter0);
        ymm1 = _mm256_add_ps(ymm0, ymm2);
        while (iter1 < SLJPEG_LENGTH_PER_M256)
        {
            *sequenceptr++ = slColourSampleClamp(_mm256_ref_ps(ymm1)[iter1], 0, 255);
            iter1++;
        }
    }

    return sequence;
}

INT32 slFastForwardDiscreteConsineTransfrom64(float *src, float *dst, INT32 offset)
{
    const float constants[] = { 0.0, 0.25, 1.0 };
    size_t dctMapIndex;
    size_t iter, u, v;
    float c;
    float *dstptr, *srcptr;
    float sum[64]; 
    
    __m256 ymm0;
    __m256 ymm1;
    __m256 ymm2;
    __m256 ymm3;
    __m256 ymm4;
    __m256 ymm5;
    __m256 ymm6;

    dstptr = dst;
    dctMapIndex = 0;
    c = (float)(1.0 / sqrt(2.0));
    ymm3 = _mm256_broadcast_ss(constants + 1);
    ymm4 = _mm256_broadcast_ss(constants + 2);
    _mm256_ref_ps(ymm4)[0] = c;

    for (u = 0; u < SLJPEG_LENGTH_PER_M256; u++)
    {
        memset(sum, 0x0, 64 * sizeof(float));
        ymm5 = _mm256_broadcast_ss(((u) ? (constants + 2) : (&c)));
        for (v = 0; v < SLJPEG_LENGTH_PER_M256; v++)
        {
            ymm2 = _mm256_broadcast_ss(constants);
            for (iter = 0, srcptr = src; iter < SLJPEG_LENGTH_PER_M256; iter++, dctMapIndex += SLJPEG_LENGTH_PER_M256, srcptr += offset)
            {
                ymm0 = _mm256_loadu_ps(srcptr);
                ymm1 = _mm256_loadu_ps(SLDCTMAP + dctMapIndex);
                ymm2 = _mm256_add_ps(_mm256_mul_ps(ymm0, ymm1), ymm2);
            }
            for (iter = 0; iter < SLJPEG_LENGTH_PER_M256; iter++)
            {
                sum[v] += _mm256_ref_ps(ymm2)[iter];
            }
        }
        ymm6 = _mm256_loadu_ps(sum);
        ymm6 = _mm256_mul_ps(ymm5, _mm256_mul_ps(ymm4, _mm256_mul_ps(ymm6, ymm3)));
        _mm256_storeu_ps(dstptr, ymm6);
        dstptr += SLJPEG_LENGTH_PER_M256;
    }

    return 0;
}

INT32 slFastInverseDiscreteConsineTransfrom64(float *src, float *dst, INT32 offset)
{
    const float constants[] = { 0.0, 0.25, 1.0 };
    size_t dctMapIndex;
    size_t iter;
    size_t u, v;
    float *dstptr, *srcptr;
    float sum[64];

    __m256 ymm0;
    __m256 ymm1;
    __m256 ymm2;
    __m256 ymm3; // constant 0.25
    __m256 ymm4; // Destination value

    dstptr = dst;
    dctMapIndex = 0;
    ymm3 = _mm256_broadcast_ss(constants + 1);

    for (u = 0; u < SLJPEG_LENGTH_PER_M256; u++)
    {
        memset(sum, 0x0, 64 * sizeof(float));
        for (v = 0; v < SLJPEG_LENGTH_PER_M256; v++)
        {
            ymm2 = _mm256_broadcast_ss(constants);
            for (iter = 0, srcptr = src; iter < SLJPEG_LENGTH_PER_M256; iter++, dctMapIndex += SLJPEG_LENGTH_PER_M256, srcptr += offset)
            {
                ymm0 = _mm256_loadu_ps(srcptr);
                ymm1 = _mm256_loadu_ps(SLIDCTMAP + dctMapIndex);
                ymm2 = _mm256_add_ps(_mm256_mul_ps(ymm0, ymm1), ymm2);
            }
            for (iter = 0; iter < SLJPEG_LENGTH_PER_M256; iter++)
            {
                sum[v] += _mm256_ref_ps(ymm2)[iter];
            }
        }
        ymm4 = _mm256_loadu_ps(sum);
        ymm4 = _mm256_mul_ps(ymm4, ymm3);
       _mm256_storeu_ps(dstptr, ymm4);
       dstptr += SLJPEG_LENGTH_PER_M256;
    }

    return 0;
}

void slUniformQuantizer(float *svu, const float *qvu)
{
    size_t iteri;

    __m256 ymm0;
    __m256 ymm1;
    __m256 ymm2;

    if (!qvu) { qvu = slJPEG_LUMINANCE_QT; }
    for (iteri = 0; iteri < 64; iteri += SLJPEG_LENGTH_PER_M256, svu += SLJPEG_LENGTH_PER_M256)
    {
        ymm0 = _mm256_loadu_ps(svu);
        ymm1 = _mm256_loadu_ps(qvu + iteri);
        ymm2 = _mm256_round_ps(_mm256_div_ps(ymm0, ymm1), 0);
        _mm256_storeu_ps(svu, ymm2);
    }
}

void slUniformDequantizer(float *sqvu, const float *qvu)
{
    int iteri;

    __m256 ymm0;
    __m256 ymm1;

    if (!qvu) { qvu = slJPEG_LUMINANCE_QT; }
    for (iteri = 0; iteri < 64; iteri += SLJPEG_LENGTH_PER_M256, sqvu += SLJPEG_LENGTH_PER_M256)
    {
        ymm0 = _mm256_loadu_ps(sqvu);
        ymm1 = _mm256_loadu_ps(qvu + iteri);
        ymm1 = _mm256_mul_ps(ymm0, ymm1);
        _mm256_storeu_ps(sqvu, ymm1);
    }
}

#else
float *slLevelDown(float *src, size_t size, int precision)
{
    size_t i;
    int level;

    level = (int)pow(2, precision - 1);
    for (i = 0; i < size; i++)
    {
        src[i] -= level;
    }

    return src;
}

float *slLevelUp(float *src, size_t size, int precision)
{
    size_t i;
    int level;

    level = (int)pow(2, precision - 1);
    for (i = 0; i < size; i++)
    {
        src[i] += level;
    }

    return src;
}

INT32 slFastForwardDiscreteConsineTransfrom64(float *src, float *dst, INT32 offset)
{
    int dctMapIndex;
    int iter, u, v;
    float c, cu, cv;
    float *dstptr, *srcptr;
    float sum, dct1;

    dstptr = dst;
    dctMapIndex = 0;
    c = 1.0 / sqrt(2.0);
    for (u = 0; u < 8; u++)
    {
        for (v = 0; v < 8; v++)
        {
            cu = u ? 1.0 : c;
            cv = v ? 1.0 : c;
            sum = 0;
            srcptr = src;
            for (iter = 0; iter < 64; iter++)
            {
                dct1 = (*srcptr++) * SLDCTMAP[dctMapIndex++];
                sum += dct1;
            }
            *dstptr++ = cu * cv * sum * 0.25;
        }
    }

    return 0;
}

INT32 slFastInverseDiscreteConsineTransfrom64(float *src, float *dst, INT32 offset)
{
    int dctMapIndex;
    int iteri, iterj;
    float *dstptr, *srcptr;
    float sum, dct1;
    
    dstptr = dst;
    for (iteri = 0, dctMapIndex = 0; iteri < 64; iteri++)
    {
        sum = 0;
        srcptr = src;
        for (iterj = 0; iterj < 64; iterj++)
        {
            dct1 = (*srcptr++) * SLIDCTMAP[dctMapIndex++];
            sum += dct1;
        }
        *dstptr++ =  sum * 0.25;
    }

    return 0;
}

void slUniformQuantizer(float *svu, const float *qvu)
{
    int i;

    i = 0;
    if (!qvu) { qvu = slJPEG_DEFAULTQT; }
    while (i < SLJPEG_BLOCKSIZE)
    {
        svu[i] = round(svu[i] / qvu[i]);
        ++i;
    }
}

void slUniformDequantizer(float *sqvu, const float *qvu)
{
    int i;

    i = 0;
    if (!qvu) { qvu = slJPEG_DEFAULTQT; }
    while (i < SLJPEG_BLOCKSIZE)
    {
        sqvu[i] = sqvu[i] * qvu[i];
        ++i;
    }
}

float *slFastForwardDiscreteConsineTransfrom64Deprecated(float * block)
{
    int u, v, x, y;
    float c, cu, cv;

    float *mat = (float *)malloc(SLJPEG_SEQUENTIAL_BLOCK_SIZE * sizeof(float));
    float *matptr, *blockptr;
    float sum, dct1;

    c = 1.0 / sqrt(2.0);
    matptr = mat;
    for (u = 0; u < SLJPEG_SAMPLEPERLINE; u++)
    {
        for (v = 0; v < SLJPEG_LINE; v++)
        {
            cu = u ? 1.0 : c;
            cv = v ? 1.0 : c;
            sum = 0;
            blockptr = block;
            for (x = 0; x < SLJPEG_SAMPLEPERLINE; x++)
            {
                for (y = 0; y < SLJPEG_LINE; y++)
                {   
                    dct1 = (*blockptr++)
                            * cos(((2.0 * x + 1.0) * u * PI) / 16.0)
                            * cos(((2.0 * y + 1.0) * v * PI) / 16.0);
                    sum += dct1;
                }
            }
            *matptr++ = cu * cv * sum * 0.25;
        }
    }

    return mat;
}

float *slFastInverseDiscreteConsineTransfrom64Deprecated(float *block)
{
    int u, v, x, y;
    float c, cu, cv;

    float *mat = (float *)malloc(SLJPEG_SEQUENTIAL_BLOCK_SIZE * sizeof(float));
    float *matptr, *blockptr;
    float sum, dct1;

    c = 1.0 / sqrt(2.0);
    matptr = mat;
    for (x = 0; x < SLJPEG_SAMPLEPERLINE; x++)
    {
        for (y = 0; y < SLJPEG_LINE; y++)
        {
            sum = 0;
            blockptr = block;
            for (u = 0; u < SLJPEG_SAMPLEPERLINE; u++)
            {
                for (v = 0; v < SLJPEG_LINE; v++)
                {
                    cu = u ? 1.0 : c;
                    cv = v ? 1.0 : c;
                    dct1 = (*blockptr++) * cu * cv
                            * cos(((2.0 * x + 1.0) * u * PI) / 16.0)
                            * cos(((2.0 * y + 1.0) * v * PI) / 16.0);
                    sum += dct1;
                }
            }
            *matptr++ =  sum * 0.25;
        }
    }

    return mat;
}

#endif /* _X86INTRIN_H_INCLUDED */

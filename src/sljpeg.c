#include <malloc.h>
#include <math.h>
#include "sljpeg.h"

RawJpeg * RawJpegRead(Stream stream)
{
    RawJpeg    *jpeg;
    RawJpeg    *jpegNode;
    RawJpeg    **join;

    WORD   segmentLen;
    size_t htTableIndex;
    size_t qtTableIndex;
    BYTE *streamEndPos;

    jpeg = NULL;
    join = NULL;
    streamEndPos = slStreamEndPosition(stream);
    slRawJpegAllocator(jpegNode);
    if ((jpegNode))
    {  
        segmentLen   = 0;
        htTableIndex = 0;
        qtTableIndex = 0;
        slRawJpegSetNullptr(jpegNode);
        while (stream->pos < streamEndPos)
        {
            if (*stream->pos == JPEG_MARKER_MASK)
            {
                switch (*(stream->pos + 1))
                {
                case SLJPEG_CODE_SOI:
                    slRawJpegReadSegment(SOI, jpegNode->_SOI, CODE_SIZE, stream);
                    break;

                case SLJPEG_CODE_DQT:
                    slRawJpegGetSegmentLength(stream, segmentLen);
                    if (qtTableIndex < SLJPEG_TABLESIZE)
                    {
                        slRawJpegReadSegment(DQT, jpegNode->_DQT[qtTableIndex], segmentLen, stream);
                        qtTableIndex++;
                    }
                    else
                    {
                        stream->pos++;
                        /* Read a one more than 4 tables, it may be ignored. */
                    }
                    break;

                case SLJPEG_CODE_SOF0:
                case SLJPEG_CODE_SOF2:
                    // printf("\n\nWas called SOF0\n\n");
                    slRawJpegGetSegmentLength(stream, segmentLen);
                    slRawJpegReadSegment(SOF0, jpegNode->_SOF, segmentLen, stream);
                    break;

                case SLJPEG_CODE_DHP:
                    slRawJpegGetSegmentLength(stream, segmentLen);
                    slRawJpegReadSegment(DHP, jpegNode->_DHP, segmentLen, stream);
                    break;

                case SLJPEG_CODE_EXP:
                    slRawJpegGetSegmentLength(stream, segmentLen);
                    slRawJpegReadSegment(EXP, jpegNode->_EXP, segmentLen, stream);
                    break;

                case SLJPEG_CODE_DHT:
                    slRawJpegGetSegmentLength(stream, segmentLen);
                    if (htTableIndex < SLJPEG_TABLESIZE)
                    {
                        slRawJpegReadSegment(DHT, jpegNode->_DHT[htTableIndex], segmentLen, stream);
                        htTableIndex++;
                    }
                    else
                    {
                        stream->pos++;
                        /* Read a one more than 4 tables, it may be ignored. */
                    }
                    break;
                    
                case SLJPEG_CODE_DAC:
                    slRawJpegGetSegmentLength(stream, segmentLen);
                    slRawJpegReadSegment(DAC, jpegNode->_DAC, segmentLen, stream);
                    break;

                case SLJPEG_CODE_DRI:
                    slRawJpegGetSegmentLength(stream, segmentLen);
                    slRawJpegReadSegment(slDRI, jpegNode->_DRI, segmentLen, stream);
                    break;

                case SLJPEG_CODE_COM:
                    slRawJpegGetSegmentLength(stream, segmentLen);
                        slRawJpegReadSegment(COM, jpegNode->_COM, segmentLen, stream);
                    break;

                case SLJPEG_CODE_APP0:
                    slRawJpegGetSegmentLength(stream, segmentLen);
                    slRawJpegReadSegment(APP0, jpegNode->_APP, segmentLen, stream);
                    break;

                case SLJPEG_CODE_SOS:
                    slRawJpegGetSegmentLength(stream, segmentLen);
                    // slRawJpegReadSegment(SOS, jpegNode->_SOS, segmentLen, stream);
                    jpegNode->_SCAN = stream->pos;
                    stream->pos += segmentLen;
                    if (jpegNode->_SOF->code[1] == SLJPEG_CODE_SOF2)
                    {
                        return jpegNode;
                    }
                    break;

                case SLJPEG_CODE_DNL:
                    slRawJpegGetSegmentLength(stream, segmentLen);
                    slRawJpegReadSegment(DNL, jpegNode->_DNL, segmentLen, stream);
                    break;

                case SLJPEG_CODE_EOI:
                    slRawJpegReadSegment(EOI, jpegNode->_EOI, CODE_SIZE, stream);
                    if (!jpeg) 
                    {
                        jpeg = jpegNode;
                    }
                    else
                    {
                        *join = jpegNode;
                    }
                    join = &(jpegNode->next);
                    slRawJpegAllocator(jpegNode);
                    if ((jpegNode))
                    {
                        slRawJpegSetNullptr(jpegNode);
                        segmentLen = 0;
                        htTableIndex = 0;
                        qtTableIndex = 0;
                    }
                    break;

                default:
                    stream->pos++;
                    break;
                }
            }
            else 
            {
                stream->pos++; 
            }
        }
    }
    else
    {
        SaveLog((char *)stream->fname->data, JPEG_SCAN_FAILED);
    }

    slReleaseAllocatedMemory(jpegNode);
    return jpeg;
}


void _DestroyRawJpeg(RawJpeg *rawJpeg)
{
    slRawJpegSetNullptr(rawJpeg);
    if (rawJpeg->next)
    {
        _DestroyRawJpeg(rawJpeg->next);
    }
    slReleaseAllocatedMemory(rawJpeg);
}

Frame JpegRead(Stream fileStream)
{
    Frame frame;
    RawJpeg *rawJpeg;

    frame = NULL;
    if ((rawJpeg = RawJpegRead(fileStream)))
    {
        // SaveLog((char *)(fileStream->fname->data), JPEG_OPEN_SUCCEED);
        frame = JpegDecodingProcess(rawJpeg, fileStream);
    }

    DestroyRawJpeg(rawJpeg);
    return frame;
}

slJpegComponent *SLJpegComponentAllocator(BYTE capacity)
{
    slJpegComponent *component;

    capacity = capacity ? (capacity * sizeof(slJpegComponent)) : sizeof(slJpegComponent);
    slAllocateMemory(component, slJpegComponent *, capacity);
    if ((component)) { memset(component, 0x0, capacity); }

    return component;
}

slJpeg *slJpegAllocator()
{
    slJpeg *jpeg;

    slAllocateMemory(jpeg, slJpeg *, sizeof(slJpeg));
    if ((jpeg))
    {
        memset(jpeg, 0x0, sizeof(slJpeg));
    }

    return jpeg;
}

void __slJpegDeallocator(slJpeg *jpeg)
{
    INT32 i;

    for (i = 0; i < SLJPEG_JPEGHUFFTABLESIZE; i++)
    {
        slJpegHuffTableDeallocator(jpeg->dcTable[i]);
        slJpegHuffTableDeallocator(jpeg->acTable[i]);
        slJpegQuantizationTableDeallocator(jpeg->qtTable[i]);
    }
    slJpegComponentDeallocator(jpeg->components);
    free(jpeg);
}

slJpegHuffTable *slJpegHuffTableAllocator(BYTE *bits, BYTE *values, INT32 length)
{
    slJpegHuffTable *hufftable;

    slAllocateMemory(hufftable, slJpegHuffTable *, sizeof(slJpegHuffTable));
    if ((hufftable))
    {
        memset(hufftable, 0x0, sizeof(slJpegHuffTable));
        if ((bits))
        {
            memcpy(hufftable->bits + 1, bits, 16);
        }
        if ((values))
        {
            memcpy(hufftable->huffval, values, length);
        }
    }

    return hufftable;
}

/* Generate size table */
INT32 slGenerateHUFFSIZE(BYTE *BITS, INT32 *HUFFSIZE, INT32 *lastk)
{
    INT32 i, j, k;

    for (i = 1, j = 1, k = 0; i <= 16; )
    {
        if (j > BITS[i])
        {
            i++;
            j = 1;
        }
        else
        {
            HUFFSIZE[k++] = i;
            j++;
        }
    }
    HUFFSIZE[k] = 0;
    *lastk = k;

    return 0;
}

INT32 slGenerateHUFFCODE(INT32 *HUFFSIZE, INT32 *HUFFCODE)
{
    INT32 k;
    INT32 si, code;

    for (k = 0, code = 0, si = HUFFSIZE[0]; ; )
    {
        HUFFCODE[k] = code;
        k++;
        code++;

        if (HUFFSIZE[k] == si)
        {
            continue;
        }
        else
        {
            if (HUFFSIZE[k] == 0)
            {
                break;
            } 
            do {
                code = code << 1;
                si++;
            } while (HUFFSIZE[k] != si);
        }
    }

    return 0;
}

INT32 slGenerateDecoderTable(slJpegHuffTable *huffTable)
{
    INT32 lastk;
    INT32 i, j;

    INT32 HUFFSIZE[SLJPEG_HUFFVALSIZE] = { 0 };
    INT32 HUFFCODE[SLJPEG_HUFFVALSIZE] = { 0 };

    /* Generate size table */
    slGenerateHUFFSIZE(huffTable->bits, HUFFSIZE, &lastk);
    /* Generate code table */
    slGenerateHUFFCODE(HUFFSIZE, HUFFCODE);

    for (i = 0, j = 0; ; )
    {
        i++;
        if (i > 16)
        {
            break;
        }
        if (huffTable->bits[i] == 0)
        {
            huffTable->MAXCODE[i] = -1;
        }
        else
        {
            huffTable->VALPTR[i] = j;
            huffTable->MINCODE[i] = HUFFCODE[j];
            j = (j + ((INT32)huffTable->bits[i])) - 1;
            huffTable->MAXCODE[i] = HUFFCODE[j];
            j++;
        }
    }

    return 0;
}

INT32 SLJpegHuffTableBinder(slJpeg *jpeg, DHT *const *definedHuffTables)
{
    size_t ci, cj;
    INT32  length, tableEnd, tableIndex;
    DHT*   definedHuffTable;
    slJpegHuffTable **huffTable;

    huffTable = NULL;
    length = tableEnd = 0x0;
    for (ci = 0; ci < SLJPEG_TABLESIZE; ci++)
    {
        if (definedHuffTables[ci] != NULL)
        {
            definedHuffTable = definedHuffTables[ci];
            if (IsIntelMode())
            {
                MotorolaToIntelMode(&(definedHuffTable->length), &tableEnd, sizeof(definedHuffTable->length));
            }
            else
            {
                tableEnd = definedHuffTable->length;
            }
            tableEnd -= (sizeof(definedHuffTable->code) + sizeof(definedHuffTable->length));
            while ((((BYTE *)definedHuffTable) - ((BYTE *)definedHuffTables[ci])) < tableEnd)
            {
                cj = length = 0;
                while (cj < 16)
                {
                    length += definedHuffTable->bits[cj++];
                }
                tableIndex = (0x0f & definedHuffTable->selector);
                huffTable  = (0x10 & definedHuffTable->selector) ? (&(jpeg->acTable[tableIndex])) : (&(jpeg->dcTable[tableIndex]));
                if (tableIndex < SLJPEG_JPEGHUFFTABLESIZE)
                {
                    if ((*huffTable) != NULL)
                    {
                        slJpegHuffTableDeallocator(*huffTable);
                    }
                    *huffTable = slJpegHuffTableAllocator(definedHuffTable->bits, definedHuffTable->values, length);
                    slGenerateDecoderTable(*huffTable);
                }
                definedHuffTable = (DHT *)(((BYTE *)definedHuffTable) + sizeof(definedHuffTable->selector) + sizeof(definedHuffTable->bits) + length);
            }
        }
    }
    
    return 0;
}

JpegQuantizationTable *slJpegQuantizationTableAllocator(INT32 precision, BYTE *elements, INT32 *length)
{
    INT32 i, offset;
    JpegQuantizationTable *quantizationTable;

    *length = 64 * (precision + 1);
    offset = precision ? 2 : 1;

    slAllocateMemory(quantizationTable, JpegQuantizationTable *, sizeof(JpegQuantizationTable) + *length * sizeof(float));
    slAssert ((quantizationTable), SLEXCEPTION_NULLPOINTER_REFERENCE, NULL);
    memset(quantizationTable, 0x0, sizeof(JpegQuantizationTable) + *length * sizeof(float));
    for (i = 0; i < SLJPEG_SEQUENTIAL_BLOCK_SIZE; i++)
    {
        quantizationTable->elements[i] = (float)*elements;
        elements += offset;
    }
    quantizationTable->precision = precision ? 10 : 8;

    return quantizationTable;
}

void SLJpegQuantizationTableBinder(slJpeg *jpeg, DQT *const *definedQuatizationTables)
{
    DQT    *definedQuatizationTable;
    INT32  ci, length, tableEnd, tableIndex;

    length = tableEnd = 0x0;
    for (ci = 0; ci < SLJPEG_TABLESIZE; ci++)
    {
        if (definedQuatizationTables[ci])
        {
            definedQuatizationTable = definedQuatizationTables[ci];
            if (IsIntelMode())
            {
                MotorolaToIntelMode(&definedQuatizationTable->length, &tableEnd, sizeof(definedQuatizationTable->length));
            }
            else
            {
                length = definedQuatizationTable->length;
            }

            tableEnd -= sizeof(definedQuatizationTable->code) + sizeof(definedQuatizationTable->length);
            while ((((BYTE *)definedQuatizationTable) - ((BYTE *)definedQuatizationTables[ci])) < tableEnd)
            {
                tableIndex = 0x0f & definedQuatizationTable->selector;
                if (tableIndex < SLJPEG_JPEGHUFFTABLESIZE)
                {
                    if ((jpeg->qtTable[tableIndex]))
                    {
                        slJpegComponentDeallocator(jpeg->qtTable[tableIndex]);
                    }
                    jpeg->qtTable[tableIndex] = slJpegQuantizationTableAllocator((0xf0 & definedQuatizationTable->selector) >> 4, definedQuatizationTable->element, &length);
                }
                definedQuatizationTable = (DQT *)(((BYTE *)definedQuatizationTable) + sizeof(definedQuatizationTable->selector) + length);
            }
        }
    }
}

INT32 slJpegScanComponentBinder(slJpeg *jpeg, ScanComponent  *scanComponent, INT32 scanNum)
{
    BYTE ci;

    for (ci = 0; ci < scanNum; ci++)
    {
        jpeg->components[ci].acIndex = 0x0F & scanComponent[ci].htSelector;
        jpeg->components[ci].dcIndex = (0xF0 & scanComponent[ci].htSelector) >> 4;
    }

    return 0;
}

INT32 slJpegComponentsBinder(slJpeg *jpeg, BYTE sofCode, FrameComponent *frameComponent)
{
    BYTE h, v;
    BYTE ci;

    INT32   mcu, MCUNumber;

    if (sofCode == SLJPEG_CODE_SOF2)
        jpeg->isProgressive = TRUE;

    MCUNumber = 0;
    if ((jpeg->components = SLJpegComponentAllocator(jpeg->colorComponent)))
    {
        for (ci = 0; ci < jpeg->colorComponent; ci++)
        {
            h = (0xf0 & frameComponent[ci].samplingFactor) >> 4;
            v =  0x0F & frameComponent[ci].samplingFactor;
            jpeg->dataPerMCU += h * v;
            jpeg->components[ci].h = h;
            jpeg->components[ci].v = v;
            jpeg->components[ci].qtIndex = frameComponent[ci].qtSelector;
            jpeg->maxHorizontalSamplingFactor = h > jpeg->maxHorizontalSamplingFactor ? h : jpeg->maxHorizontalSamplingFactor;
            jpeg->maxVerticalSamplingFactor = v > jpeg->maxVerticalSamplingFactor ? v : jpeg->maxVerticalSamplingFactor;
        }
        for (ci = 0; ci < jpeg->colorComponent; ci++)
        {
            jpeg->components[ci].x = (INT32)ceil((double)jpeg->width * ((float)jpeg->components[ci].h / (float)jpeg->maxHorizontalSamplingFactor));
            jpeg->components[ci].y = (INT32)ceil((double)jpeg->height * ((float)jpeg->components[ci].v / (float)jpeg->maxVerticalSamplingFactor));

            jpeg->components[ci].paddingx = slMod8(jpeg->components[ci].x) ? 8 - slMod8(jpeg->components[ci].x) : 0;
            jpeg->components[ci].paddingy = slMod8(jpeg->components[ci].y) ? 8 - slMod8(jpeg->components[ci].y) : 0;

            mcu = (jpeg->components[ci].x + jpeg->components[ci].paddingx) / 8;
            jpeg->components[ci].paddingMcux = (mcu % jpeg->components[ci].h);
            jpeg->components[ci].mcux = mcu + jpeg->components[ci].paddingMcux;;

            mcu = (jpeg->components[ci].y + jpeg->components[ci].paddingy) / 8;
            jpeg->components[ci].paddingMcuy = (mcu % jpeg->components[ci].v);
            jpeg->components[ci].mcuy = mcu + jpeg->components[ci].paddingMcuy;;

            MCUNumber = jpeg->components[ci].mcux  * jpeg->components[ci].mcuy;
            jpeg->MCUNumber += MCUNumber;

            jpeg->components[ci].sampleCount = ((size_t)MCUNumber) * 64;
            jpeg->sampleCount += jpeg->components[ci].sampleCount;
        }
        jpeg->MCUNumber /= jpeg->dataPerMCU;
    }

    return 0;
}

void BindJpegToRawJpeg(slJpeg *jpeg, const RawJpeg *rawJpeg) 
{
    if (IsMotorolaMode())
    {
        jpeg->width = (DWORD)rawJpeg->_SOF->width;
        jpeg->height = (DWORD)rawJpeg->_SOF->height;
    }
    else
    {
        MotorolaToIntelMode(&(rawJpeg->_SOF->width), &(jpeg->width), sizeof(rawJpeg->_SOF->width));
        MotorolaToIntelMode(&(rawJpeg->_SOF->height), &(jpeg->height), sizeof(rawJpeg->_SOF->width));
    }
    jpeg->samplePrecision = rawJpeg->_SOF->precision;
    jpeg->colorComponent  = rawJpeg->_SOF->componentNum;

    /* Bind and calculate conponets' data */
    if (jpeg->colorComponent)
    {
        slJpegComponentsBinder(jpeg, rawJpeg->_SOF->code[1], rawJpeg->_SOF->components);
    }

    /* Bind Huffman table */
    SLJpegHuffTableBinder(jpeg, rawJpeg->_DHT);

    /* Bind Quantization table */
    SLJpegQuantizationTableBinder(jpeg, rawJpeg->_DQT);
}

JpegEncodingHuffTable *JpegEncodingHuffTableAllocator(slJpegHuffTable *huffTable)
{
    JpegEncodingHuffTable *encodingHuffTable;
    INT32 i, k, lastk;
    INT32 huffsize[SLJPEG_HUFFVALSIZE] = { 0 };
    INT32 huffcode[SLJPEG_HUFFVALSIZE] = { 0 };
        
    slAllocateMemory(encodingHuffTable, JpegEncodingHuffTable *, sizeof(JpegEncodingHuffTable));
    if ((encodingHuffTable))
    {
        memset(encodingHuffTable, 0x0, sizeof(JpegEncodingHuffTable));
        /* Generate size table */
        slGenerateHUFFSIZE(huffTable->bits, huffsize, &lastk);

        /* Generate code table */
        slGenerateHUFFCODE(huffsize, huffcode);

        /* generates the Huffman codes in symbol value order. */
        for (k = 0; k < lastk; k++)
        {
            i = huffTable->huffval[k];
            encodingHuffTable->ehufco[i] = huffcode[k];
            encodingHuffTable->ehufsi[i] = huffsize[k];
        }
    }

    return encodingHuffTable;
}

slJpegScanBitStream *slJpegScanBitStreamAllocator(BYTE *byteSequence, BYTE *sequenceEnd)
{
    slJpegScanBitStream *bitStream;

    slAllocateMemory(bitStream, slJpegScanBitStream*, sizeof(slJpegScanBitStream));
    slAssert(
        bitStream,
        SLEXCEPTION_MALLOC_FAILED,
        NULL
    );
    memset(bitStream, 0x0, sizeof(slJpegScanBitStream));
    bitStream->start = byteSequence;
    bitStream->end   = sequenceEnd;
    bitStream->pos   = byteSequence;
    slJpegScanBitStreamReset(bitStream, 0);
    if ((*byteSequence == 0xff) && !*(bitStream->pos))
    {
        bitStream->pos += 1;
    }

    return bitStream;
}

INT32 slNextBitFromScanBitStream(slJpegScanBitStream *bitStream)
{
    INT32 bit;

    if (bitStream->count > 0)
    {
        bit = (bitStream->byte) >> 7;
        bitStream->count--;
        bitStream->byte <<= 1;
    }
    else
    {
#ifdef SL_DEBUG_ON_STATE
        putchar(32);
#endif
        slJpegScanBitStreamReset(bitStream, 0);

        if (bitStream->byte != 0xff)
        {
            bit = slNextBitFromScanBitStream(bitStream);
        }
        else if (!(slJpegScanBitStreamNextByte(bitStream)))
        {
            slJpegScanBitStreamSkipByte(bitStream, 1);
            bit = slNextBitFromScanBitStream(bitStream);
        }
        else
        {
            bit = -1;
        }
    }

#ifdef SL_DEBUG_ON_STATE
    printf("%d", bit);
#endif

    return bit;
}


INT32 slDECODE(slJpegScanBitStream *bitStream, slJpegHuffTable *hufftable)
{
    INT32 I, J;
    INT32 CODE;
    INT32 BIT;
    INT32 value;

    I     = 1;
    CODE  = 0;
    value = SLJPEG_ENDOFSCAN;
    while ((BIT = slNextBitFromScanBitStream(bitStream)) != SLJPEG_ENDOFSCAN)
    {
        CODE = (CODE << 1) + BIT;
        if (CODE > hufftable->MAXCODE[I])
        {
            I++;
        }
        else
        {
            J = hufftable->VALPTR[I];
            J = J + CODE - hufftable->MINCODE[I];
            value = hufftable->huffval[J];
            break;
        }
    }

    return value;
}

INT32 slRECIEVE(INT32 SSSS, INT32 *V, slJpegScanBitStream *bitStream)
{
    INT32 I;
    INT32 BIT;

    for (I = 0, *V = 0; I != SSSS; )
    {
        I++;
        if ((BIT = slNextBitFromScanBitStream(bitStream)) == -1)
        {
            return -1;
        }
        *V= (*V << 1) + BIT;
    }

    return 0;
}

void slEXTEND(INT32 *V, INT32 T)
{
    INT32 VT;

    VT = T ? (1 << (T - 1)) : T;
    if (*V < VT)
    {
        VT = ((~(DWORD)0) << T) + 1;
        *V = *V + VT;
    }
}

INT32 slEntroyDecodingProcess(slJpegScanBitStream *bitStream, float *ZZSequence, slJpegHuffTable *dcTable, slJpegHuffTable *acTable, float *PRED)
{
    INT32  K;
    INT32  DIFF;
    INT32  RS;
    INT32  RRRR, SSSS;
    INT32  state;

    // Decode DC Coefficients
    if ((SSSS = slDECODE(bitStream, dcTable)) == -1)
    {
        return -1;
    }

    state = slRECIEVE(SSSS, &DIFF, bitStream);
    if (state == -1)
    {
        return 2;
    }

    slEXTEND(&DIFF, SSSS);
    ZZSequence[0] = DIFF  + *PRED;
    *PRED = ZZSequence[0];

    // Decode_AC_Coefficients
    for (K = 1; K < 64; )
    {
        if ((RS = slDECODE(bitStream, acTable)) == -1)
        {
            return -1;
        }
        SSSS = RS % 16;
        RRRR = RS >> 4;

        if (SSSS == 0)
        {
            if (RRRR != 0xF)
            {
                break;
            }
            else
            {
                K += 0x10;
            }
        }
        else
        {
            K = K + RRRR;
            state = slRECIEVE(SSSS, &DIFF, bitStream);

            if (state == -1)
            {
                return 2;
            }
            slEXTEND(&DIFF, SSSS);
            ZZSequence[K] = (float)DIFF;
            K++;
        }
    }

    return 0;
}


INT32 slJpegSequentialModeDecodeScan(slJpeg *jpeg, slJpegScanBitStream *bitStream, float **componentPosition, INT32 componentsInScan)
{
    INT32 state;
    INT32 MCUI;
    INT32 MCUNumber;
    INT32 decodedMCUNumber;

    INT32 h,  hmax;
    INT32 v,  vmax;
    INT32 ci, cmax;

    float *PRED;
    float *previousPosition;
    INT32 *horizontalDataUnits;

    // float *start = componentPosition[0];
    slJpegComponent *components;

    slAllocateMemory(PRED, float*, ((size_t)componentsInScan + 1) * sizeof(float));
    slAllocateMemory(horizontalDataUnits, INT32*, ((size_t)componentsInScan + 1) * sizeof(INT32));
    slAssert(
        (PRED) && (horizontalDataUnits),
        SLEXCEPTION_MALLOC_FAILED,
        -1
    );
    memset(PRED , 0, componentsInScan * sizeof(float));
    memset(horizontalDataUnits, 0, componentsInScan * sizeof(int));

    cmax             = componentsInScan;
    MCUNumber        = jpeg->MCUNumber;
    components       = jpeg->components;
    previousPosition = 0;
    /*  Decode MCU */
    for (
        MCUI = 0, decodedMCUNumber = 0;
        MCUI < MCUNumber;
        MCUI++, decodedMCUNumber++ )
    {
        /* Decode Data Units */
        for (ci = 0; ci < cmax; ci++)
        {
            for (
                h = 0, v = 0, hmax = components[ci].h, vmax = components[ci].v;
                h != hmax && v != vmax;
            )
            {
                #ifdef SL_DEBUG_DISPLAY_DECODED_DETAILS
                printf("MCUI = %d   MCU number = %d  m = %d PRED = %f\n", N, MCUNumber, m, PRED);
                printf("h = %d, v = %d, ci = %d\n", h + 1, v + 1, ci);
                #endif

                if ((state = slEntroyDecodingProcess(
                                bitStream,
                                componentPosition[ci],
                                jpeg->dcTable[components[ci].dcIndex],
                                jpeg->acTable[components[ci].acIndex],
                                PRED + ci
                            )
                    ) != 0)
                {
                    if (slJpegScanBitStreamNextByte(bitStream) >= 0xd0 && slJpegScanBitStreamNextByte(bitStream) <= 0xd7)
                    {
                        memset(PRED , 0,  componentsInScan * sizeof(float));
                        MCUNumber -= MCUI;
                        MCUI = -1;
                        ci = cmax;
                        slJpegScanBitStreamReset(bitStream, 1);
                        break;
                    }
                    else
                    {
                        ci   = cmax;
                        MCUI = MCUNumber;
                        break;
                    }
                }
                else
                {
                    #ifdef SL_DEBUG_ON_STATE
                    putchar(10);
                    #endif

                    #ifdef SL_DEBUG_DISPLAY_EVERY_DECODED_ZZSEQUENCE
                    DisplayFloatMatrix((componentPosition[ci]), 64, ci == 0 ? "Y: \n" : (ci == 1 ? "CB:\n" : "CR:\n") , 8);
                    #endif

                    componentPosition[ci] += SLJPEG_SEQUENTIAL_BLOCK_SIZE;
                    if (++h == hmax && ++v && vmax == 1)
                    {
                        break;
                    }
                    if (h == hmax && v != vmax)
                    {
                        previousPosition = componentPosition[ci];
                        h = 0;
                        componentPosition[ci] += ((size_t)v * ((size_t)components[ci].mcux - (size_t)hmax) * 64);
                    }
                    else if (h == hmax && v == vmax && horizontalDataUnits[ci] < (components[ci].mcux - hmax))
                    {
                        componentPosition[ci] = previousPosition;
                    }
                    else if (h == hmax && v == vmax  && horizontalDataUnits[ci] >= components[ci].mcux - hmax)
                    {
                        previousPosition = componentPosition[ci];
                        horizontalDataUnits[ci] = -1 * hmax;
                    }
                }
            }
            horizontalDataUnits[ci] += hmax;
        }
    }

    slReleaseAllocatedMemory(PRED);
    slReleaseAllocatedMemory(horizontalDataUnits);
    return 0;
}

INT32 slJpegProgressiveModeDecodeScan(slJpeg *jpeg, slJpegScanBitStream *bitStream, float **componentPosition, INT32 componentsInScan)
{
    // TODO
    return 0;
}

void slUnZigZag(float *src, float *dst)
{
    DWORD i;
    for (i = 0; i < SLJPEG_SEQUENTIAL_BLOCK_SIZE; i++)
    {
        dst[i] = src[SLJPEG_ZIGZAG_INDEX[i]];
    }
}

INT32 NormalizeBlockAsMatrix(float *src, float *dst, size_t width, size_t height, INT32 horizontalComplement, INT32 verticalComplement, INT32 alignedLength)
{
    size_t  iteri;
    size_t  iterj;
    
    INT32  horizontalCounter;
    INT32  verticalCounter;
    float  *srcptr;
    float  *dstptr;
    float  *presrcptr;
    INT32  offset;

    offset = (alignedLength * alignedLength) - alignedLength;

    for (iteri = 0, verticalCounter = 0, dstptr = dst, presrcptr = srcptr = src; iteri < height; iteri++)
    {
        for (iterj = 0, horizontalCounter = 0; iterj < width; iterj++, horizontalCounter++)
        {
            if (horizontalCounter == alignedLength)
            {
                horizontalCounter = 0;
                srcptr += offset;
            }
            *dstptr++ = *srcptr++;
        }

        if (++verticalCounter == alignedLength)
        {
            verticalCounter = 0;
            srcptr += horizontalComplement;
            presrcptr = srcptr;
        }
        else
        {
            presrcptr += alignedLength;
            srcptr = presrcptr;
        }
    }

    return 0;
}

INT32 slJpegSequentialModeDecodeComponent(float *dst, slJpegComponent *component, size_t sampleCount, float *ZZSequence, float *quantizationTable)
{
    float *dctSequence;
    float *blockSequence;

    size_t i;

    slAllocateMemory(blockSequence, float*, sampleCount * sizeof(float));
    slAllocateMemory(dctSequence, float*, sampleCount * sizeof(float));
    slAssert((blockSequence) && (dctSequence), SLEXCEPTION_MALLOC_FAILED, -1);
    memset(blockSequence, 0x0, sampleCount * sizeof(float));
    
    for (i = 0; i < sampleCount; i += SLJPEG_SEQUENTIAL_BLOCK_SIZE)
    {
        slUniformDequantizer(
            ZZSequence + i,
            quantizationTable
        );
        slUnZigZag(
            ZZSequence + i,
            blockSequence + i
        );
        slFastInverseDiscreteConsineTransfrom64(
            blockSequence + i,
            dctSequence + i,
            8
        );
    }
    slReleaseAllocatedMemory(blockSequence);
    slLevelUp(
        dctSequence,
        sampleCount,
        8
    );
    NormalizeBlockAsMatrix(
        dctSequence,
        dst,
        component->x,
        component->y,
        component->paddingMcux * SLJPEG_SEQUENTIAL_BLOCK_SIZE + component->paddingx,
        component->paddingMcuy * SLJPEG_SEQUENTIAL_BLOCK_SIZE + component->paddingy,
        8
    );
    slReleaseAllocatedMemory(dctSequence);
    
    return 0;
}

INT32 slJpegChromaUpSampling(Frame frame, slJpeg *jpeg)
{
    float *cb, *cr;

    slAllocateMemory(cb, float*, jpeg->components[1].sampleCount + 8);
    slAllocateMemory(cr, float*, jpeg->components[2].sampleCount + 8);
    slAssert(
        (cb) && (cr),
        SLEXCEPTION_MALLOC_FAILED,
        -1
    );

    memcpy(cb, frame->data + frame-> size * 0x1, jpeg->components[1].sampleCount);
    memcpy(cr, frame->data + frame-> size * 0x2, jpeg->components[2].sampleCount);

    if (jpeg->components[0].h == 2 && jpeg->components[0].v == 2)
    {
        slChromaUpsampling420(frame, cb, cr);
    }
    else if (jpeg->components[0].h == 2 && jpeg->components[0].v == 1)
    {
        slChromaUpsampling422(frame, cb, cr);
    }
    
    slReleaseAllocatedMemory(cb);
    slReleaseAllocatedMemory(cr);
    return 0;
}

INT32 slJpegDecodeScan(slJpeg *jpeg, BYTE *byteSequence, BYTE *byteSequenceEnd, float **componentPositions)
{
    SOS                 *startOfScan;
    ProgressiveMeta     *progressiveMeta;
    slJpegScanBitStream *bitStream;
    INT32 (*scanDecoder)(slJpeg *, slJpegScanBitStream *, float **, INT32);

    scanDecoder = slJpegSequentialModeDecodeScan;
    while (byteSequence < byteSequenceEnd)
    {
        if (*byteSequence == 0xFF && *(byteSequence + 1) == SLJPEG_CODE_SOS)
        {
            startOfScan = (SOS *)byteSequence;
            slJpegScanComponentBinder(jpeg, startOfScan->componentSelector, startOfScan->componentNum);
            if (IsIntelMode()) 
            {
                MotorolaToIntelMode2(&(startOfScan->length));
            }
            byteSequence += (size_t)startOfScan->length + CODE_SIZE;
            progressiveMeta = (ProgressiveMeta *)(((BYTE *)startOfScan) + startOfScan->length + CODE_SIZE - sizeof(ProgressiveMeta));
            if ((jpeg->isProgressive))
            {
                scanDecoder = slJpegProgressiveModeDecodeScan;
                jpeg->Ss = progressiveMeta->spectralSelectionStart;
                jpeg->Se = progressiveMeta->spectralSelectionEnd;
                jpeg->Al = (progressiveMeta->successiveApproximation     ) & 0x0f;
                jpeg->Ah = (progressiveMeta->successiveApproximation >> 4) & 0x0f;
            }
            slAssert(
                bitStream = slJpegScanBitStreamAllocator(byteSequence, byteSequenceEnd),
                SLEXCEPTION_MALLOC_FAILED,
                -1
            ); 
            scanDecoder(
                jpeg,
                bitStream,
                componentPositions,
                startOfScan->componentNum
            );
            byteSequence = bitStream->pos;
            slJpegScanBitStreamDeallocator(bitStream);
        }
        else
        {
            byteSequence++;
        }
    }

    return 0;
}

Frame JpegDecodingProcess(const RawJpeg *rawJpeg, Stream stream)
{
    slJpeg *jpeg;
    Frame frame, rgbframe;

    INT32 ci;
    float *ZZSequence;
    float **componentsPosition;
    float **componentsOffsetPosition;

    slAssert (
           (rawJpeg->_SOF) 
        && (rawJpeg->_SOF->code[1] == SLJPEG_CODE_SOF0 || rawJpeg->_SOF->code[1] == SLJPEG_CODE_SOF1)
        && (jpeg = slJpegAllocator()),
        SLEXCEPTION_NULLPOINTER_REFERENCE,
        NULL
    );

    // #1 Decoder setup
    while (rawJpeg->next) { rawJpeg = rawJpeg->next; }
    // #2 Up to here, the data of SOF0 and SOS should have been reached.
    BindJpegToRawJpeg(jpeg, rawJpeg);

    // #3 Decode Scan
    slAllocateMemory(ZZSequence, float*, jpeg->sampleCount * sizeof(float));
    slAllocateMemory(componentsPosition, float **, jpeg->colorComponent * sizeof(float *));
    slAllocateMemory(componentsOffsetPosition, float **, jpeg->colorComponent * sizeof(float *));
    slAssert(
           (frame = slFrameAllocator(jpeg->width, jpeg->height, jpeg->colorComponent, SLFRAME_DTYPE_FLOAT, NULL))
        && (ZZSequence)
        && (componentsPosition)
        && (componentsOffsetPosition),
        SLEXCEPTION_MALLOC_FAILED,
        NULL
    );
    memset(ZZSequence, 0x0, jpeg->sampleCount * sizeof(float));

    componentsPosition[0] = ZZSequence;
    for (ci = 1; ci < jpeg->colorComponent; ci++)
    {
        componentsPosition[ci] = componentsPosition[ci - 1] + (((size_t)jpeg->components[ci - 1].mcux  * (size_t)jpeg->components[ci - 1].mcuy)) * 64;
    }
    memcpy(componentsOffsetPosition, componentsPosition, jpeg->colorComponent * sizeof(float *));

    slJpegDecodeScan(jpeg, rawJpeg->_SCAN, slStreamEndPosition(stream), componentsOffsetPosition);
    slReleaseAllocatedMemory(componentsOffsetPosition);

    for (ci = 0; ci < jpeg->colorComponent; ci++)
    {
        slJpegSequentialModeDecodeComponent(
            ((float*)frame->data) + ci * frame->size,
            jpeg->components + ci,
            jpeg->components[ci].sampleCount,
            componentsPosition[ci],
            jpeg->qtTable[jpeg->components[ci].qtIndex]->elements
        );
    }
    slReleaseAllocatedMemory(componentsPosition);
    slReleaseAllocatedMemory(ZZSequence);

    if (jpeg->colorComponent == 3 && !(jpeg->components[0].h == 1 && jpeg->components[0].v == 1))
    {
        slJpegChromaUpSampling(frame, jpeg);
    }
    rgbframe = slYCbCrToRGB(frame);
    slFrameDeallocator(frame);
    slJpegDeallocator(jpeg);
    
    return rgbframe;
}

INT32 NormalizeMatrixAsBlock(BYTE *src, float *dst, size_t width, size_t height, INT32 horizontalComplement, INT32 verticalComplement, INT32 alignedLength)
{
    size_t iteri;
    size_t iterj;

    BYTE  *srcptr;
    float *dstptr;
    float *predstptr;

    INT32 mod;
    INT32 offset;

    mod    = alignedLength - 1;
    offset = (alignedLength * alignedLength) - alignedLength;

    for (iteri = 0, srcptr = src, predstptr = dstptr = dst; iteri < height; iteri++)
    {
        for (iterj = 0; iterj < width; iterj++)
        {   
            if (iterj && !((iterj) & mod))
            {
                dstptr += offset;
            }
            *dstptr++ = (float)(*srcptr++);
        }
        if (!((iteri + 1) & mod))
        {
            dstptr += horizontalComplement;
            predstptr = dstptr;
        }
        else
        {
            predstptr += alignedLength;
            dstptr = predstptr;
        }
    }

    return 0;
}

INT32 slBaselineEnCodingComponent(BYTE *data, size_t width, size_t height)
{
    float *blocks;
    float *dctResult;

    size_t iteri;
    size_t imageSize;

    INT32 horizontalComplement;
    INT32 verticalComplement;


    horizontalComplement = (width & 0x7)  ? 8 - (width  & 0x7) : 0;
    verticalComplement   = (height & 0x7) ? 8 - (height & 0x7) : 0;
    
    /* #1 Padding the height and width of the image with regard to multiples of 8 */
    imageSize = (width + horizontalComplement) * (height + verticalComplement);
    slAssert(
        blocks = (float *)malloc(imageSize * sizeof(float)),
        SLEXCEPTION_NULLPOINTER_REFERENCE,
        -1
    );
    memset(blocks, 0x0, imageSize * sizeof(float));

    /* #2 Normalizing the 2-dimensional matrix image data as a block to block, each of which with a binary sequence of 64 samples.s */
    NormalizeMatrixAsBlock(data, blocks, width, height, horizontalComplement, verticalComplement, 8);
    width  += horizontalComplement;
    height += verticalComplement;

    /* #3 level shift  */
    slLevelDown(blocks, imageSize, 8);

    /* #3 Discrete Consine Transformation */
        slAssert(
        dctResult = (float *)malloc(imageSize * sizeof(float)),
        SLEXCEPTION_NULLPOINTER_REFERENCE,
        -1
    );
    for (iteri = 0; iteri < imageSize; iteri += 64)
    {
        slFastForwardDiscreteConsineTransfrom64(blocks + iteri, dctResult + iteri, 8);
        slUniformQuantizer(dctResult + iteri, slJPEG_CHROMINANCE_QT);
    }
    free(blocks);
    blocks = dctResult;

#if 1
    for (iteri = 0; iteri < imageSize; iteri += 64)
    {
        DisplayFloatMatrix((dctResult + iteri), 64, "DCT:\n", 8);
        // printf("%.2f\t", blocks[iteri]);
    }
#endif
    return 0;
}

#define slEOBSIZE(EOBRUN) (EOBRUN)
void slEncodeEOBRUN(INT32 *EOBRUN, JpegEncodingHuffTable *enht, slJpegScanBitStream *bitStream)
{
    INT32 SSSS;
    INT32 I;

    INT32 k;

    if ((*EOBRUN) != 0)
    {
        SSSS = slEOBSIZE(*EOBRUN);
        I = SSSS * 0x10;
        k = enht->ehufsi[I];
        while (k-- > 0)
        {
            // Append EHUFSI(I) bits of EHUFCO(I) Append SSSS low order bits of EOBRUN
            *EOBRUN = 0;
        }
    }
}

#define slEncodeZRL(R) { /* Append EHUFSI(X’F0’) bits of EHUFCO(X’F0’) */ (R) -= 16; }

void slEncodeRZZ(INT32 *K)
{
    // INT32 I, SSSS;
}

/* @EOBRUN: EOBRUN is the run length of EOBs. EOBRUN is set to zero at the start of each restart interval. */
void slEncodeACCoefficientsWithSpectralSelection(INT32 *EOBRUN, INT32 spectralSelectionStart, INT32 spectralSelectionEnd, JpegEncodingHuffTable *enht, slJpegScanBitStream *bitStream, float *ZZSequence)
{
    INT32 k, R; /* run length of zero cofficients */

    R = 0;
    k = spectralSelectionStart - 1;
    while (k != spectralSelectionEnd)
    {
        k++;
        if (ZZSequence[k] == 0)
        {
            R++;
            if (k != spectralSelectionEnd)
            {
                continue;
            }
            (*EOBRUN)++;
            if (*EOBRUN == 0x7FFF)
            {
                slEncodeEOBRUN(EOBRUN, enht, bitStream);
            }
            break;
        }
        else
        {
            slEncodeEOBRUN(EOBRUN, enht, bitStream);
            while (R >= 16)
            {
                slEncodeZRL(R);
            }
            slEncodeRZZ(&k);
        }
    }
}

INT32 JpegEncodingProcess(Frame frame, CharSequence **cs)
{
    slJpeg *jpeg;
    INT32 ci;

    slAssert(
        jpeg = slJpegAllocator(),
        SLEXCEPTION_NULLPOINTER_REFERENCE,
        -1
    );
    slAssert(
        jpeg->components = SLJpegComponentAllocator(frame->dims),
        SLEXCEPTION_MALLOC_FAILED,
        -1
    );
    
    jpeg->width = frame->row;
    jpeg->height = frame->cols; 
    for (ci = 0; ci < frame->dims; ci++)
    {
        //jpeg->components[ci]->h = 
    }

    slBaselineEnCodingComponent(frame->data, jpeg->width, jpeg->height);
    

    return 0;
}

/* End of the line */

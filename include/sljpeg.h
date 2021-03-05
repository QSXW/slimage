#pragma once

#ifndef __SLJPEG_H__
#define __SLJPEG_H__


#include <malloc.h>
#include <memory.h>

#include <typedefs.h>
#include <slassert.h>
#include <stream.h>
#include <sequence.h>
#include <liblog.h>
#include <slendian.h>
#include <slframe.h>
#include <slcolorspace.h>
#include <generic.h>
#include <sldct.h>

#define SL_DEBUG_ON_STATE 1
#define SL_DEBUG_CODE_ON_STATE

#if 1
#undef SL_DEBUG_ON_STATE
#undef SL_DEBUG_CODE_ON_STATE
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define SLJPEG_ENDOFSCAN -1

#define DisplayFloatMatrix(matrix, size, title, flag) do {\
    if (title) { printf("\033[0;31m%s\033[0m", title); }\
    for (int i = 0; i < size; i++) { printf("%.1f\t", matrix[i]); if (flag && ((i + 1) % flag == 0)) { putchar(10); } }\
    putchar(10);\
} while (0)

#define DisplayIntMatrix(matrix, size, title, flag) {\
    if (title) { printf("\033[0;31m%s\033[0m", title); }\
    for (int i = 0; i < size; i++) { printf("%6d", (matrix)[i]); if (flag && ((i + 1) % flag == 0)) { putchar(10); } }\
    putchar(10);\
}

#define DisplayIntHexMatrix(matrix, size, title, flag) {\
    if (title) { printf("\033[0;31m%s\033[0m", title); }\
    for (int i = 0; i < size; i++) { printf("%4.2x", (matrix)[i]); if (flag && ((i + 1) % flag == 0)) { putchar(10); } }\
    putchar(10);\
}

typedef struct _slJpegScanBitStream {
    BYTE   *start;
    BYTE   *pos;
    BYTE   *end;
    INT32  count;
    BYTE   byte;
} slJpegScanBitStream;

slJpegScanBitStream 
*slJpegScanBitStreamAllocator(
    BYTE *byteSequence,
    BYTE *sequenceEnd
    );
#define slJpegScanBitStreamReset(bitStream, offset) do { bitStream->pos += offset; bitStream->byte  = *(bitStream->pos); bitStream->count = 8; bitStream->pos++;} while (0)
#define slJpegScanBitStreamNextByte(bitStream) (*((bitStream)->pos))
#define slJpegScanBitStreamSkipByte(bitStream, offset) ((bitStream)->pos += (offset))
#define slJpegScanBitStreamDeallocator(bitStream) slReleaseAllocatedMemory(bitStream)
INT32
slNextBitFromStream(
    slJpegScanBitStream *bitStream
    );

#define JPEG_COMPONENT_NUMBER 3
#define SEG_MASK 0x00000000u

#define CODE_SIZE  2
#define SLJPEG_TABLESIZE 8
#define JPEG_HUFF_CODE_SIZE   512
#define JPEG_MARKER_MASK      0xFFu

/* Huffman coding */
#define SLJPEG_CODE_SOF0        0xC0u /* Baseline DCT */
#define SLJPEG_CODE_SOF1        0xC1u /* Extended sequential DCT */
#define SLJPEG_CODE_SOF2        0xC2u /* Progressive DCT */
#define SLJPEG_CODE_SOF3        0xC3u /* Lossless (sequential) */
#define SLJPEG_CODE_SOF5        0xC5u /* Differential sequential DCT */
#define SLJPEG_CODE_SOF6        0xC6u /* Differential progressive DCT */
#define SLJPEG_CODE_SOF7        0xC7u /* Differential lossless (sequential) */
/* Arithmetic coding */
#define SLJPEG_CODE_SOF8        0xC8u /* Reserved for JPEG extensions */
#define SLJPEG_CODE_SOF9        0xC9u /* Extended sequential DCT */
#define SLJPEG_CODE_SOFA        0xCAu /* Progressive DCT */
#define SLJPEG_CODE_SOFB        0xCBu /* Lossless (sequential) */
#define SLJPEG_CODE_SOFD        0xCDu /* Differential sequential DCT */
#define SLJPEG_CODE_SOFE        0xCEu /* Differential progressive DCT */
#define SLJPEG_CODE_SOFF        0xCFu /* Differential lossless (sequential) */

#define SLJPEG_CODE_DHT         0xC4u /* Define Huffman table(s) */
#define SLJPEG_CODE_DAC         0xCCu /* Define arithmetic coding conditioning(s) */

/* Restart with modulo 8 count m */
#define SLJPEG_CODE_RST0        0xD0
#define SLJPEG_CODE_RST1        0xD1
#define SLJPEG_CODE_RST2        0xD2
#define SLJPEG_CODE_RST3        0xD3
#define SLJPEG_CODE_RST4        0xD4
#define SLJPEG_CODE_RST5        0xD5
#define SLJPEG_CODE_RST6        0xD6
#define SLJPEG_CODE_RST7        0xD7

#define SLJPEG_CODE_SOI         0xD8u
#define SLJPEG_CODE_EOI         0xD9u
#define SLJPEG_CODE_SOS         0xDAu
#define SLJPEG_CODE_DQT         0xDBu
#define SLJPEG_CODE_DNL         0xDCu /* Define number of lines */
#define SLJPEG_CODE_DRI         0xDDu
#define SLJPEG_CODE_DHP         0xDEu /* Define hierarchical progression */
#define SLJPEG_CODE_EXP         0xDFu /* Expand reference component(s) */

/* Reserved for application segments */
#define SLJPEG_CODE_APP0        0xE0u
#define SLJPEG_CODE_APP1        0xE1u
#define SLJPEG_CODE_APP2        0xE2u
#define SLJPEG_CODE_APP3        0xE3u
#define SLJPEG_CODE_APP4        0xE4u
#define SLJPEG_CODE_APP5        0xE5u
#define SLJPEG_CODE_APP6        0xE6u
#define SLJPEG_CODE_APP7        0xE7u
#define SLJPEG_CODE_APP8        0xE8u
#define SLJPEG_CODE_APP9        0xE9u
#define SLJPEG_CODE_APPA        0xEAu
#define SLJPEG_CODE_APPB        0xEBu
#define SLJPEG_CODE_APPC        0xECu
#define SLJPEG_CODE_APPD        0xEDu
#define SLJPEG_CODE_APPE        0xEEu
#define SLJPEG_CODE_APPF        0xEFu

/* Reserved for JPEG extensions */
#define SLJPEG_CODE_JPG0        0XF0u
#define SLJPEG_CODE_JPG1        0XF1u
#define SLJPEG_CODE_JPG2        0XF2u
#define SLJPEG_CODE_JPG3        0XF3u
#define SLJPEG_CODE_JPG4        0XF4u
#define SLJPEG_CODE_JPG5        0XF5u
#define SLJPEG_CODE_JPG6        0XF6u
#define SLJPEG_CODE_JPG7        0XF7u
#define SLJPEG_CODE_JPG8        0XF8u
#define SLJPEG_CODE_JPG9        0XF9u
#define SLJPEG_CODE_JPGA        0XFAu
#define SLJPEG_CODE_JPGB        0XFBu
#define SLJPEG_CODE_JPGC        0XFCu
#define SLJPEG_CODE_JPGD        0XFDu

#define SLJPEG_CODE_COM          0xFEu

/* Reserved markers */
#define SLJPEG_CODE_TEM          0x01u /* For temporary private use in arithmetic coding */
#define SLJPEG_CODE_RES          0x02u /* X'FF02 through X'FFBF */

#define SLJPEG_HUFFBITSSIZE         0x10
#define IGNORABLE_SIZE              3

#define QT_PRECISION_MASK       0b11110000
#define QT_NUMBER_MASK          0b00001111
#define QT_BYTES_NUM_BASE       0x40
#define QT_NUMBER               0x4

#define slJpegPointTransform(n, power) (((float)(n)) / (2 << power))

#pragma pack(push, 1)

typedef struct _SLSOI
{
    BYTE ID[CODE_SIZE];
} SOI;

typedef struct  _SLFrameComponent
{
    BYTE    index;
    BYTE    samplingFactor;
    BYTE    qtSelector;
} FrameComponent;

typedef struct _SLSOF
{
    BYTE                code[CODE_SIZE];
    WORD                length;
    BYTE                precision;
    WORD                height;
    WORD                width;
    BYTE                componentNum;
    FrameComponent      components[0];
} SOF0, DHP;

typedef struct  _SLScanComponent
{
    BYTE index;
    BYTE htSelector;;                            
} ScanComponent;

typedef struct  _SLSOS
{
    BYTE          code[CODE_SIZE];
    WORD          length;
    BYTE          componentNum;
    ScanComponent componentSelector[0];
} SOS;

typedef struct  _SLProgressiveMeta {
    BYTE spectralSelectionStart;
    BYTE spectralSelectionEnd;
    BYTE successiveApproximation;
} ProgressiveMeta;

#define DQT_INFO_SIZE 5
typedef struct  _SLDQT
{
    BYTE         code[CODE_SIZE];
    WORD         length; 
    BYTE         selector; /* Specifies element precision and destination identifier */ 
    BYTE         element[0];
} DQT;

#define DHT_INFO_SIZE 21
typedef struct  _SLDHT
{
    BYTE code[CODE_SIZE];
    WORD length;
    BYTE selector;                                  
    BYTE bits[SLJPEG_HUFFBITSSIZE];
    BYTE values[0];
} DHT;

typedef struct  _SLConditioningTable {
    BYTE selector;
    WORD value;
} ConditioningTable;

typedef struct  _SLDAC {
    BYTE              code[CODE_SIZE];
    WORD              length;
    ConditioningTable condition[0];
} DAC;

typedef struct  _SLDRI
{
    BYTE    code[CODE_SIZE];
    WORD    length;
    WORD    MCUNumber; /* When equals to zero, disable restart interval for the following scan. */
} slDRI;

typedef struct COM
{
    BYTE       code[CODE_SIZE];
    WORD       length;
    BYTE       comment[0];
} COM;

typedef struct  _SLAPP0
{
    BYTE   code[CODE_SIZE];
    WORD   length;
    BYTE   identifier[5];
    BYTE   majorRevisionNum;
    BYTE   minorRevisionNum;
    BYTE   units;
    WORD   horizontalDensity;
    WORD   verticalDensity;
    BYTE   thumbnailWidth;
    BYTE   thumbnailHeight;
    BYTE   thumbnail[0];
} APP0;

typedef struct _slAPP1 {
    BYTE   code[CODE_SIZE];
    WORD   length;
    BYTE   identifier[6];
    BYTE   data[0];
} SLAPP1;

typedef struct  _SLEXP {
    BYTE code[CODE_SIZE];
    WORD length;
    WORD expansion;
} EXP;

/* The DNL segement shall only occur at the end of the first scan */
typedef struct  _SLDNL {
    BYTE code[CODE_SIZE];
    WORD length;
    WORD lineNum;
} DNL;

typedef struct  _SLEOI
{
    BYTE code[CODE_SIZE];
} EOI;

typedef CharSequence SLJPEGScan;
#if 0
typedef struct _SLJPEGScan
{
    BYTE   *end;
    BYTE   *data;
} SLJPEGScan;
#endif
#pragma pack(pop)



/*
*   The struct _RawJpeg is for write and read more flexible.
*   The required JPEG markers within a segment:
*   SOI (must appear at very beginning of segment)
*   SOFn
*   SOS (one for each scan, if there is more than one scan)
*   EOI (must appear at very end of segment)
*/
typedef struct  _RawJpeg {
    SOI              *_SOI;
    DHP              *_DHP;
    EXP              *_EXP;
    SOF0             *_SOF;
    DQT              *_DQT[SLJPEG_TABLESIZE];
    DHT              *_DHT[SLJPEG_TABLESIZE];
    DAC              *_DAC;
    slDRI            *_DRI;
    COM              *_COM;
    APP0             *_APP;
    SOS              *_SOS;
    DNL              *_DNL;
    EOI              *_EOI;
    BYTE             *_SCAN;
    struct _RawJpeg  *next;
} RawJpeg;

#define slRawJpegSetNullptr(RAWJPEG) (memset(RAWJPEG, 0x0, sizeof(RawJpeg)))
// #define slRawJpegReadSegment(segment, size, stream) do { if (!segment) { segment = (void *)malloc(size > sizeof(segment) ? size : sizeof(segment)); } if (segment) { ReadStream(segment, 0, size, stream); }} while (0)
#define slRawJpegReadSegment(_DATATYPE, segment, size, stream) do { segment = ((_DATATYPE *)stream->pos); stream->pos += size; } while (0)
#define slRawJpegAllocator() ((RawJpeg *)malloc(sizeof(RawJpeg)))
#define DestroyRawJpeg(_jpeg) do { _DestroyRawJpeg(_jpeg); _jpeg = NULL; } while(0)
#define slRawJpegGetSegmentLength(stream, length) do { length =  *(((WORD *)(stream->pos)) + 1); if (IsIntelMode()) { MotorolaToIntelMode2(&length); } length += CODE_SIZE; } while (0)

void
_DestroyRawJpeg(
    RawJpeg *rawJpeg
);
Frame
JpegRead(
    Stream fileStream
    );
RawJpeg
*RawJpegRead(
    Stream stream
    );
void
ShowJpegData(
    const RawJpeg *jpeg,
    const size_t  fileSize
    );
Frame
JpegDecodingProcess(
    const RawJpeg *jpeg,
    Stream stream
    );

#define SLJPEGCOLORMODE_GRAYSCALED 1
#define SLJPEGCOLORMODE_YCBCR      3
#define SLJPEGCOLORMODE_CMYK       4
#define SLJPEG_JPEGHUFFTABLESIZE   4

#define SLJPEG_HUFFVALSIZE 257
typedef struct __slJpegHuffTable {
    BYTE  bits[17];
    BYTE  huffval[SLJPEG_HUFFVALSIZE];
    INT32 MINCODE[17];
    INT32 MAXCODE[17];
    INT32 VALPTR[17];
} slJpegHuffTable;

slJpegHuffTable
*slJpegHuffTableAllocator(
    BYTE *bits,
    BYTE *values,
    INT32 length
    );
#define slJpegHuffTableDeallocator(jpegHuffTable) do { if (jpegHuffTable) { free(jpegHuffTable); }} while (0)

typedef struct _slJpegQuantizationTable {
    INT32 precision;
    float elements[0];
} JpegQuantizationTable;

JpegQuantizationTable
*slJpegQuantizationTableAllocator(
    INT32 precision,
    BYTE *element,
    INT32 *length
    );
#define slJpegQuantizationTableDeallocator(jpegQuantizationTable) slReleaseAllocatedMemory(jpegQuantizationTable)

typedef struct __slJpegEncodingProcedureCodeTable {
    BYTE ehufco[SLJPEG_HUFFVALSIZE];
    BYTE ehufsi[SLJPEG_HUFFVALSIZE];
} JpegEncodingHuffTable;

typedef struct __slJpegComponent {
    BYTE   h;
    BYTE   v;
    BYTE   paddingx;
    BYTE   paddingy;
    INT32  mcux;
    INT32  mcuy;
    INT32  paddingMcux;
    INT32  paddingMcuy;
    size_t sampleCount;
    DWORD  x;
    DWORD  y;
    BYTE   qtIndex;
    BYTE   acIndex;
    BYTE   dcIndex;
} slJpegComponent;

typedef struct  _slJpeg {
    BYTE                     maxHorizontalSamplingFactor;
    BYTE                     maxVerticalSamplingFactor;
    BYTE                     samplePrecision;
    BYTE                     colorComponent;
    DWORD                    width;
    DWORD                    height;
    size_t                   sampleCount;
    slJpegComponent         *components;
    slJpegHuffTable         *acTable[SLJPEG_JPEGHUFFTABLESIZE];
    slJpegHuffTable         *dcTable[SLJPEG_JPEGHUFFTABLESIZE];
    JpegQuantizationTable   *qtTable[SLJPEG_JPEGHUFFTABLESIZE];
    WORD                     dataPerMCU;
    INT32                    MCUNumber; 
    BYTE                     isProgressive;
    BYTE                     Ss;
    BYTE                     Se;
    BYTE                     Al;
    BYTE                     Ah;
    BYTE                     nextRestartNum;
} slJpeg;

slJpegComponent
*SLJpegComponentAllocator(
    BYTE capacity
    );
#define slJpegComponentDeallocator(jpegComponents) slReleaseAllocatedMemory(jpegComponents)
slJpeg
*slJpegAllocator(
    );
void __slJpegDeallocator(
    slJpeg *jpeg
    );
#define slJpegDeallocator(jpeg) do{ if (jpeg) { __slJpegDeallocator(jpeg); jpeg = NULL; }}while(0)
JpegEncodingHuffTable
*JpegEncodingHuffTableAllocator(
    slJpegHuffTable *huffTable
    );
#define JpegEncodingHuffTableDeallocator(encodingHuffTable) slReleaseAllocatedMemory(encodingHuffTable)

void
BindJpegToRawJpeg(
    slJpeg *jpeg,
    const RawJpeg *rawJpeg
    );
INT32
slEntroyDecodingProcess(
    slJpegScanBitStream *bitStream,
    float *ZZSequence,
    slJpegHuffTable *dcTable,
    slJpegHuffTable *acTable,
    float *PRED
    );
INT32
JpegEncodingProcess(
    Frame frame,
    CharSequence **cs
    );

static const BYTE SLJPEG_ZIGZAG_INDEX[] = {
    0,  1,  5,  6,  14, 15, 27, 28,
    2,  4,  7,  13, 16, 26, 29, 42,
    3,  8,  12, 17, 25, 30, 41, 43,
    9,  11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63
};

#ifdef __cplusplus 
}
#endif /* __cplusplus */

#endif /* __SLJPEG_H__ */

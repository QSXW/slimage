#pragma once

#ifndef __STREAM_H__
#define __STREAM_H__

#include <stdio.h>
#include "typedefs.h"
#include "liblog.h"
#include "sequence.h"

#if defined( __cplusplus )
extern "C" {
#endif /* __cplusplus */

#define STREAM_MBUF_PRESET_SIZE 41943040
#define PAUSE do{printf("Press any key to continue...");getchar();}while(0)
#define BREAKBOINT while(0)

#define FILE_SIZE(fp)       ((fseek(fp, 0, SEEK_END)), (ftell(fp)))
#define RESET_TO_SEK(fp)    (fseek(fp, 0, SEEK_SET))

enum FILE_MODE { READABLE = 0, WRITEABLE = 3, CACHED = 8 };

typedef struct  _Stream {
    FILE            *fp;
    size_t          fsize;
    BYTE            *pos;
    BYTE            *_mbuf;
    CharSequence    *fname;
    BYTE            *_end;
    Bool            readable;
    Bool            writable;
} *Stream;

#define ReadByte(_stream) ((_stream->_mbuf) ? _ReadByte(_stream) : fgetc(_stream->fp))
#define CloseStream(_stream) (__CloseStream(_stream), _stream = NULL)
#define _ReadStream(_dst, _offset, _size, _stream) (_stream->pos += _offset, memcpy(_dst, _stream->pos, _size), _stream->pos += _size)
#define slStreamStartPosition(stream) (stream->_mbuf)
#define slStreamCurrentPosition(stream) (stream->pos)
#define slStreamEndPosition(stream) (stream->_end)

Stream
NewStream(
    const char *filename,
    const int mode
    );
Stream 
__NewStream();

int
_ReadByte(
    Stream _stream
    );
size_t
ReadStream(
    void *_dst,
    int _offset,
    size_t _size,
    Stream _stream
    );
void
__CloseStream(
    Stream _stream
    );

#if defined( __cplusplus )
}
#endif /* __cplusplus */
#endif

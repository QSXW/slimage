#include "stream.h"

Stream NewStream(const char *fname, const int _mode)
{
    Stream stream;
    const char *mode[] = { "rb", "wb", "r+", "rb+", "a", "a+" };

    if ((stream = __NewStream()))
    {
        if (!(stream->fp = fopen(fname, mode[_mode & 0xF7])))
        {
            SaveLog(fname, FILE_NOT_FOUND);
            CloseStream(stream);
            return NULL;
        }
        stream->fname = NewCharSequenceWithString((BYTE *)fname);
        stream->fsize = FILE_SIZE(stream->fp);
        RESET_TO_SEK(stream->fp);
        if (stream->fsize > 0)
        {
            if (_mode & CACHED /* || stream->fsize < STREAM_MBUF_PRESET_SIZE */)
            {
                stream->_mbuf = (BYTE *)malloc(stream->fsize + 1);
                stream->_end = stream->_mbuf + stream->fsize;
                *stream->_end = '\0';
                fread(stream->_mbuf, stream->_end - stream->_mbuf, 1, stream->fp);
                stream->pos = stream->_mbuf;
            }
            stream->readable = TRUE;
        }
        if (_mode & WRITEABLE) { stream->writable = TRUE; }
    }

    return stream;
}

Stream __NewStream()
{
    Stream stream;
    if ((stream = (Stream)malloc(sizeof(struct _Stream))))
    {
        memset(stream, 0x0, sizeof(struct _Stream));
    }
    return stream;
}

size_t ReadStream(void *_dst, int _offset, size_t _size, Stream _stream)
{
    if (_stream->_mbuf)
    {
        _stream->pos += _offset;
        memcpy(_dst, _stream->pos, _size);
        _stream->pos += _size;
    }
    else
    {
        fread(_dst, _size, 1, _stream->fp);
    }

    return _size;
}

int _ReadByte(Stream _stream)
{
    if (_stream->pos >= _stream->_end) { return EOF; }
    return *_stream->pos++;
}

void __CloseStream(Stream _stream)
{   
    if (_stream)
    {
        if (_stream->_mbuf) { free(_stream->_mbuf); }
        if (_stream->fname) { csfree(_stream->fname); }
        if (_stream->fp) { fclose(_stream->fp); };
        free(_stream);
    }
}

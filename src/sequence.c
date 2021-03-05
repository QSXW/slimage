#include "sequence.h"

size_t cslen(const BYTE *const s)
{
    const register BYTE *p = s;
    while (*p) { p++; }
    return (size_t)(p - s);
}

CharSequence *CharSequenceAllocator()
{ 
    CharSequence *cs;

    if ((cs = (CharSequence *)malloc(sizeof(CharSequence))))
    {
        memset(cs, 0x0, sizeof(CharSequence));
    }

    return cs;
}

void cspush(CharSequence **_cs, const BYTE *_s)
{
    CharSequence *cs = *_cs;
    size_t size;
    
    while (cs->next != NULL && cs->data != NULL)
    {
        cs->threshhold++;
        cs = cs->next;
    }
    if (cs->data != NULL)
    {
        cs->next = CharSequenceAllocator();
        cs->threshhold++;
        cs = cs->next;
    }

    size = cslen(_s);
    cs->data = (BYTE *)malloc(size + 1);
    memcpy(cs->data, _s, size + 1);
    *(cs->data + size) = '\0';
    cs->size = size;

    if ((*_cs)->threshhold > 12)
    {
        cs = *_cs;
        *_cs = cscopy(cs);
        csfree(cs);
    }
}

void CharSequencePushStack(CharSequence **_cs, const void *_s, size_t size)
{
    CharSequence *cs = *_cs;
    
    while (cs->next != NULL && cs->data != NULL)
    {
        cs->threshhold++;
        cs = cs->next;
    }
    if (cs->data != NULL)
    {
        cs->next = CharSequenceAllocator();
        cs->threshhold++;
        cs = cs->next;
    }

    cs->data = (BYTE *)malloc(size);
    memcpy(cs->data, _s, size);
    *(cs->data + size) = '\0';
    cs->size = size;

    if ((*_cs)->threshhold > 12)
    {
        cs = *_cs;
        *_cs = cscopy(cs);
        csfree(cs);
    }
}

void csfree(CharSequence *_cs)
{
    CharSequence *cs = _cs;
    while (cs != NULL)
    {
        _cs = cs->next;
        free(cs->data);
        free(cs);
        cs = _cs;
    }
}

BYTE csat(CharSequence *_cs, size_t _index)
{
    CharSequence *cs = _cs;
    while (cs && _index >= cs->size)
    {
        _index -= cs->size;
        cs = cs->next;
    }
    if (!cs)
    {
        return 0;
    }
    return *(cs->data + _index);
}

CharSequence *cscopy(CharSequence *_cs)
{
    CharSequence *cs;

    if ((cs = CharSequenceAllocator()))
    {
        cs->data = (BYTE *)malloc(cssize(_cs) + 1);
        while (_cs)
        {
            memcpy(cs->data + cs->size, _cs->data, _cs->size);
            cs->size += _cs->size;
            _cs = _cs->next;
        }
        cs->data[cs->size] = '\0';
    }

    return cs;
}

size_t cssize(CharSequence *_cs)
{
    size_t length = 0;
    while (_cs)
    {
        length += _cs->size;
        _cs = _cs->next;
    }
    return length;
}

void cscout(CharSequence *_cs)
{
    size_t i;
    while (_cs != NULL)
    {
        i = 0;
        while (i < _cs->size)
        {
            putchar(*(_cs->data + i++));
        }
        _cs = _cs->next;
    }
}

CharSequence *cscat(CharSequence *_csst, CharSequence *_csnd)
{
    CharSequence *cs = NULL;
    CharSequence **join = &_csst;

    while (*join)
    {
        join = &((*join)->next);
    }
    *(join) = _csnd;
    cs = cscopy(_csst);
    *(join) = NULL;
    return cs;
}

CharSequence *cscat_faster(CharSequence **_csst, CharSequence **_csnd)
{
    CharSequence *cs = NULL;
    CharSequence **join = _csst;

    while (*join)
    {
        join = &((*join)->next);
    }
    *join = *_csnd;
    cs = *_csst;

    *_csst = NULL;
    *_csnd = NULL;
    return cs;
}

CharSequence *NewCharSequenceWithString(const BYTE *_s)
{
    CharSequence *cs = CharSequenceAllocator();
    if ((cs = CharSequenceAllocator()))
    {
        cspush(&cs, _s);
    }

    return cs;
}

char *_cs2lower(char *const _s)
{
#ifdef _LETTER_CASE_CONVERT_FAST_
    const unsigned char map[] = {
         0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,
         16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
         32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
         48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
         64,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
        112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,  91,  92,  93,  94,  95,
         96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
        112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
        128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
        144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
        160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
        176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
        192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
        208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
        224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
        240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
    };

    register unsigned char *p = (unsigned char *)_s;
    while ((*p = map[*p])) { p++; }
#else
    char *p = _s;

    while (*p)
    {
        if (*p > 65 && *p < 90)
        {
            *p += 32;
        }
        p++;
    }
#endif
    return _s;
}

char *_cs2upper(char *const _s)
{
#ifdef _LETTER_CASE_CONVERT_FAST_
    const unsigned char map[] = {
          0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
         16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
         32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
         48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
         64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
         80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
         96,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
         80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90, 123, 124, 125, 126, 127,
        128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
        144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
        160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
        176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
        192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
        208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
        224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
        240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
    };
    register unsigned char *p = (unsigned char *)_s;
    while ((*p = map[*p])) { p++; }
#else
    char *p = _s;
    while (*p)
    {
        if (*p > 97 && *p < 122)
        {
            *p -= 32;
        }
        p++;
    }
#endif
    return _s;
}

int cscomp(const char *_s, const char *_t)
{
    register const char *p0 = _s;
    register const char *p1 = _t;

    while (*p0 == *p1)
    {
        if (*p0 == '\0') { return 0; }
        p0++;
        p1++;
    }
    return (*p0 - *p1);
}

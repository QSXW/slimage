#include <slendian.h>

void MotorolaToIntelMode(void *src, void *dst, INT32 typeSize)
{   
    INT32 i;
    BYTE *srcptr;
    BYTE *dstptr;

    for(srcptr = (BYTE *)src, dstptr = (BYTE *)dst, i = 0; --typeSize >= 0; )
    {
        dstptr[i++] = srcptr[typeSize];
    }
}

BYTE IsMotorolaMode()
{
    DWORD c = 0x100;
    return (0x1 & *((BYTE *)&c));
}

void MotorolaToIntelMode2(WORD *w)
{
    BYTE bytefront = 0x0;
    bytefront = *((BYTE *)(w));
    (*w) >>= 8;
    *((BYTE *)(w) + 1) = bytefront;
}

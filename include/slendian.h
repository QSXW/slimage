#pragma once
#ifndef __SLENDIAN_H__
#define __SLENIAN_H__

#include <typedefs.h>

void
MotorolaToIntelMode2(
    WORD *w
    );
void MotorolaToIntelMode(
    void *src,
    void *dst,
    INT32 typeSize
    );
BYTE
IsMotorolaMode(
    );

#define IntelToMotorolaMode(src, dst, typeSize) ((MotorolaToIntelMode(src, dst, typeSize)))
#define IsIntelMode() (!IsMotorolaMode())

#endif /* __SLENDIAN_H__ */

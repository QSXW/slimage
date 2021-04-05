#pragma once
#ifndef __SLENDIAN_H__
#define __SLENIAN_H__

#include "typedefs.h"

#if defined( __cplusplus )
extern "C" {
#endif /* __cplusplus */

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

#if defined( __cplusplus )
}
#endif /* __cplusplus */
#endif /* __SLENDIAN_H__ */

#pragma once

#ifndef __SEQUENCE_H__
#define __SEQUENCE_H__

#include "typedefs.h"
#include <malloc.h>
#include <memory.h>
#include <stdio.h>

#define _LETTER_CASE_CONVERT_FAST_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct CharSequence 
{
    size_t  size;
    struct  CharSequence *next;
    int     threshhold;
    BYTE    *data;
} CharSequence;

CharSequence
*CharSequenceAllocator();
CharSequence
*NewCharSequenceWithString(
    const BYTE *_s
    );

void
cspush(
    CharSequence **_cs,
    const BYTE *_s
    );
void
csfree(
    CharSequence *_cs
    );
BYTE
csat(
    CharSequence *_cs,
    size_t _index
    );
size_t
cssize(
    CharSequence *_cs
    );
INT32
cscomp(
    const char *_s,
    const char *_t
    );
void
cscout(
    CharSequence *_cs
    );
CharSequence
*cscopy(
    CharSequence *_cs
    );
size_t
cslen(
    const BYTE *s
    );
CharSequence
*cscat(
    CharSequence *_csst,
    CharSequence *_csnd
    );
CharSequence
*cscat_faster(
    CharSequence **_csst,
    CharSequence **_csnd
    );

void
CharSequencePushStack(
    CharSequence **_cs,
    const void *_s,
    size_t size
    );

#define cs2lower(_cs) (_cs2lower(_cs->data))
#define cs2upper(_cs) (_cs2upper(_cs->data))
char
*_cs2lower(
    char*const _s
    );
char
*_cs2upper(
    char *const _s
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SEQUENCE_H__ */
/* End of the __SEQUENCE_H__ */
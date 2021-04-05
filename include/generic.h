#pragma once

#ifndef __SLGENERIC_H__
#define __SLGENERIC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define slIsOdd(x) ((0x1) & (x))
#define slIsEven(x) (!slIsOdd(x))
#define slMin(x, y) ((x) > (y) ? (y) : (x))
#define slMax(x, y) ((x) > (y) ? (x) : (y))
#define slAbsolute(x) (((x) < 0) ? ((x) * -1) : (x))
#define slMod8(number) (((number) & 0x7))
#define slMod4(number) (((number) & 0x3))
#define slMod(number, mod) (((number) & ((mod) - 1))) /* The fast modula operation for the power of 2 */
#define slPower2(number) (((size_t)0x1) << (number))
#define slSquare(number) ((number) * (number))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SLGENERIC_H__ */
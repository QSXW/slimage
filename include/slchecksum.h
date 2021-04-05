#pragma once

#if !defined( __SLCHECKSUM_H__ )
#define __SLCHECKSUM_H__

#include <stdint.h>

#if defined( __cplusplus )
extern "C" {
#endif /* __cplusplus */

#define CRC32_GENERATOR_POLYNOMIAL 0x04C11DB7
uint32_t slCyclicRedundanceCheck32(const uint8_t *message, uint32_t length);

#if defined( __cplusplus )
}
#endif /* __cplusplus */
#endif /* __SLCHECKSUM_H__ */

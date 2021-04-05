#include "slchecksum.h"

uint32_t slCyclicRedundanceCheck32(const uint8_t *message, uint32_t length) {
    uint32_t i, j;
    uint32_t CRC, MSB;

    CRC  =  ~((uint32_t)0);
    for (i = 0; i < length; i++)
    {
        CRC ^= (((uint32_t)message[i]) << 24);
        for (j = 0; j < 8; j++)
        {
            MSB = CRC >> 31;
            CRC <<= 1;
            CRC ^= (0 - MSB) & CRC32_GENERATOR_POLYNOMIAL;
        }
    }
    
    return CRC;
}
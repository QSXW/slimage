#pragma once

#ifndef __SLCIF_H__
#define __SLCIF_H__

#include "typedefs.h"

namespace CIF {

    enum CifChunkFlags {
        CIFCHUNKFLAGS_META = 0x1u,
        CIFCHUNKFLAGS_SEQUENCE,
        CIFCHUNKFLAGS_EXTENDED
    };

    class CifChunk {
        uint8_t weight;
        uint8_t flag;
        uint8_t data[0];
    };

    class Cif {
    public:
        Cif();
        ~Cif();

    private:

    };
};


#endif /* __SLCIF_H__ */

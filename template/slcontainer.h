#pragma once

#ifndef __SLCONTAINER_H__
#define __SLCONTAINER_H__

#include "slstack.h"
#include "sltree.h"
namespace slcontainer {
    using namespace::sltree;
    using namespace ::slstack;
    using IntContainer      = RedBlackTree<int>;
    using FloatContainer    = RedBlackTree<float>;
    using StringContainer   = RedBlackTree<std::string>;
}


#endif /* __SLCONTAINER_H__ */

#ifndef IG20250710132744
#define IG20250710132744

#include "float2.h"
#include <memory>

/// Node for KD-Tree 
struct NodeKDT
{
    std::unique_ptr<NodeKDT> lower{};
    std::unique_ptr<NodeKDT> higher{};
    int dimension{};

    float2 data{};

};

namespace snx{
    /// KD Tree
    class KDT{
    };
}

#endif
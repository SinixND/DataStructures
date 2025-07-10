#ifndef IG20250710132659
#define IG20250710132659

#include "float2.h"
#include <memory>

/// Node for (Point) Region Quad Tree
struct NodeRQT
{
    std::unique_ptr<NodeRQT> nw{};
    std::unique_ptr<NodeRQT> ne{};
    std::unique_ptr<NodeRQT> sw{};
    std::unique_ptr<NodeRQT> se{};

    /// Node has either data or (position and children)
    float2 position{};
    float2 data{};
};

namespace snx{
    /// Region Quad Tree
    class RQT{
    };
}

#endif
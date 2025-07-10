#ifndef IG20250710132723
#define IG20250710132723

#include "float2.h"
#include <array>
#include <memory>

/// Node for Point Quad Tree
struct NodePQT
{
    std::array<std::unique_ptr<NodePQT>, 4> children{};

    /// Represents node position too
    float2 data{};
};

namespace snx
{
    /// Point Quad Tree
    class PQT
    {
        std::unique_ptr<NodePQT> root{};

    public:
        void add( float2 value )
        {
            add( value, root );
        }

    public:
        void add( float2 value, std::unique_ptr<NodePQT>& node )
        {
            if ( !node->children[0] )
            {
                node->data = value;
            }

            //* Determin quadrant
        }

    private:
    };
}

#endif

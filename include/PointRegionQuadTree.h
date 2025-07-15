#ifndef IG20250710132723
#define IG20250710132723

//* https://opendsa-server.cs.vt.edu/ODSA/Books/CS3/html/PRquadtree.html

#include "AABB.h"
#include "float2.h"
#include <array>
#include <memory>
#include <vector>

std::array<float2, 4> constexpr directions{
    float2{ -1, -1 }, // NW
    float2{ 1, -1 },  // NE
    float2{ -1, 1 },  // SW
    float2{ 1, 1 }    // SE
};

template <typename Type>
struct NodePRQT
{
    std::vector<std::unique_ptr<NodePRQT<Type>>> children{};

    AABB box{};
    float2 valuePosition{};
    Type value{};

    NodePRQT()
    {
        children.reserve( 4 );
    }

    NodePRQT(
        AABB box,
        float2 position,
        Type value
    )
        : NodePRQT()
    {
        this->box = box;
        this->valuePosition = position;
        this->value = value;
    }
};

namespace snx
{
    /// Point Quad Tree
    template <typename Type>
    class PRQT
    {
        std::unique_ptr<NodePRQT<Type>> root{};

    public:
        PRQT(
            AABB box,
            float2 position,
            Type value
        )
            : root( std::make_unique<NodePRQT<Type>>() )
        {
            root->box = box;
            root->valuePosition = position;
            root->value = value;
        }

        void add(
            Type value,
            float2 position
        )
        {
            add(
                value,
                position,
                root
            );
        }

    private:
        static bool hasChildren( std::unique_ptr<NodePRQT<Type>> const& node )
        {
            return !node->children.empty();
        }

        void add(
            Type value,
            float2 position,
            std::unique_ptr<NodePRQT<Type>>& node
        )
        {
            //* Calculate direction of input position
            int dir{ 0 }; // NW

            if ( position.x > node->box.center.x )
            {
                dir |= 1 << 0; // E
            }

            if ( position.y > node->box.center.y )
            {
                dir |= 1 << 1; // S
            }

            if ( !hasChildren( node ) ) // is leaf, has value
            {
                //* Split leaf and insert old
                node->children.resize( 4 );

                //* Calculate direction of existing position
                int oldDir = 0; // NW

                if ( node->valuePosition.x > node->box.center.x )
                {
                    oldDir |= 1 << 0; // E
                }

                if ( node->valuePosition.y > node->box.center.y )
                {
                    oldDir |= 1 << 1; // S
                }

                //* Add new leaf node for existing position and data
                node->children[oldDir] = std::make_unique<NodePRQT<Type>>();
                node->children[oldDir]->box = {
                    { node->box.center.x + directions[oldDir].x * ( node->box.dimensions.x / 4 ),
                      node->box.center.y + directions[oldDir].y * ( node->box.dimensions.y / 4 ) },
                    { node->box.dimensions.x / 2,
                      node->box.dimensions.y / 2 }
                };
                node->children[oldDir]->valuePosition = node->valuePosition;
                node->children[oldDir]->value = node->value;
            }

            //* If children exists in direction, descend
            if ( node->children[dir] )
            {
                //* Check next children
                add(
                    value,
                    position,
                    node->children[dir]
                );

                return;
            }

            //* Add new leaf node for new position and data
            node->children[dir] = std::make_unique<NodePRQT<Type>>();
            node->children[dir]->box = {
                { node->box.center.x + directions[dir].x * ( node->box.dimensions.x / 4 ),
                  node->box.center.y + directions[dir].y * ( node->box.dimensions.y / 4 ) },
                { node->box.dimensions.x / 2,
                  node->box.dimensions.y / 2 }
            };
            node->children[dir]->valuePosition = position;
            node->children[dir]->value = value;

            return;
        }
    };
}

#endif

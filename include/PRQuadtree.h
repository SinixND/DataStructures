#ifndef IG20250710132723
#define IG20250710132723

//* https://opendsa-server.cs.vt.edu/ODSA/Books/CS3/html/PRquadtree.html

#include "AABB.h"
#include "float2.h"
#include <array>
#include <memory>
#include <vector>

namespace snx::PQRT
{
    std::array<float2, 4> constexpr directions{
        float2{ -1, -1 }, // NW
        float2{ 1, -1 },  // NE
        float2{ -1, 1 },  // SW
        float2{ 1, 1 }    // SE
    };

    template <typename Type>
    struct NodePRQT
    {
        std::vector<std::unique_ptr<NodePRQT<Type>>> branches{};

        AABB box{};
        float2 valuePosition{};
        Type value{};

        NodePRQT()
        {
            branches.reserve( 4 );
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

        void insert(
            Type value,
            float2 position
        )
        {
            insert(
                value,
                position,
                root
            );
        }

    private:
        static bool hasBranches( std::unique_ptr<NodePRQT<Type>> const& node )
        {
            return !node->branches.empty();
        }

        void insert(
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

            if ( !hasBranches( node ) ) // is leaf, has value
            {
                //* Split leaf and insert old
                node->branches.resize( 4 );

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
                node->branches[oldDir] = std::make_unique<NodePRQT<Type>>();
                node->branches[oldDir]->box = {
                    { node->box.center.x + directions[oldDir].x * ( node->box.dimensions.x / 4 ),
                      node->box.center.y + directions[oldDir].y * ( node->box.dimensions.y / 4 ) },
                    { node->box.dimensions.x / 2,
                      node->box.dimensions.y / 2 }
                };
                node->branches[oldDir]->valuePosition = node->valuePosition;
                node->branches[oldDir]->value = node->value;
            }

            //* If branches exists in direction, descend
            if ( node->branches[dir] )
            {
                //* Check next branches
                insert(
                    value,
                    position,
                    node->branches[dir]
                );

                return;
            }

            //* Add new leaf node for new position and data
            node->branches[dir] = std::make_unique<NodePRQT<Type>>();
            node->branches[dir]->box = {
                { node->box.center.x + directions[dir].x * ( node->box.dimensions.x / 4 ),
                  node->box.center.y + directions[dir].y * ( node->box.dimensions.y / 4 ) },
                { node->box.dimensions.x / 2,
                  node->box.dimensions.y / 2 }
            };
            node->branches[dir]->valuePosition = position;
            node->branches[dir]->value = value;

            return;
        }
    };
}

#endif

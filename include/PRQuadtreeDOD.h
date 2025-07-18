#ifndef IG20250717092941
#define IG20250717092941

//* https://lisyarus.github.io/blog/posts/building-a-quadtree.html

#include "snxTypes.h"
#include <vector>

struct Node
{
    snx::Id branches[2][2]{
        { 0, 0 },
        { 0, 0 }
    };
    snx::Id value{ 0 };
};

template <typename Type>
struct Value
{
    snx::Float2 position{};
    Type data{};
};

namespace snx
{
    template <typename Type>
    class PRQT_DOD
    {
        std::vector<Node> nodes_{
            Node{},
            Node{}
        };
        std::vector<Value<Type>> values_{ Value<Type>{} };
        AABB bbox_{};
        Id root_{ 1 };

    public:
        PRQT_DOD( AABB const& bbox )
            : bbox_( bbox )
        {
        }

        void insert(
            Type const& value,
            Float2 const& position
        )
        {
            insert(
                value,
                position,
                root_,
                bbox_
            );
        }

    private:
        bool hasBranch(
            Id node,
            Int2 quad
        )
        {
            return nodes_[node].branches[quad.x][quad.y];
        }

        bool hasValue( Id node )
        {
            return node < values_.size();
        }

        Int2 getQuadrant(
            Float2 from,
            Float2 to
        )
        {
            Int2 quad{ 0, 0 }; // NW

            if ( to.x > from.x )
            {
                quad.x = 1; // E
            }

            if ( to.y > from.y )
            {
                quad.y = 1; // S
            }

            return quad;
        }

        AABB getQuadrantBox(
            AABB const& bbox,
            Int2 quad
        )
        {
            return AABB{
                { bbox.center.x + ( ( ( quad.x * 2 ) - 1 ) * ( bbox.dimensions.x / 4 ) ),
                  bbox.center.y + ( ( ( quad.y * 2 ) - 1 ) * ( bbox.dimensions.y / 4 ) ) },
                { bbox.dimensions.x / 2,
                  bbox.dimensions.y / 2 }
            };
        }

        void insert(
            Type const& value,
            Float2 const& position,
            Id node,
            AABB const& bbox
        )
        {
            Int2 quad{ getQuadrant(
                bbox.center,
                position
            ) };

            if ( hasBranch(
                     node,
                     quad
                 ) )
            {
                //* Follow
                printf( "Follow branch %i->%i\n", node, nodes_[node].branches[quad.x][quad.y] );
                insert(
                    value,
                    position,
                    nodes_[node].branches[quad.x][quad.y],
                    getQuadrantBox( bbox, quad )
                );

                return;
            }

            if ( hasValue( node ) )
            {
                printf( "Split leaf %i into %i and add value\n", node, nodes_.size() );

                Int2 oldQuad{ getQuadrant(
                    bbox.center,
                    values_[nodes_[node].value].position
                ) };

                //* Divide / Make branch node
                nodes_[node]
                    .branches[oldQuad.x][oldQuad.y] = nodes_.size();
                nodes_.emplace_back();

                //* Reassign old/existing value
                printf( "Add old value to %i\n", nodes_.size() - 1 );
                nodes_.back().value = nodes_[node].value;

                printf( "Reset value of %i\n", node );
                nodes_[node].value = 0;

                //* Add new branch
                printf( "Add value to new %i->%i\n", node, nodes_.size() );
                nodes_[node].branches[quad.x][quad.y] = nodes_.size();
                nodes_.emplace_back();
                nodes_.back().value = value;

                return;
            }

            //* Add value
            printf( "Add value to %i\n", node );
            nodes_[node].value = values_.size();
            values_.emplace_back( Value<Type>{} );
        }
    };
}

#endif

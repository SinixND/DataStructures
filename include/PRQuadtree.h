#ifndef IG20250717092941
#define IG20250717092941

//* https://lisyarus.github.io/blog/posts/building-a-quadtree.html

#include "snxTypes.h"
#include <cstdio>
#include <vector>

namespace snx
{
    template <typename Type>
    class PRQT
    {
        struct Node
        {
            //* [y][x]
            snx::Id children[2][2]{
                { 0, 0 },
                { 0, 0 }
            };
            snx::Id dataId{ 0 };
        };

        struct Data
        {
            snx::Float2 position{};
            Type value{};
        };

        //* TODO: Remove for release
    public:
        std::vector<Node> nodes_{
            Node{},
            Node{}
        };
        std::vector<Data> data_{ Data{} };
        AABB bbox_{};
        Id root_{ 1 };

    public:
        explicit PRQT( Float2 const& dimensions )
            : bbox_( AABB{ { dimensions.x / 2, dimensions.y / 2 }, { dimensions.x, dimensions.y } } )
        {
        }

        void insert(
            Float2 const& position,
            Type const& value
        )
        {
            insert(
                position,
                value,
                root_,
                bbox_
            );
        }

    private:
        bool hasChild(
            Id parentNode,
            Int2 quad
        ) const
        {
            return nodes_[parentNode].children[quad.y][quad.x];
        }

        bool isBranch( Id node ) const
        {
            return nodes_[node].children[0][0]
                   + nodes_[node].children[0][1]
                   + nodes_[node].children[1][0]
                   + nodes_[node].children[1][1];
        }

        bool hasData( Id node ) const
        {
            return nodes_[node].dataId;
        }

        static Int2 getQuadrant(
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

        void insertNode(
            Id node,
            Int2 quad
        )
        {
            nodes_[node].children[quad.y][quad.x] = nodes_.size();
            nodes_.emplace_back();
        }

        void insertData( Data data )
        {
            nodes_.back().dataId = data_.size();
            data_.emplace_back(
                data.position,
                data.value
            );
        }

        void insertToChild(
            Id node,
            Int2 quad,
            Data data
        )
        {
            insertNode(
                node,
                quad
            );

            insertData( data );
        }

        void moveToChild(
            Id node,
            Int2 oldQuad
        )
        {
            insertNode(
                node,
                oldQuad
            );

            nodes_.back().dataId = nodes_[node].dataId;

            nodes_[node].dataId = 0;
        }

        void insert(
            Float2 const& position,
            Type const& value,
            Id node,
            AABB const& bbox
        )
        {
            // Check for equal position
            if (
                data_[nodes_[node].dataId].position.x == position.x
                && data_[nodes_[node].dataId].position.y == position.y

            )
            {
                return;
            }

            Int2 quad{ getQuadrant(
                bbox.center,
                position
            ) };

            if ( hasChild(
                     node,
                     quad
                 ) )
            {
                //* Follow child
                insert(
                    position,
                    value,
                    nodes_[node].children[quad.y][quad.x],
                    getQuadrantBox( bbox, quad )
                );

                return;
            }

            if ( isBranch( node ) )
            {
                insertToChild(
                    node,
                    quad,
                    { position,
                      value }
                );

                return;
            }

            if ( hasData( node ) )
            {
                Int2 oldQuad{ getQuadrant(
                    bbox.center,
                    data_[nodes_[node].dataId].position
                ) };

                moveToChild(
                    node,
                    oldQuad
                );

                insert(
                    position,
                    value,
                    node,
                    bbox
                );

                return;
            }

            insertData(
                { position,
                  value }
            );
        }
    };
}

#endif

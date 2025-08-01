#ifndef IG20250717092941
#define IG20250717092941

//* https://lisyarus.github.io/blog/posts/building-a-quadtree.html

#include "snxMath.h"
#include <algorithm>
#include <raylib.h>

#define GUI

#include "snxTypes.h"
#include <vector>

namespace snx
{
    template <typename Type>
    class PRQT
    {
        struct Node
        {
            //* [y][x]
            Id children[2][2]{
                { 0, 0 },
                { 0, 0 }
            };

            Id dataId{ 0 };
        };

        struct Data
        {
            Float2 position{};
            Type value{};
        };

        struct NearestNeighbor
        {
            float distanceSquared{};
            Float2 position{};
        };

#if defined( GUI )
    public:
#endif
        std::vector<Node> nodes_{
            Node{}, // unused: 0 == invalid
            Node{}  // root
        };

        std::vector<Data> data_{
            Data{} // unused: 0 == invalid
        };

        AABB bbox_{};  // root bbox
        Id root_{ 1 }; // root nodeId

    public:
        explicit PRQT( Float2 const& dimensions )
            : bbox_(
                  AABB{
                      { dimensions.x / 2, dimensions.y / 2 },
                      { dimensions.x / 2, dimensions.y / 2 }
                  }
              )
        {
        }

        void insert(
            Float2 const& position,
            Type const& value
        )
        {
            //* Check for equal position
            if (
                data_[nodes_[node].dataId].position.x == position.x
                && data_[nodes_[node].dataId].position.y == position.y
            )
            {
                return;
            }

            insert(
                position,
                value,
                root_,
                bbox_
            );
        }

        //* https://ericandrewlewis.github.io/how-a-quadtree-works/
        Float2 getNearestNeighbor(
            Float2 const& targetPosition
        )
        {
            NearestNeighbor nearestNeighbor{
                ( 4 * bbox_.halfSize.x * bbox_.halfSize.x )
                + ( 4 * bbox_.halfSize.y * bbox_.halfSize.y )
            };

            getNearestNeighbor(
                nearestNeighbor,
                targetPosition,
                root_,
                bbox_
            );

            return nearestNeighbor.position;
        }

    private:
        void drawBbox(
            AABB const& bbox,
            Color color
        )
        {
            DrawRectangleLinesEx(
                Rectangle{
                    bbox.center.x - bbox.halfSize.x,
                    bbox.center.y - bbox.halfSize.y,
                    2 * bbox.halfSize.x,
                    2 * bbox.halfSize.y
                },
                1,
                color
            );
        }

        bool hasChild(
            Id parentNode,
            Int2 quad
        ) const
        {
            return nodes_[parentNode].children[quad.y][quad.x];
        }

        bool isBranch(
            Id node
        ) const
        {
            return nodes_[node].children[0][0]
                   + nodes_[node].children[0][1]
                   + nodes_[node].children[1][0]
                   + nodes_[node].children[1][1];
        }

        bool hasData(
            Id node
        ) const
        {
            return nodes_[node].dataId;
        }

        static Int2 getQuadrant(
            Float2 parentBbox,
            Float2 targetPosition
        )
        {
            Int2 quad{ 0, 0 }; // NW

            if ( targetPosition.x > parentBbox.x )
            {
                quad.x = 1; // E
            }

            if ( targetPosition.y > parentBbox.y )
            {
                quad.y = 1; // S
            }

            return quad;
        }

        AABB getQuadrantBox(
            AABB const& parentBbox,
            Int2 childQuad
        )
        {
            return AABB{
                { parentBbox.center.x + ( ( ( childQuad.x * 2 ) - 1 ) * ( parentBbox.halfSize.x / 2 ) ),
                  parentBbox.center.y + ( ( ( childQuad.y * 2 ) - 1 ) * ( parentBbox.halfSize.y / 2 ) ) },
                { parentBbox.halfSize.x / 2,
                  parentBbox.halfSize.y / 2 }
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

        void insertData( Data data)
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

            insertData( data);
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
                    getQuadrantBox(
                        bbox,
                        quad
                    )
                );

                return;
            }

            if ( isBranch( node ) )
            {
                insertToChild(
                    node,
                    quad,
                    { position, value }
                );

                return;
            }

            if ( hasData( node ) )
            {
                Int2 oldQuad{
                    getQuadrant(
                        bbox.center,
                        data_[nodes_[node].dataId].position
                    )
                };

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

            insertData( { position, value } );
        }

        bool isCloser(
            AABB const& bbox,
            Float2 const& position,
            float currentNearestDistanceSquared
        )
        {
            //* https://stackoverflow.com/a/18157551/21874203
            float dx{
                std::max(
                    ( bbox.center.x - bbox.halfSize.x ) - position.x,
                    std::max(
                        0.f,
                        position.x - ( bbox.center.x + bbox.halfSize.x )
                    )
                )
            };

            float dy{
                std::max(
                    ( bbox.center.y - bbox.halfSize.y ) - position.y,
                    std::max(
                        0.f,
                        position.y - ( bbox.center.y + bbox.halfSize.y )
                    )
                )
            };

            float distanceSquared = ( dx * dx ) + ( dy * dy );

            return distanceSquared < currentNearestDistanceSquared;
        }

        void checkChild(
            NearestNeighbor& nearestNeighborIO,
            Float2 const& targetPosition,
            Int2 const& childQuad,
            Id parentNode,
            AABB const& childBbox
        )
        {
            if ( hasChild(
                     parentNode,
                     childQuad
                 )
                 && isCloser(
                     childBbox,
                     targetPosition,
                     nearestNeighborIO.distanceSquared
                 ) )
            {
                getNearestNeighbor(
                    nearestNeighborIO,
                    targetPosition,
                    nodes_[parentNode].children[childQuad.y][childQuad.x],
                    childBbox
                );
            }
        }

        void updateNearestNeighbor(
            NearestNeighbor& nearestNeighborIO,
            Float2 const& targetPosition,
            Id leafNode
        )
        {
            float newDistanceSquared{
                getDistanceSquared(
                    targetPosition,
                    data_[nodes_[leafNode].dataId].position
                )
            };

            if ( newDistanceSquared > nearestNeighborIO.distanceSquared )
            {
                return;
            }

            nearestNeighborIO.distanceSquared = newDistanceSquared;

            nearestNeighborIO.position = data_[nodes_[leafNode].dataId].position;
        }

        void cycleQuad( Int2& childQuad )
        {
            Float2 tempQuad{
                1.f * childQuad.x,
                1.f * childQuad.y
            };

            tempQuad.x -= .5f;
            tempQuad.y -= .5f;

            //* Rotate 90 deg CCW
            //* [ 0 -1 ]
            //* [ 1  0 ]
            childQuad.x = static_cast<int>( .5f + ( 1 * tempQuad.y ) );
            childQuad.y = static_cast<int>( .5f + ( -1 * tempQuad.x ) );

            return;
        }

        void checkSiblings(
            NearestNeighbor& nearestNeighborIO,
            Int2 const& firstChildQuad,
            Id parentNode,
            AABB const& parentBbox,
            Float2 const& targetPosition
        )
        {
            //* Check remaining siblings if their
            //* box can be closer than nearestNeighbor
            Int2 childQuad{ firstChildQuad };

            for ( int i{ 0 }; i < 3; ++i )
            {
                cycleQuad( childQuad );
                checkChild(
                    nearestNeighborIO,
                    targetPosition,
                    childQuad,
                    parentNode,
                    getQuadrantBox(
                        parentBbox,
                        childQuad
                    )
                );
            }
        }

        void getNearestNeighbor(
            NearestNeighbor& nearestNeighborIO,
            Float2 const& targetPosition,
            Id node,
            AABB const& nodeBbox
        )
        {
            //* Find quad in direction of target
            Int2 targetQuad{
                getQuadrant(
                    nodeBbox.center,
                    targetPosition
                )
            };

            //* Check first child at targetQuad
            checkChild(
                nearestNeighborIO,
                targetPosition,
                targetQuad,
                node,
                getQuadrantBox(
                    nodeBbox,
                    targetQuad
                )
            );

            //* Deepest node containing target found
            //* Update nearest neighbor if contains data
            if ( hasData( node ) )
            {
                updateNearestNeighbor(
                    nearestNeighborIO,
                    targetPosition,
                    node
                );
            }

            checkSiblings(
                nearestNeighborIO,
                targetQuad,
                node,
                nodeBbox,
                targetPosition
            );
        }
    };
}

#endif

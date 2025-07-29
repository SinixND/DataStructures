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
            snx::Id children[2][2]{
                { 0, 0 },
                { 0, 0 }
            };

            snx::Id dataId{
                0
            };
        };

        struct Data
        {
            snx::Float2 position{};
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
            Node{},
            Node{}
        };

        std::vector<Data> data_{
            Data{}
        };

        AABB bbox_{};
        Id root_{ 1 };

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
                targetPosition,
                root_,
                bbox_,
                nearestNeighbor
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
            return nodes_[node].children[0][0] + nodes_[node].children[0][1] + nodes_[node].children[1][0] + nodes_[node].children[1][1];
        }

        bool hasData(
            Id node
        ) const
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
                { bbox.center.x + ( ( ( quad.x * 2 ) - 1 ) * ( bbox.halfSize.x / 2 ) ),
                  bbox.center.y + ( ( ( quad.y * 2 ) - 1 ) * ( bbox.halfSize.y / 2 ) ) },
                { bbox.halfSize.x / 2,
                  bbox.halfSize.y / 2 }
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

        void insertData(
            Data data
        )
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

            insertData(
                data
            );
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

        void updateDistanceSquared(
            float& distanceSquaredIO,
            Float2 to,
            Float2 from
        )
        {
            distanceSquaredIO = std::min(
                distanceSquaredIO,
                getDistanceSquared(
                    to,
                    from
                )
            );
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

            float dSquared = dx * dx + dy * dy;

            return dSquared < currentNearestDistanceSquared;
        }

        void getNearestNeighbor(
            Float2 const& targetPosition,
            Id node,
            AABB const& currentBbox,
            NearestNeighbor& nearestNeighbor
        )
        {
            //* Find query target node
            Int2 targetQuad{
                getQuadrant(
                    currentBbox.center,
                    targetPosition
                )
            };

            //* Follow target
            if ( hasChild(
                     node,
                     targetQuad
                 ) )
            {
                getNearestNeighbor(
                    targetPosition,
                    nodes_[node].children[targetQuad.y][targetQuad.x],
                    getQuadrantBox(
                        currentBbox,
                        targetQuad
                    ),
                    nearestNeighbor
                );
            }

            //* Deepest node found
            //* Update nearest neighbor if data available
            if ( hasData( node ) )
            {
#if defined( GUI )
                drawBbox(
                    currentBbox,
                    BLUE
                );
#endif

                if ( nearestNeighbor.distanceSquared > getDistanceSquared(
                         targetPosition,
                         data_[nodes_[node].dataId].position
                     ) )
                {
                    updateDistanceSquared(
                        nearestNeighbor.distanceSquared,
                        targetPosition,
                        data_[nodes_[node].dataId].position
                    );

                    nearestNeighbor.position = data_[nodes_[node].dataId].position;
                }
            }

            if ( isBranch( node ) )
            {
                //* Check children if their box can be closer than nearestNeighbor
                for ( int y{ 0 }; y < 2; ++y )
                {
                    for ( int x{ 0 }; x < 2; ++x )
                    {
                        //* Skip child if
                        if (
                            !nodes_[node].children[y][x]
                            || !isCloser(

                                getQuadrantBox(

                                    currentBbox,
                                    { x, y }
                                ),
                                targetPosition,
                                nearestNeighbor.distanceSquared
                            )
                            || ( x == targetQuad.x
                                 && y == targetQuad.y )
                        )
                        {
                            continue;
                        }

                        getNearestNeighbor(
                            targetPosition,
                            nodes_[node].children[y][x],
                            getQuadrantBox(
                                currentBbox,
                                { x, y }
                            ),
                            nearestNeighbor
                        );
                    }
                }
            }
            return;
        }
    };
}

#endif

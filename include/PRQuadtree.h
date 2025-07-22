#ifndef IG20250717092941
#define IG20250717092941

//* https://lisyarus.github.io/blog/posts/building-a-quadtree.html

#include "snxMath.h"
#include <algorithm>
#include <cstdlib>
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
            snx::Id dataId{ 0 };
        };

        struct Data
        {
            snx::Float2 position{};
            Type value{};
        };

        struct Target
        {
            float closestDistanceSquared{};
            Float2 nearestNeighbor{};
            AABB bbox{};
            bool wasFound{ false };
        };

#if defined( GUI )
    public:
#endif
        std::vector<Node> nodes_{
            Node{},
            Node{}
        };
        std::vector<Data> data_{ Data{} };
        AABB bbox_{};
        Id root_{ 1 };

    public:
        explicit PRQT( Float2 const& dimensions )
            : bbox_( AABB{ { dimensions.x / 2, dimensions.y / 2 }, { dimensions.x / 2, dimensions.y / 2 } } )
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

        Float2 getNearestNeighbor( Float2 const& targetPosition )
        {
            Target target{
                ( 4 * bbox_.halfSize.x * bbox_.halfSize.x ) + ( 4 * bbox_.halfSize.y * bbox_.halfSize.y )
            };

            getNearestNeighbor(
                targetPosition,
                root_,
                bbox_,
                target
            );

            return target.nearestNeighbor;
        }

    private:
        void drawBbox(
            AABB const& bbox,
            Color color
        )
        {
#if defined( DEBUG )
            // BeginDrawing();
#endif
            DrawRectangleLinesEx(
                Rectangle{
                    bbox.center.x - bbox.halfSize.x,
                    bbox.center.y - bbox.halfSize.y,
                    2 * bbox.halfSize.x,
                    2 * bbox.halfSize.y
                },
                5,
                color
            );
#if defined( DEBUG )
            // EndDrawing();
#endif
        }

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

        void updateDistanceSquared(
            float& distanceSquaredIO,
            Float2 to,
            Float2 from
        )
        {
            distanceSquaredIO = std::min(
                //* Previous closest distance
                distanceSquaredIO,
                //* New closest distance
                getDistanceSquared(
                    to,
                    from
                )
            );
        }

        void cycleQuad( Int2& quad )
        {
            Float2 tempQuad{ 1.0f * quad.x, 1.0f * quad.y };

            tempQuad.x -= .5f;
            tempQuad.y -= .5f;

            quad.x = static_cast<int>( .5f + ( 1 * tempQuad.y ) );
            quad.y = static_cast<int>( .5f + ( -1 * tempQuad.x ) );

            return;
        }

        bool isAdjacent(
            AABB const& bbox1,
            AABB const& bbox2
        )
        {
            return !( std::abs( bbox1.center.x - bbox2.center.x ) > bbox1.halfSize.x + bbox2.halfSize.x )
                   && !( std::abs( bbox1.center.y - bbox2.center.y ) > bbox1.halfSize.y + bbox2.halfSize.y )
                //    && !( std::abs( bbox1.center.x - bbox2.center.x ) < std::abs( bbox1.halfSize.x - bbox2.halfSize.x ) )
                //    && !( std::abs( bbox1.center.y - bbox2.center.y ) < std::abs( bbox1.halfSize.y - bbox2.halfSize.y ) )
                ;
        }

        void getNearestNeighbor(
            Float2 const& targetPosition,
            Id node,
            AABB const& currentBbox,
            Target& target
        )
        {
            //* Find query target node
            Int2 targetQuad{ getQuadrant(
                currentBbox.center,
                targetPosition
            ) };

            //* Follow children
            if ( hasChild(
                     node,
                     targetQuad
                 ) )
            {
                getNearestNeighbor(
                    targetPosition,
                    nodes_[node].children[targetQuad.y][targetQuad.x],
                    getQuadrantBox( currentBbox, targetQuad ),
                    target
                );
            }

            //* Deepest node found
            if ( 
                !target.wasFound,
            )
            {
                //* Found target
                if(hasData( node )
                || !isBranch(node))
                {
                    target.bbox = currentBbox;
                }
                else 
                {
                    target.bbox = getQuadrantBox( currentBbox, targetQuad );
                }

                target.wasFound = true;
                drawBbox( target.bbox, PURPLE );
            }

            //* Is leaf?
            if (
                hasData( node )
                // && ( target.closestDistanceSquared > getDistanceSquared( targetPosition, data_[nodes_[node].dataId].position ) )
            )
            {
                drawBbox(
                    currentBbox,
                    BLUE
                );
                if ( target.closestDistanceSquared > getDistanceSquared( targetPosition, data_[nodes_[node].dataId].position ) )
                {
                    updateDistanceSquared(
                        target.closestDistanceSquared,
                        targetPosition,
                        data_[nodes_[node].dataId].position
                    );

                    target.nearestNeighbor = data_[nodes_[node].dataId].position;
                }
            }

            //* Check siblings
            if ( !isBranch( node ) )
            {
                return;
            }

            Int2 sibling{ targetQuad };
            for ( int i{ 0 }; i < 3; ++i )
            {
                cycleQuad( sibling );
                if (
                    isAdjacent(
                        getQuadrantBox( currentBbox, sibling ),
                        target.bbox
                    )
                )
                {
                    // drawBbox(
                    //     getQuadrantBox( currentBbox, sibling ),
                    //     RED
                    // );
                    getNearestNeighbor(
                        targetPosition,
                        nodes_[node].children[sibling.y][sibling.x],
                        getQuadrantBox( currentBbox, sibling ),
                        target
                    );
                }
            }
        }
    };
}

#endif

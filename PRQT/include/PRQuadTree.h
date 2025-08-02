#ifndef IG20250717092941
#define IG20250717092941

//* https://lisyarus.github.io/blog/posts/building-a-quadtree.html

#include <algorithm>
#include <cstdint>
#include <raylib.h>
#include <vector>

#define GUI

namespace snx
{
    using Id = std::uint32_t;
    // using Id = size_t;

    struct Int2
    {
        int x, y;
    };

    struct Float2
    {
        float x, y;
    };

    struct AABB
    {
        Float2 center;
        Float2 halfSize;
    };

    template <typename Type>
    class PRQT
    {
        struct Node
        {
            //* [y][x]
            Id quadrants[2][2]{
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
            float distanceFactor{ 0 }; // "1 / (distance ^ 2)": The greater the distance, the smaller the factor
            Id dataId{ 0 };
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
                data_[nodes_[root_].dataId].position.x == position.x
                && data_[nodes_[root_].dataId].position.y == position.y
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
        Id getNearestNeighbor( Float2 const& targetPosition )
        {
            NearestNeighbor nearestNeighbor{};

            getNearestNeighbor(
                nearestNeighbor,
                targetPosition,
                root_,
                bbox_
            );

            return nearestNeighbor.dataId;
        }

    private:
        bool hasQuadrant(
            Id const parentNode,
            Int2 const quadrant
        ) const
        {
            return nodes_[parentNode].quadrants[quadrant.y][quadrant.x];
        }

        bool isParent( Id const node ) const
        {
            return nodes_[node].quadrants[0][0]
                   + nodes_[node].quadrants[0][1]
                   + nodes_[node].quadrants[1][0]
                   + nodes_[node].quadrants[1][1];
        }

        bool hasData( Id const node ) const
        {
            return nodes_[node].dataId;
        }

        static Int2 getQuadrant(
            Float2 const parentBbox,
            Float2 const targetPosition
        )
        {
            Int2 quadrant{ 0, 0 }; // NW

            if ( targetPosition.x > parentBbox.x )
            {
                quadrant.x = 1; // E
            }

            if ( targetPosition.y > parentBbox.y )
            {
                quadrant.y = 1; // S
            }

            return quadrant;
        }

        static AABB getQuadrantBox(
            AABB const& parentBbox,
            Int2 const quadrant
        )
        {
            return AABB{
                { parentBbox.center.x + ( ( ( quadrant.x * 2 ) - 1 ) * ( parentBbox.halfSize.x / 2 ) ),
                  parentBbox.center.y + ( ( ( quadrant.y * 2 ) - 1 ) * ( parentBbox.halfSize.y / 2 ) ) },
                { parentBbox.halfSize.x / 2,
                  parentBbox.halfSize.y / 2 }
            };
        }

        void insertNode(
            Id const node,
            Int2 const quadrant
        )
        {
            nodes_[node].quadrants[quadrant.y][quadrant.x] = nodes_.size();
            nodes_.emplace_back();
        }

        void insertData( Data const data )
        {
            nodes_.back().dataId = data_.size();
            data_.emplace_back(
                data.position,
                data.value
            );
        }

        void insertToQuadrant(
            Id const node,
            Int2 const newQuadrant,
            Data const data
        )
        {
            insertNode(
                node,
                newQuadrant
            );

            insertData( data );
        }

        void moveToQuadrant(
            Id const node,
            Int2 const newQuadrant
        )
        {
            insertNode(
                node,
                newQuadrant
            );

            nodes_.back().dataId = nodes_[node].dataId;
            nodes_[node].dataId = 0;
        }

        void insert(
            Float2 const& position,
            Type const& value,
            Id const node,
            AABB const& bbox
        )
        {
            Int2 targetQuadrant{ getQuadrant(
                bbox.center,
                position
            ) };

            if ( hasQuadrant(
                     node,
                     targetQuadrant
                 ) )
            {
                //* Follow quadrant
                insert(
                    position,
                    value,
                    nodes_[node].quadrants[targetQuadrant.y][targetQuadrant.x],
                    getQuadrantBox(
                        bbox,
                        targetQuadrant
                    )
                );

                return;
            }

            if ( isParent( node ) )
            {
                insertToQuadrant(
                    node,
                    targetQuadrant,
                    { position, value }
                );

                return;
            }

            if ( hasData( node ) )
            {
                Int2 newQuadrant{
                    getQuadrant(
                        bbox.center,
                        data_[nodes_[node].dataId].position
                    )
                };

                moveToQuadrant(
                    node,
                    newQuadrant
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

        static bool isCloser(
            AABB const& bbox,
            Float2 const& position,
            float const oldDistanceFactor
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

            float distanceFactor = 1 / ( ( dx * dx ) + ( dy * dy ) );

            return distanceFactor > oldDistanceFactor;
        }

        void checkQuadrant(
            NearestNeighbor& nearestNeighborIO,
            Float2 const& targetPosition,
            Int2 const& quadrant,
            Id const parentNode,
            AABB const& quadrantBbox
        )
        {
            if ( hasQuadrant(
                     parentNode,
                     quadrant
                 )
                 && isCloser(
                     quadrantBbox,
                     targetPosition,
                     nearestNeighborIO.distanceFactor
                 ) )
            {
                getNearestNeighbor(
                    nearestNeighborIO,
                    targetPosition,
                    nodes_[parentNode].quadrants[quadrant.y][quadrant.x],
                    quadrantBbox
                );
            }
        }

        float getDistanceFactor( Float2 const& to, Float2 const& from ) const
        {
            return 1
                   / ( ( to.x - from.x ) * ( to.x - from.x )
                       + ( to.y - from.y ) * ( to.y - from.y ) );
        }

        void updateNearestNeighbor(
            NearestNeighbor& nearestNeighborIO,
            Float2 const& targetPosition,
            Id const leafNode
        ) const
        {
            float newDistanceFactor{ getDistanceFactor(
                targetPosition,
                data_[nodes_[leafNode].dataId].position
            ) };

            if ( newDistanceFactor < nearestNeighborIO.distanceFactor )
            {
                return;
            }

            nearestNeighborIO.distanceFactor = newDistanceFactor;

            nearestNeighborIO.dataId = nodes_[leafNode].dataId;
        }

        static void cycleQuadrant( Int2& quadrant )
        {
            Float2 tempQuadrant{
                1.f * quadrant.x,
                1.f * quadrant.y
            };

            tempQuadrant.x -= .5f;
            tempQuadrant.y -= .5f;

            //* Rotate 90 deg CCW
            //* [ 0 -1 ]
            //* [ 1  0 ]
            quadrant.x = static_cast<int>( .5f + ( 1 * tempQuadrant.y ) );
            quadrant.y = static_cast<int>( .5f + ( -1 * tempQuadrant.x ) );

            return;
        }

        void checkSiblings(
            NearestNeighbor& nearestNeighborIO,
            Int2 const& firstQuadrantQuadrant,
            Id const parentNode,
            AABB const& parentBbox,
            Float2 const& targetPosition
        )
        {
            //* Check remaining siblings if their
            //* box can be closer than nearestNeighbor
            Int2 quadrant{ firstQuadrantQuadrant };

            for ( int i{ 0 }; i < 3; ++i )
            {
                cycleQuadrant( quadrant );
                checkQuadrant(
                    nearestNeighborIO,
                    targetPosition,
                    quadrant,
                    parentNode,
                    getQuadrantBox(
                        parentBbox,
                        quadrant
                    )
                );
            }
        }

        void getNearestNeighbor(
            NearestNeighbor& nearestNeighborIO,
            Float2 const& targetPosition,
            Id const node,
            AABB const& nodeBbox
        )
        {
            //* Find quadrant in direction of target
            Int2 targetQuadrant{
                getQuadrant(
                    nodeBbox.center,
                    targetPosition
                )
            };

            //* Check first quadrant at targetQuadrant
            checkQuadrant(
                nearestNeighborIO,
                targetPosition,
                targetQuadrant,
                node,
                getQuadrantBox(
                    nodeBbox,
                    targetQuadrant
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
                targetQuadrant,
                node,
                nodeBbox,
                targetPosition
            );
        }
    };
}

#endif

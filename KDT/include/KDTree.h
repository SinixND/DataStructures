#ifndef IG20250801111543
#define IG20250801111543

//* https://opendsa-server.cs.vt.edu/ODSA/Books/CS3/html/KDhtml
//* https://www.geeksforgeeks.org/cpp/kd-trees-in-cpp/

#include <array>
#include <cstdint>
#include <cstdlib>
#include <raylib.h>
#include <vector>

#define GUI

namespace snx
{
    using Id = std::uint32_t;
    // using Id = size_t;

    template <size_t K>
    using KDPosition = std::array<float, K>;

    template <size_t K, typename Type>
    class KDT
    {
        struct Node
        {
            Id branches[2]{
                0,
                0
            };

            Id dataId{ 0 };
        };

        struct Data
        {
            KDPosition<K> position{};
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
        std::vector<Node>
            nodes_{
                Node{}, // unused: 0 == invalid
                Node{}  // root
            };

        std::vector<Data> data_{
            Data{} // unused: 0 == invalid
        };

        Id const root_{ 1 }; // root nodeId

    public:
        void insert(
            KDPosition<K> const& position,
            Type const& value
        )
        {
            //* Check for equal position
            bool isEqual{ true };
            for ( size_t k{ 0 }; k < K; ++k )
            {
                if ( data_[nodes_[root_].dataId].position[k] != position[k] )
                {
                    isEqual = false;
                    break;
                }
            }

            if ( isEqual )
            {
                return;
            }

            if ( !hasData( root_ ) )
            {
                insertData( { position, value } );

                return;
            }

            insert(
                position,
                value,
                root_,
                0
            );
        }

        //* https://youtu.be/Glp7THUpGow?si=p8M3jZT4WqrCgwju&t=267
        Id getNearestNeighbor( KDPosition<2> const& targetPosition )
        {
            NearestNeighbor nearestNeighbor{};

            getNearestNeighbor(
                nearestNeighbor,
                targetPosition,
                root_,
                0
            );

            return nearestNeighbor.dataId;
        }

    private:
        bool hasData( Id const node ) const
        {
            return nodes_[node].dataId;
        }

        void insertData( Data const data )
        {
            nodes_.back().dataId = data_.size();
            data_.emplace_back(
                data.position,
                data.value
            );
        }

        size_t getBranch(
            float const newCoordinate,
            float const oldCoordinate
        )
        {
            return ( newCoordinate < oldCoordinate )
                       ? 0  // less than
                       : 1; // greater than or equal
        }

        bool hasBranch(
            Id const parentNode,
            size_t const branch
        ) const
        {
            return nodes_[parentNode].branches[branch];
        }

        void insertNode(
            Id const node,
            size_t const branch
        )
        {
            nodes_[node].branches[branch] = nodes_.size();
            nodes_.emplace_back();
        }

        void insert(
            KDPosition<K> const& newPosition,
            Type const& newValue,
            Id const parentNode,
            size_t const dimension
        )
        {
            size_t const branch{ getBranch(
                newPosition[dimension],
                data_[nodes_[parentNode].dataId].position[dimension]
            ) };

            if ( hasBranch(
                     parentNode,
                     branch
                 ) )
            {
                insert(
                    newPosition,
                    newValue,
                    nodes_[parentNode].branches[branch],
                    ( dimension + 1 ) % K
                );
            }
            else
            {
                insertNode(
                    parentNode,
                    branch
                );

                insertData( { newPosition, newValue } );
            }
        }

        float getDistanceFactor( KDPosition<K> const& to, KDPosition<K> const& from ) const
        {
            return 1
                   / ( ( to[0] - from[0] ) * ( to[0] - from[0] )
                       + ( to[1] - from[1] ) * ( to[1] - from[1] ) );
        }

        void updateNearestNeighbor(
            NearestNeighbor& nearestNeighborIO,
            KDPosition<K> const& targetPosition,
            Id const node
        ) const
        {
            float newDistanceFactor{ getDistanceFactor(
                targetPosition,
                data_[nodes_[node].dataId].position
            ) };

            if ( newDistanceFactor < nearestNeighborIO.distanceFactor )
            {
                return;
            }

            nearestNeighborIO.distanceFactor = newDistanceFactor;

            nearestNeighborIO.dataId = nodes_[node].dataId;
        }

        bool isBranchCloser(
            Id const node,
            size_t const dimension,
            KDPosition<2> const& targetPosition,
            float const oldDistanceFactor
        ) const
        {
            float branchBorder{
                data_[nodes_[node].dataId].position[dimension]
            };

            float branchDistance{ branchBorder - targetPosition[dimension] };
            float branchDistanceFactor{ 1 / ( branchDistance * branchDistance ) };

            return branchDistanceFactor > oldDistanceFactor;
        }

        void getNearestNeighbor(
            NearestNeighbor& nearestNeighborIO,
            KDPosition<2> const& targetPosition,
            Id const node,
            size_t const dimension
        )
        {
            size_t branch{ getBranch(
                targetPosition[dimension],
                data_[nodes_[node].dataId].position[dimension]
            ) };

            if ( hasBranch( node, branch ) )
            {
                getNearestNeighbor(
                    nearestNeighborIO,
                    targetPosition,
                    nodes_[node].branches[branch],
                    ( dimension + 1 ) % K
                );
            }

            //* Deepest node found
            updateNearestNeighbor(
                nearestNeighborIO,
                targetPosition,
                node
            );

            //* Check other branch
            if (
                hasBranch(
                    node,
                    branch
                )
                && isBranchCloser(
                    node,
                    dimension,
                    targetPosition,
                    nearestNeighborIO.distanceFactor
                )
            )
            {
                branch = ( branch + 1 ) % 2;

                getNearestNeighbor(
                    nearestNeighborIO,
                    targetPosition,
                    nodes_[node].branches[branch],
                    ( dimension + 1 ) % K
                );
            }
        }
    };
}

#endif

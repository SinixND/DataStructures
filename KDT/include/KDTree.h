#ifndef IG20250801111543
#define IG20250801111543

//* https://opendsa-server.cs.vt.edu/ODSA/Books/CS3/html/KDhtml
//* https://www.geeksforgeeks.org/cpp/kd-trees-in-cpp/

#include "snxTypes.h"
#include <array>
#include <vector>

#define GUI

namespace snx
{
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
        KDPosition<2> getNearestNeighbor( KDPosition<2> const& targetPosition )
        {
            KDPosition<2> nearestNeighbor{ 0, 0 };

            getNearestNeighbor(
                nearestNeighbor,
                targetPosition,
                root_
            );

            return nearestNeighbor;
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
            size_t const level
        )
        {
            size_t const dimension{ level % K };

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
                    level + 1
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

        void getNearestNeighbor(
            [[maybe_unused]] KDPosition<2>& nearestNeighbor,
            [[maybe_unused]] KDPosition<2> const targetPosition,
            [[maybe_unused]] Id const node
        )
        {
        }
    };
}

#endif

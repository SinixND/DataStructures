#ifndef IG20250801111543
#define IG20250801111543

//* https://opendsa-server.cs.vt.edu/ODSA/Books/CS3/html/KDtree.html
//* https://www.geeksforgeeks.org/cpp/kd-trees-in-cpp/

#include "snxTypes.h"
#include <array>

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
            Id children[2]{
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

        Id root_{ 1 }; // root nodeId

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
                1
            );
        }

    private:
        bool hasData(
            Id node
        ) const
        {
            return nodes_[node].dataId;
        }

        void insertData( Data data )
        {
            nodes_.back().dataId = data_.size();
            data_.emplace_back(
                data.position,
                data.value
            );
        }

        size_t getDirecion(
            float newCoordinate,
            float oldCoordinate
        )
        {
            return ( newCoordinate < oldCoordinate )
                       ? 0  // less than
                       : 1; // greater than or equal
        }

        bool hasChild(
            Id parentNode,
            size_t direction
        ) const
        {
            return nodes_[parentNode].children[direction];
        }

        void insertNode(
            Id node,
            size_t direction
        )
        {
            nodes_[node].children[direction] = nodes_.size();
            nodes_.emplace_back();
        }

        void insert(
            KDPosition<K> const& position,
            Type const& value,
            Id node,
            size_t level
        )
        {
            size_t dimension{ level % K };
            size_t direction{ getDirecion(
                position[dimension],
                data_[nodes_[node].dataId].position[dimension]
            ) };

            if ( hasChild( node, direction ) )
            {
                insert(
                    position,
                    value,
                    nodes_[node].children[direction],
                    ++level
                );
            }
            else
            {
                insertNode( node, direction );
                insertData( { position, value } );
            }
        }
    };
}

#endif

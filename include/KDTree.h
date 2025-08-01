#ifndef IG20250801111543
#define IG20250801111543

//* https://opendsa-server.cs.vt.edu/ODSA/Books/CS3/html/KDtree.html
//* https://www.geeksforgeeks.org/cpp/kd-trees-in-cpp/

    template<typename K>
    using KDPosition = std::array<float, K>;

namespace snx
{
    template <size_t K, typename Type>
    class KDT
    {
        struct Node
        {
            Id children[2]
            {
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

        size_t const LEFT{0};
        size_t const RIGHT{1};

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
            Float2 const& position,
            Type const& value
        )
        {
            //* Check for equal position
            bool isEqual{true};
            for (size_t k{0}; k < K) 
            {
                if ( data_[nodes_[node].dataId].position[k] != position[k])
                {
                    isEqual = false;
                    break;
                }
            }

            if (isEqual) return;

            if (!hasData(root_))
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

        };

        Float2 getNearestNeighbor(
            Float2 const& targetPosition
        )
        {

        };

    private:
        bool hasData(
            Id node
        ) const
        {
            return nodes_[node].dataId;
        }

        void insertData( Data data)
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
                return (newCoordinate<oldCoordinate) 
                    ? LEFT 
                    : RIGHT;
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
            Float2 const& position,
            Type const& value,
            Id node,
            size_t level
        )
        {
            size_t dimension{level % K};
            size_t direction{getDirecion(
                position[k],
                data_[nodes_[node].dataId].position[k]
            )};

            if (hasChild(direction))
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
                insertNode(node, direction);
                insertData( { position, value } );
            }
        };

        Float2 getNearestNeighbor()
        {

        };
    }
}

#endif
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
            Id childLeft{ 0 };
            Id childRight{ 0 };

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

    }
}

#endif
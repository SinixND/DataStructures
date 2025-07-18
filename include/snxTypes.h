#ifndef IG20250710132640
#define IG20250710132640

#include <cstdint>
// #include <cstddef>

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
        Float2 dimensions;
    };
}
#endif

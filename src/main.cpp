#include "PointRegionQuadTree.h"
#include "RNG.h"

int main()
{
    snx::PRQT<int> qtree{
        { { 50, 50 },
          { 100, 100 } },
        { 0, 0 },
        1
    };

    for ( size_t n{ 2 }; n < 100; ++n )
    {
        qtree.add(
            n,
            float2{
                snx::RNG::random( 0, 5E06 ) / 5E04f,
                snx::RNG::random( 0, 5E06 ) / 5E04f,
            }
        );
    }

    return 0;
}

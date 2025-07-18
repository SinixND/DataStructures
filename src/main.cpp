#include "PRQuadtreeDOD.h"
#include "RNG.h"
#include "snxTypes.h"
#include <cstdio>

using namespace snx;

int main()
{
    PRQT_DOD<int> qt{
        AABB{ { 50, 50 }, { 100, 100 } }
    };

    for ( size_t n{ 1 }; n < 6; ++n )
    {
        printf( "\nINSERT VALUE =====\n" );
        qt.insert(
            n,
            Float2{
                snx::RNG::random( 0, 5E06 ) / 5E04f,
                snx::RNG::random( 0, 5E06 ) / 5E04f,
            }
        );
    }

    return 0;
}

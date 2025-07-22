#ifndef IG20250727001259
#define IG20250727001259

#include "snxTypes.h"

namespace snx
{
    inline float square( float v )
    {
        return v * v;
    }

    inline float getDistanceSquared( Float2 to, Float2 from )
    {
        return ( to.x - from.x ) * ( to.x - from.x )
               + ( to.y - from.y ) * ( to.y - from.y );
    }
}

#endif

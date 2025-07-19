#include "PRQuadtree.h"
#include "RNG.h"
#include "snxTypes.h"
#include <cstdio>
#include <raylib.h>

#define GUI

void renderNode(
    snx::PRQT<int>& tree,
    snx::Id node,
    snx::AABB bbox
)
{
    DrawRectangleLinesEx(
        Rectangle{
            bbox.center.x - ( bbox.dimensions.x / 2 ),
            bbox.center.y - ( bbox.dimensions.y / 2 ),
            bbox.dimensions.x,
            bbox.dimensions.x,
        },
        1,
        WHITE
    );

    for ( int y{ 0 }; y < 2; ++y )
    {
        for ( int x{ 0 }; x < 2; ++x )
        {
            if ( tree.nodes_[node].children[y][x] )
            {
                renderNode(
                    tree,
                    tree.nodes_[node].children[y][x],
                    snx::AABB{
                        { bbox.center.x + ( ( ( x * 2 ) - 1 ) * ( bbox.dimensions.x / 4 ) ),
                          bbox.center.y + ( ( ( y * 2 ) - 1 ) * ( bbox.dimensions.y / 4 ) ) },
                        { bbox.dimensions.x / 2,
                          bbox.dimensions.y / 2 }
                    }
                );
            }
        }
    }

    if ( tree.nodes_[node].dataId )
    {
        DrawCircleV(
            Vector2{
                tree.data_[tree.nodes_[node].dataId].position.x,
                tree.data_[tree.nodes_[node].dataId].position.y
            },
            2,
            RED
        );
    }
}

void renderTree( snx::PRQT<int> tree )
{
    renderNode( tree, tree.root_, tree.bbox_ );
}

int main()
{
    snx::RNG::seed( 1 );
    const int screenWidth = 512;
    const int screenHeight = 512;

#if defined( GUI )
    InitWindow( screenWidth, screenHeight, "raylib [core] example - basic window" );

    SetTargetFPS( 60 ); // Set our game to run at 60 frames-per-second
#endif

    snx::PRQT<int> qt{
        snx::Float2{ screenWidth, screenHeight }
    };

    for ( size_t n{ 1 }; n < 20; ++n )
    {
        qt.insert(
            snx::Float2{
                snx::RNG::random( 0, screenWidth * 10E03 ) / 10E03f,
                snx::RNG::random( 0, screenHeight * 10E03 ) / 10E03f
            },
            n
        );
    }

#if defined( GUI )
    while ( !WindowShouldClose() ) // Detect window close button or ESC key
    {
        BeginDrawing();

        ClearBackground( BLACK );

        renderTree( qt );

        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context
#endif

    return 0;
}

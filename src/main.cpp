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
            bbox.center.x - bbox.halfSize.x,
            bbox.center.y - bbox.halfSize.y,
            2 * bbox.halfSize.x,
            2 * bbox.halfSize.x,
        },
        1,
        GRAY
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
                        { bbox.center.x + ( ( ( x * 2 ) - 1 ) * ( bbox.halfSize.x / 2 ) ),
                          bbox.center.y + ( ( ( y * 2 ) - 1 ) * ( bbox.halfSize.y / 2 ) ) },
                        { bbox.halfSize.x / 2,
                          bbox.halfSize.y / 2 }
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
    const int screenWidth = 1000;
    const int screenHeight = 1000;

#if defined( GUI )
    InitWindow( screenWidth, screenHeight, "raylib [core] example - basic window" );

    SetTargetFPS( 300 ); // Set our game to run at 60 frames-per-second
#endif

    snx::PRQT<int> qt{
        snx::Float2{ screenWidth, screenHeight }
    };

    // for ( size_t n{ 1 }; n < 200; ++n )
    // {
    //     qt.insert(
    //         snx::Float2{
    //             snx::RNG::random( 0, screenWidth * 10E03 ) / 10E03f,
    //             snx::RNG::random( 0, screenHeight * 10E03 ) / 10E03f
    //         },
    //         n
    //     );
    // }

    qt.insert( { 300, 300 }, 1 );
    qt.insert( { 350, 300 }, 2 );
    qt.insert( { 800, 700 }, 3 );
    qt.insert( { 900, 600 }, 4 );

    Vector2 target{ -100, -100 };
    snx::Float2 nearestNeighbor{ -100, -100 };

#if defined( GUI )
    while ( !WindowShouldClose() ) // Detect window close button or ESC key
    {
        BeginDrawing();

        DrawFPS( 10, 10 );

        renderTree( qt );
        if ( IsMouseButtonDown( MOUSE_LEFT_BUTTON ) )
        {
            ClearBackground( BLACK );
            target = GetMousePosition();

            nearestNeighbor = qt.getNearestNeighbor( { target.x, target.y } );

            DrawCircleV( target, 3, PURPLE );
            DrawCircleV( { nearestNeighbor.x, nearestNeighbor.y }, 5, GREEN );
        }

        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context
#endif

    return 0;
}

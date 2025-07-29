#include "PRQuadtree.h"
#include "RNG.h"
#include "snxTypes.h"
#include <cstdio>
#include <raylib.h>

#define GUI

int constexpr SCREEN_WIDTH = 1000;
int constexpr SCREEN_HEIGHT = 1000;
int constexpr NODES = 20;
int constexpr FPS_TARGET = 300;

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
#if defined( GUI )
    InitWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [core] example - basic window" );

    SetTargetFPS( FPS_TARGET ); // Set our game to run at 60 frames-per-second
#endif

    snx::PRQT<int> qt{
        snx::Float2{ SCREEN_WIDTH, SCREEN_HEIGHT }
    };

    for ( size_t n{ 1 }; n < NODES; ++n )
    {
        qt.insert(
            snx::Float2{
                snx::RNG::random( 0, SCREEN_WIDTH * 10E03 ) / 10E03f,
                snx::RNG::random( 0, SCREEN_HEIGHT * 10E03 ) / 10E03f
            },
            n
        );
    }

    // qt.insert( { 60, 60 }, 1 );
    // qt.insert( { 70, 60 }, 2 );
    // qt.insert( { 160, 140 }, 3 );
    // qt.insert( { 180, 120 }, 4 );

    Vector2 target{ -100, -100 };
    snx::Float2 nearestNeighbor{ -100, -100 };

#if defined( GUI )
    while ( !WindowShouldClose() ) // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground( BLACK );

        DrawFPS( 10, 10 );

        // renderTree( qt );
        if ( IsMouseButtonDown( MOUSE_LEFT_BUTTON ) )
        {
            target = GetMousePosition();

            nearestNeighbor = qt.getNearestNeighbor( { target.x, target.y } );

            DrawCircleV( target, 3, PURPLE );
            DrawCircleV( { nearestNeighbor.x, nearestNeighbor.y }, 3, GREEN );
        }

        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context
#endif

    return 0;
}

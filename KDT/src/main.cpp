#include "KDTree.h"
#include "RNG.h"
#include "snxTypes.h"
#include <raylib.h>

#define GUI

int constexpr SCREEN_WIDTH = 1024;
int constexpr SCREEN_HEIGHT = 1024;
int constexpr NODES = 20;
int constexpr FPS_TARGET = 300;

void renderNode(
    snx::KDT<2, int> const& tree,
    snx::Id node
)
{
    if ( tree.nodes_[node].dataId )
    {
        DrawCircleV(
            Vector2{
                tree.data_[tree.nodes_[node].dataId].position[0],
                tree.data_[tree.nodes_[node].dataId].position[1]
            },
            2,
            RED
        );
    }

    for ( int k{ 0 }; k < 2; ++k )
    {
        if ( !tree.nodes_[node].children[k] )
        {
            continue;
        }

        renderNode(
            tree,
            tree.nodes_[node].children[k]
        );
    }
}

void renderTree( snx::KDT<2, int> const& tree )
{
    renderNode( tree, tree.root_ );
}

int main()
{
    snx::RNG::seed( 1 );
#if defined( GUI )
    InitWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [core] example - basic window" );

    SetTargetFPS( FPS_TARGET ); // Set our game to run at 60 frames-per-second
#endif

    snx::KDT<2, int> tree{};

    for ( size_t n{ 1 }; n < NODES; ++n )
    {
        tree.insert(
            snx::KDPosition<2>{
                snx::RNG::random( 0, SCREEN_WIDTH * 10E03 ) / 10E03f,
                snx::RNG::random( 0, SCREEN_HEIGHT * 10E03 ) / 10E03f
            },
            n
        );
    }

    // tree.insert( { 60, 60 }, 1 );
    // tree.insert( { 70, 60 }, 2 );
    // tree.insert( { 160, 140 }, 3 );
    // tree.insert( { 180, 120 }, 4 );

    Vector2 target{ -100, -100 };

#if defined( GUI )
    while ( !WindowShouldClose() ) // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground( BLACK );

        DrawFPS( 10, 10 );

        renderTree( tree );
        if ( IsMouseButtonDown( MOUSE_LEFT_BUTTON ) )
        {
            target = GetMousePosition();

            DrawCircleV( target, 4, PURPLE );
        }

        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context
#endif

    return 0;
}

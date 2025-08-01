#include "KDTree.h"
#include "RNG.h"
#include "snxTypes.h"
#include <cstddef>
#include <raylib.h>

#define GUI

int constexpr SCREEN_WIDTH = 1024;
int constexpr SCREEN_HEIGHT = 1024;
[[maybe_unused]] int constexpr NODES = 20;
int constexpr FPS_TARGET = 300;

void renderNode(
    snx::KDT<2, int> const& tree,
    snx::Id const node,
    size_t const level,
    Rectangle const& parentBox
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

        DrawRectangleLinesEx(
            parentBox,
            1,
            GRAY
        );
    }

    size_t const d = level % 2; // Dimension: x{0} or y{1}

    for ( int b{ 0 }; b < 2; ++b ) // Branch: smaller{0} or greater{1}
    {
        if ( !tree.nodes_[node].branches[b] )
        {
            continue;
        }

        Rectangle branchBox{ parentBox };

        if ( !b ) // smaller
        {
            if ( !d ) // x
            {
                branchBox.width = tree.data_[tree.nodes_[node].dataId].position[d] - parentBox.x;
            }
            else // y
            {
                branchBox.height = tree.data_[tree.nodes_[node].dataId].position[d] - parentBox.y;
            }
        }
        else // greater
        {
            if ( !d ) // x
            {
                branchBox.width = parentBox.x + parentBox.width - tree.data_[tree.nodes_[node].dataId].position[d];

                branchBox.x = tree.data_[tree.nodes_[node].dataId].position[d];
            }
            else // y
            {
                branchBox.height = parentBox.y + parentBox.height - tree.data_[tree.nodes_[node].dataId].position[d];

                branchBox.y = tree.data_[tree.nodes_[node].dataId].position[d];
            }
        }

        renderNode(
            tree,
            tree.nodes_[node].branches[b],
            level + 1,
            branchBox
        );
    }
}

void renderTree(
    snx::KDT<2, int> const& tree,
    int const screenWidth,
    int const screenHeight
)
{
    Rectangle rootBox{
        0,
        0,
        (float)screenWidth,
        (float)screenHeight
    };

    renderNode(
        tree,
        tree.root_,
        0,
        rootBox
    );
}

int main()
{
    snx::RNG::seed( 3 );
#if defined( GUI )
    InitWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [core] example - basic window" );

    SetTargetFPS( FPS_TARGET ); // Set our game to run at 60 frames-per-second
#endif

    snx::KDT<2, int> tree{};

    // tree.insert( { 60, 60 }, 1 );
    // tree.insert( { 70, 60 }, 2 );
    // tree.insert( { 160, 140 }, 3 );
    // tree.insert( { 180, 120 }, 4 );

    Vector2 target{ -100, -100 };

    size_t n{ 0 };

#if defined( GUI )
    while ( !WindowShouldClose() ) // Detect window close button or ESC key
    {
        if ( n < NODES + 1 )
        {
            tree.insert(
                snx::KDPosition<2>{
                    snx::RNG::random( 0, SCREEN_WIDTH * 10E03 ) / 10E03f,
                    snx::RNG::random( 0, SCREEN_HEIGHT * 10E03 ) / 10E03f
                },
                n
            );

            ++n;
        }

        BeginDrawing();
        ClearBackground( BLACK );

        DrawFPS( 10, 10 );

        renderTree(
            tree,
            SCREEN_WIDTH,
            SCREEN_HEIGHT
        );

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

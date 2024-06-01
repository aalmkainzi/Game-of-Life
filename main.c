// config
#define GRID_W 100
#define GRID_H 100
#define BACKGROUND BLACK
#define LINE_COLOR BLANK
#define BORDER_COLOR YELLOW
#define CELL_SIZE 10.0
#define CELL_COLOR ORANGE
#define CELL_SHAPE CIRCLE
#define HOVER_COLOR SKYBLUE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#if defined(__linux__)
#include "raylib_linux/include/raylib.h"
#include "raylib_linux/include/rlgl.h"
#include "raylib_linux/include/raymath.h"
#elif defined(_WIN32)
#include "raylib_windows/include/raylib.h"
#include "raylib_windows/include/rlgl.h"
#include "raylib_windows/include/raymath.h"
#endif

bool grid [GRID_H][GRID_W] = { 0 };
bool grid2[GRID_H][GRID_W] = { 0 };

typedef struct {
    bool n1: 1;
    bool n2: 1;
    bool n3: 1;
    bool n4: 1;
    bool n5: 1;
    bool n6: 1;
    bool n7: 1;
    bool n8: 1;
} Neighbors;

Neighbors get_neighbors(bool(*grid)[GRID_H][GRID_W], int x, int y);
bool new_state(bool is_alive, Neighbors nbrs);
void iclamp(int *num, int min, int max);
bool time_elapsed(double seconds);

enum
{
    CIRCLE,
    SQUARE,
    TRIANGLE
} Cell_Shape;

int main()
{
    srand(time(NULL));
    
    bool(*current_grid)[GRID_H][GRID_W] = &grid;
    bool(*other_grid)  [GRID_H][GRID_W] = &grid2;
    
    int window_w = CELL_SIZE * GRID_W;
    int window_h = CELL_SIZE * GRID_H;
    
    Camera2D camera = { 0 };
    camera.zoom = 1;
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(window_w, window_h, "Game Of Life");
    
    bool is_running = false;
    double tick_diff = 0.1;
    
    while(!WindowShouldClose())
    {
        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(BACKGROUND);
        
        Vector2 mouse = GetMousePosition();
        Vector2 mouse_world = GetScreenToWorld2D(mouse, camera);
        
        int hovered_cellx = (mouse_world.x) / (CELL_SIZE);
        int hovered_celly = (mouse_world.y) / (CELL_SIZE);
        iclamp(&hovered_cellx, 0, GRID_W - 1);
        iclamp(&hovered_celly, 0, GRID_H - 1);
        
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            // dragging
            Vector2 delta = GetMouseDelta();
            
            camera.target.x = camera.target.x - (delta.x * 1/camera.zoom);
            camera.target.y = camera.target.y - (delta.y * 1/camera.zoom);
        }
        
        float scroll = GetMouseWheelMove();
        if(scroll != 0 || (IsKeyDown(KEY_MINUS) && camera.zoom >= 0.75) || (IsKeyDown(KEY_EQUAL) && camera.zoom <= 17))
        {
            camera.offset = mouse;
            camera.target = mouse_world;
            
            int dir = 1;
            if(scroll < 0)
                dir = -1;
            else if(IsKeyDown(KEY_MINUS) && camera.zoom >= 0.75)
                dir = -1;
            
            camera.zoom += dir * 0.125;
            
            camera.zoom = Clamp(camera.zoom, 0.75, 17);
        }
        
        if(!is_running && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            (*current_grid)[hovered_celly][hovered_cellx] ^= 1;
        }
        
        if(IsKeyPressed(KEY_C) && !is_running)
        {
            memset(current_grid, 0, sizeof(grid));
        }
        if(IsKeyPressed(KEY_R) && !is_running)
        {
            memset(current_grid, 0, sizeof(grid));
            for(int i = 0 ; i < GRID_H ; i++)
                for(int j = 0 ; j < GRID_W ; j++)
                    (*current_grid)[i][j] = rand() % 2;
        }
        if(IsKeyPressed(KEY_S) || IsKeyPressed(KEY_SPACE))
        {
            is_running = !is_running;
        }
        if(IsKeyPressed(KEY_UP) && tick_diff >= 0.0001)
        {
            tick_diff -= 0.01;
        }
        if(IsKeyPressed(KEY_DOWN) && tick_diff <= 4)
        {
            tick_diff += 0.01;
        }
        
        // Color the hovered cell
        DrawRectangle(hovered_cellx * CELL_SIZE, hovered_celly * CELL_SIZE, CELL_SIZE, CELL_SIZE, HOVER_COLOR);
        
        for(int i = 0 ; i < GRID_H ; i++)
        {
            for(int j = 0 ; j < GRID_W ; j++)
            {
                if((*current_grid)[i][j])
                {
                    switch(CELL_SHAPE)
                    {
                        case CIRCLE:
                            DrawCircle(j * CELL_SIZE + CELL_SIZE/2, i * CELL_SIZE + CELL_SIZE/2, CELL_SIZE / 2, CELL_COLOR);
                            break;
                        case SQUARE:
                            DrawRectangle(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, CELL_COLOR);
                            break;
                        case TRIANGLE:
                            DrawTriangle(
                                (Vector2){j * CELL_SIZE + CELL_SIZE/2, i * CELL_SIZE},
                                (Vector2){j * CELL_SIZE, i * CELL_SIZE + CELL_SIZE},
                                (Vector2){j * CELL_SIZE + CELL_SIZE, i * CELL_SIZE + CELL_SIZE},
                                CELL_COLOR
                            );
                            break;
                    }
                }
            }
        }
        
        // left border
        DrawLine(
                0, 0,
                (CELL_SIZE * GRID_W), 0,
                BORDER_COLOR
        );
        // right border
        DrawLine(
            0, (GRID_H * CELL_SIZE),
            (CELL_SIZE * GRID_W), (GRID_H * CELL_SIZE),
            BORDER_COLOR
        );
        // top border
        DrawLine(
            0, 0,
            0, (CELL_SIZE * GRID_H),
            BORDER_COLOR
        );
        // bottom border
        DrawLine(
            (GRID_W * CELL_SIZE), 0,
            (GRID_W * CELL_SIZE), (CELL_SIZE * GRID_H),
            BORDER_COLOR
        );
        
        for(int i = 1 ; i < GRID_H ; i++)
        {
            DrawLine(
                0, (i * CELL_SIZE),
                (CELL_SIZE * GRID_W), (i * CELL_SIZE),
                LINE_COLOR
            );
        }
        
        for(int i = 1 ; i < GRID_W ; i++)
        {
            DrawLine(
                (i * CELL_SIZE), 0,
                (i * CELL_SIZE), (CELL_SIZE * GRID_H),
                LINE_COLOR
            );
        }
        
        if(is_running && time_elapsed(tick_diff))
        {
            for(int i = 0 ; i < GRID_H ; i++)
            {
                for(int j = 0 ; j < GRID_W ; j++)
                {
                    bool cell_state = (*current_grid)[i][j];
                    Neighbors nbors = get_neighbors(current_grid, j, i);
                    bool next_cell_state = new_state(cell_state, nbors);
                    (*other_grid)[i][j] = next_cell_state;
                }
            }
            
            // swapping the matrices
            void *temp = current_grid;
            current_grid = other_grid;
            other_grid = temp;
        }
        
        EndMode2D();
        EndDrawing();
    }
    
    return 0;
}

Neighbors get_neighbors(bool(*game_grid)[GRID_H][GRID_W], int x, int y)
{
    Neighbors ret = { 0 };
    
    int y_from_bottom     = GRID_H - y;
    int x_from_right      = GRID_W - x;
    
    int top_neighbor_y    = GRID_H - (y_from_bottom % GRID_H) - 1;
    int bottom_neighbor_y = (y + 1) % GRID_H;
    int right_neighbor_x  = (x + 1) % GRID_W;
    int left_neighbor_x   = GRID_W - (x_from_right % GRID_W) - 1;
    
    ret.n1 = (*game_grid)[top_neighbor_y][x];
    ret.n2 = (*game_grid)[top_neighbor_y][right_neighbor_x];
    ret.n3 = (*game_grid)[y][right_neighbor_x];
    ret.n4 = (*game_grid)[bottom_neighbor_y][right_neighbor_x];
    ret.n5 = (*game_grid)[bottom_neighbor_y][x];
    ret.n6 = (*game_grid)[bottom_neighbor_y][left_neighbor_x];
    ret.n7 = (*game_grid)[y][left_neighbor_x];
    ret.n8 = (*game_grid)[top_neighbor_y][left_neighbor_x];
    
    return ret;
}

bool new_state(bool is_alive, Neighbors nbrs)
{
    union {
        Neighbors nbrs;
        unsigned char c;
    } nbrs_u8;
    nbrs_u8.nbrs = nbrs;
    
    int count =
#ifdef _MSC_VER
__popcnt(nbrs_u8.c);
#else
__builtin_popcount(nbrs_u8.c);
#endif
    
    if(is_alive && (count == 2 || count == 3))
    {
        return true;
    }
    if(!is_alive && count == 3)
    {
        return true;
    }
    
    return false;
}

void iclamp(int *num, int min, int max)
{
    if(*num > max)
        *num = max;
    else if(*num < min)
        *num = min;
}

bool time_elapsed(double seconds)
{
    static bool first_call = true;
    static double prev_time;
    if(first_call)
    {
        first_call = false;
        prev_time = GetTime();
        return true;
    }
    
    double now = GetTime();
    if((now - prev_time) >= seconds)
    {
        prev_time = now;
        return true;
    }
    
    return false;
}

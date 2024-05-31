#include "raylib/include/raylib.h"
#include "raylib/include/rlgl.h"
#include "raylib/include/raymath.h"
#include <stdbool.h>
#include <stdio.h>

#define GRID_W 100
#define GRID_H 100

bool grid [GRID_H][GRID_W] = { 0 };
bool grid2[GRID_H][GRID_W] = { 0 };

typedef struct {
    bool n[8];
} Neighbors;

Neighbors get_neighbors(bool(*grid)[GRID_H][GRID_W], int x, int y);
bool new_state(bool is_alive, Neighbors nbrs);
void iclamp(int *num, int min, int max);
bool time_elapsed(double seconds);

int main()
{
    bool(*current_grid)[GRID_H][GRID_W] = &grid;
    bool(*other_grid)  [GRID_H][GRID_W] = &grid2;
    
    float cell_size = 10.0f;
    
    const int WINDOW_W = cell_size * GRID_W;
    const int WINDOW_H = cell_size * GRID_H;
    
    Camera2D camera = { 0 };
    camera.zoom = 1;
    
    InitWindow(WINDOW_W, WINDOW_H, "Game Of Life");
    SetTargetFPS(60);
    
    enum
    {
        RUNNING,
        STOPPED,
    } current_state = STOPPED;
    
    while(!WindowShouldClose())
    {
        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(WHITE);
        
        Vector2 mouse = GetMousePosition();
        
        Vector2 mouse_world = GetScreenToWorld2D(mouse, camera);
        
        int hovered_cellx = (mouse_world.x) / (cell_size);
        int hovered_celly = (mouse_world.y) / (cell_size);
        
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
            Vector2 mouse_world = GetScreenToWorld2D(GetMousePosition(), camera);
            
            camera.offset = GetMousePosition();
            
            camera.target = mouse_world;
            
            int dir = 1;
            if(scroll < 0)
                dir = -1;
            else if(IsKeyDown(KEY_MINUS) && camera.zoom >= 0.75)
                dir = -1;
            
            camera.zoom += dir * 0.125;
            
            camera.zoom = Clamp(camera.zoom, 0.75, 17);
        }
        
        if(current_state == STOPPED && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            (*current_grid)[hovered_celly][hovered_cellx] = true;
        }
        
        if(IsKeyPressed(KEY_S))
        {
            current_state = (current_state == STOPPED) ? RUNNING : STOPPED;
        }
        
        for(int i = 0 ; i < GRID_H ; i++)
        {
            for(int j = 0 ; j < GRID_W ; j++)
            {
                if((*current_grid)[i][j])
                    DrawCircle(j * cell_size + cell_size/2, i * cell_size + cell_size/2, cell_size / 2, BLUE);
            }
        }
        
        DrawRectangle(hovered_cellx * cell_size, hovered_celly * cell_size, cell_size, cell_size, SKYBLUE);
        
        for(int i = 0 ; i <= GRID_H ; i++)
        {
            DrawLine(
                0, (i * cell_size),
                (cell_size * GRID_W), (i * cell_size),
                BLACK
            );
        }
        
        for(int i = 0 ; i <= GRID_W ; i++)
        {
            DrawLine(
                (i * cell_size), 0,
                (i * cell_size), (cell_size * GRID_H),
                BLACK
            );
        }
        
        if(current_state == RUNNING && time_elapsed(0.5))
        {
            for(int i = 0 ; i < GRID_H ; i++)
            {
                for(int j = 0 ; j < GRID_W ; j++)
                {
                    (*other_grid)[i][j] = new_state((*current_grid)[i][j], get_neighbors(current_grid, j, i));
                }
            }
            
            // swapping the matrices
            typeof(current_grid) temp = current_grid;
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
    
    int top_neighbor_y    = y_from_bottom % GRID_H;
    int bottom_neighbor_y = (y + 1) % GRID_H;
    int right_neighbor_x  = (x + 1) % GRID_W;
    int left_neighbor_x   = x_from_right % GRID_W;
    
    ret.n[0] = (*game_grid)[top_neighbor_y][x];
    ret.n[1] = (*game_grid)[top_neighbor_y][right_neighbor_x];
    ret.n[2] = (*game_grid)[y][right_neighbor_x];
    ret.n[3] = (*game_grid)[bottom_neighbor_y][right_neighbor_x];
    ret.n[4] = (*game_grid)[bottom_neighbor_y][x];
    ret.n[5] = (*game_grid)[bottom_neighbor_y][left_neighbor_x];
    ret.n[6] = (*game_grid)[y][left_neighbor_x];
    ret.n[7] = (*game_grid)[top_neighbor_y][left_neighbor_x];
    
    return ret;
}

bool new_state(bool is_alive, Neighbors nbrs)
{
    int count = 0;
    for(int i = 0 ; i < 8 ; i++)
        count += nbrs.n[i];
    
    if(is_alive && count < 2)
    {
        return false;
    }
    if(is_alive && (count == 2 || count == 3))
    {
        return true;
    }
    if(is_alive && count > 2)
    {
        return false;
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
        prev_time = GetTime();
        return true;
    }
    
    if((GetTime() - prev_time) >= seconds)
    {
        return true;
    }
    else
    {
        return false;
    }
}

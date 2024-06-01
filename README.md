# Game of Life

call `make` in directory to build.

# Config
At the top of the `main.c` there's some configuration options:
```C
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
```

# Controls
- right click to put live cell
- space to start or stop
- left click and drag to move
- scroll to zoom
- up/down arrow to change tick speed
- C to clear screen
- R to make random grid

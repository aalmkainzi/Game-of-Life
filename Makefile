gol: main.c
	gcc main.c raylib/lib/libraylib.a -o gol -lgdi32 -lwinmm -Wall -Wextra
debug: main.c
	gcc -ggdb main.c raylib/lib/libraylib.a -o gol -lgdi32 -lwinmm -Wall -Wextra

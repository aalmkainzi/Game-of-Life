gol: main.c
	gcc main.c raylib/lib/libraylib.a -o gol -lGL -lm -lpthread -ldl -lrt -lX11 -Wall -Wextra
debug: main.c
	gcc -ggdb main.c raylib/lib/libraylib.a -o gol -lGL -lm -lpthread -ldl -lrt -lX11 -Wall -Wextra

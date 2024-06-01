UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
	CFLAGS = raylib_linux/lib/libraylib.a -lGL -lm -lpthread -ldl -lrt
endif
ifeq ($(UNAME), Windows_NT)
	CFLAGS = raylib_windows/lib/libraylib.a -lgdi32 -lwinmm
endif

gol: main.c
	gcc -O2 main.c $(CFLAGS) -o gol -Wall -Wextra
debug: main.c
	gcc -ggdb main.c $(CFLAGS) -o gol -lX11 -Wall -Wextra

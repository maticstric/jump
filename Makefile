CC=gcc
CFLAGS= -std=c99 -Wall

LDFLAGS= -I/opt/homebrew/include -I./include -L/opt/homebrew/lib -lSDL2 -lSDL2_image

all: src/*.c
		$(CC) $(CFLAGS) $? $(LDFLAGS)

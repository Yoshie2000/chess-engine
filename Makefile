CC = gcc
CFLAGS = -Wall -pedantic -Wextra -fcommon -O3 -fgnu89-inline -funroll-all-loops

SOURCES = src/engine.c src/board.c src/move.c src/types.c
OBJS = $(patsubst %.c,%.o, $(SOURCES))

all:	engine

%o:		%.c
		$(CC) $(CFLAGS) -c $<

engine:	$(OBJS)
		$(CC) $(CFLAGS) $^ -o $@

clean:	
		$(RM) src/*.o *~ engine
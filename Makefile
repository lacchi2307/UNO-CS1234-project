CC     = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS   = -lncurses

SRCS = main.c game.c card.c display.c
OBJS = $(SRCS:.c=.o)

uno: $(OBJS)
	$(CC) $(CFLAGS) -o uno $(OBJS) $(LIBS)

%.o: %.c uno.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f uno $(OBJS)

.PHONY: clean

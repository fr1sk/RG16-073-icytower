PROGRAM = IcyTower
CC = gcc
CFLAGS = -g -Wall -I/usr/X11R6/include -I/usr/pkg/include
LDFLAGS = -lGL -lGLU -lglut -lm
SOURCE = icyTower.c

$(PROGRAM): $(SOURCE)
		$(CC) -o $(PROGRAM) $(SOURCE) $(CFLAGS) $(LDFLAGS)

.PHONY: clean dist

beauty:
	-indent -kr -nut main.c
	- rm *~ *BAK

clean:
	-rm *.o $(PROGRAM) *core

dist: clean
	-tar -chvj -C .. -f ../$(PROGRAM).tar.bz2 $(PROGRAM)

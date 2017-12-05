PROGRAM=emux86
OBJECTS=main.o util.o cpu.o
CFLAGS=-g -O0 -Wall
LDLIBS=
CC=gcc

all: emux86

$(PROGRAM): $(OBJECTS)
	$(CC) $^ -o $@ $(LDLIBS)

%.o: %.c
	$(CC) -c $^ $(CFLAGS) -o $@

.phony: clean

clean:
	rm -f $(PROGRAM) *.o

CC = gcc
CFLAGS = -Wall -std=gnu17 -g
LDFLAGS = -L../serializer -lserializer -pthread

.PHONY: clean

default: monkey-crossing.o

monkey-crossing.o: monkey-crossing.c monkey-crossing.h
	$(CC) $(CFLAGS) -I.. -c monkey-crossing.c $(LDFLAGS)

clean:
	rm -f *.o

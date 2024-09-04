CC = gcc
CFLAGS = -Wall -std=gnu17 -g
LDFLAGS = -L../serializer -lserializer -pthread

.PHONY: clean

default: reader-writer.o

reader-writer.o: reader-writer.c reader-writer.h
	$(CC) $(CFLAGS) -I.. -c reader-writer.c $(LDFLAGS)

clean:
	rm -f *.o

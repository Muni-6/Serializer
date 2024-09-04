CC = gcc
CFLAGS = -Wall -std=gnu17 -g
LDFLAGS = -L../serializer -lserializer -pthread

.PHONY: clean

default: child-care.o

child-care.o: child-care.c child-care.h
	$(CC) $(CFLAGS) -I.. -c child-care.c $(LDFLAGS)

clean:
	rm -f *.o

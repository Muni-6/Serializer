CC = gcc
CFLAGS = -Wall -std=gnu17 -g
LDFLAGS = -pthread

.PHONY: clean

default: libserializer.a

libserializer.a: serializer.o
	$(AR) rcs $@ $^

serializer.o: serializer.c serializer.h
	$(CC) $(CFLAGS) -o serializer.o -c serializer.c

clean:
	rm -f *.o lib*

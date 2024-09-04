CC = gcc
CFLAGS = -Wall -std=gnu17 -g
LDFLAGS = -L../serializer -lserializer -pthread

.PHONY: clean

default: out-tester-2

out-tester-2: main.c ../problem-reader-writer/reader-writer.o
	$(CC) $(CFLAGS) -I.. -o out-tester-2 main.c ../problem-reader-writer/reader-writer.o $(LDFLAGS)

clean:
	rm -f out*

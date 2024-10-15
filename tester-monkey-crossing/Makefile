CC = gcc
CFLAGS = -Wall -std=gnu17 -g
LDFLAGS = -L../serializer -lserializer -pthread

.PHONY: clean

default: out-tester-monkey-crossing

out-tester-monkey-crossing: main.c ../problem-monkey-crossing/monkey-crossing.o
	$(CC) $(CFLAGS) -I.. -o out-tester-monkey-crossing main.c ../problem-monkey-crossing/monkey-crossing.o $(LDFLAGS)

clean:
	rm -f out*

CC = gcc
CFLAGS = -Wall -std=gnu17 -g
LDFLAGS = -L../serializer -lserializer -pthread

.PHONY: clean

default: out-tester-child-care-2

out-tester-child-care-2: main.c ../problem-child-care/child-care.o
	$(CC) $(CFLAGS) -I.. -o out-tester-child-care-2 main.c ../problem-child-care/child-care.o $(LDFLAGS)

clean:
	rm -f out*

SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=./server

all: build
	$(EXECUTABLE)

build: $(OBJECTS)
	gcc -o $(EXECUTABLE) $^

.c.o:
	gcc -o $@ -c -Wall -Wextra $<

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
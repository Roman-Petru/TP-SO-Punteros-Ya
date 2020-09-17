TARGET_LIB = libred.a
LIBS = -lcommons -lpthread -lnetdb
CC = gcc
CFLAGS = -Wall -g

.PHONY: default all clean

default: $(TARGET_LIB)
all: clean default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET_LIB): $(OBJECTS)
	ar rcs $(TARGET_LIB) $(OBJECTS)

clean:
	-rm -f *.o
	-rm -f $(TARGET_LIB)

compilarSiNoExiste:
	if [ -f ./libred.a ]; then echo libRed ya compilado!; else echo Compilando libRed...; make default; fi
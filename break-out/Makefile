CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = break-out

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

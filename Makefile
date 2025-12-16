CC      := clang
CFLAGS  := -Os -Wall -Wextra
TARGET  := cwol
SRC     := main.c

.PHONY: all run clean

all: $(TARGET)

run: all
	./$(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)


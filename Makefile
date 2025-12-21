CC       ?= clang
CFLAGS   ?= -Os -Wall -Wextra -DDEBUG
LDFLAGS  ?= -lsqlite3
TARGET   := cwol
OBJDIR   := build
SRC      := $(wildcard *.c)
HEADERS  := $(wildcard *.h)
OBJ      := $(patsubst %.c, $(OBJDIR)/%.o, $(SRC))

.PHONY: all run install clean 

all: $(TARGET)

run: all
	./$(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: %.c $(HEADERS) | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

install: all
	install -Dm755 $(TARGET) $(DESTDIR)/usr/bin

$(OBJDIR):
	mkdir -p $@

clean:
	rm -rf $(TARGET) $(OBJDIR)

clean_db:
	rm -rf ~/.local/share/cwol && ./setup.sh

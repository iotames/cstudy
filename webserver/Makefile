CC=gcc
CFLAGS=-Iinclude -Wall -Wextra -g
SRCDIR=src
OBJDIR=obj
BINDIR=.
SOURCES=$(SRCDIR)/main.c $(SRCDIR)/http_server.c $(SRCDIR)/router.c
OBJECTS=$(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET=webserver.exe

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir $(OBJDIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

all: $(TARGET)

clean:
	del /Q $(OBJDIR)\*.o 2>nul
	del /Q $(TARGET) 2>nul

.PHONY: all clean

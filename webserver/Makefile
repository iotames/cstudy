CC=gcc
CFLAGS=-Iinclude -Wall -Wextra -g
SRCDIR=src
OBJDIR=obj
SOURCES=$(SRCDIR)/main.c $(SRCDIR)/http_server.c $(SRCDIR)/router.c
OBJECTS=$(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET=webserver.exe

all: $(TARGET)


$(TARGET): $(OBJECTS)
# Windows 下 socket 相关函数需要手动链接 ws2_32 库。
	$(CC) $(CFLAGS) $^ -o $@ -lws2_32

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q $(OBJDIR)\*.o 2>nul
	del /Q $(TARGET) 2>nul

.PHONY: all clean
CC=gcc
CFLAGS=-Iinclude -Wall -Wextra -g
SRCDIR=src
OBJDIR=obj
SOURCES=$(SRCDIR)/main.c $(SRCDIR)/http_server.c $(SRCDIR)/router.c
OBJECTS=$(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# 根据操作系统设置目标文件名和链接库
ifeq ($(OS),Windows_NT)
	TARGET=webserver.exe
	LIBS=-lws2_32
	MKDIR=if not exist $(OBJDIR) mkdir $(OBJDIR)
	RM=del /Q
	RMOBJ=$(OBJDIR)\*.o
	RMTARGET=$(TARGET)
	RMREDIR=2>nul
else
	TARGET=webserver
	LIBS=
	MKDIR=mkdir -p $(OBJDIR)
	RM=rm -f
	RMOBJ=$(OBJDIR)/*.o
	RMTARGET=$(TARGET)
	RMREDIR=
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-$(RM) $(RMOBJ) $(RMREDIR) || true
	-$(RM) $(RMTARGET) $(RMREDIR) || true

.PHONY: all clean
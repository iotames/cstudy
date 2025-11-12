CC = gcc
# -D_GNU_SOURCE 这个标志用于启用GNU扩展，包括alloca函数的声明。在没有GNU扩展的环境中，alloca可能不可见。
CFLAGS = -Wall -g -std=c99 -D_GNU_SOURCE
LIBS = -lpthread -lsqlite3
SOURCES = product_server.c mongoose.c cJSON.c
TARGET = product_server

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LIBS)

clean:
	rm -f $(TARGET) products.db

.PHONY: clean

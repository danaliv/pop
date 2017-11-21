CFLAGS += -std=c99 -I../../include
LDFLAGS += -shared

OS = $(shell uname -s)

ifeq ($(shell uname -s), Darwin)
	LDFLAGS += -undefined dynamic_lookup
	TARGET = ext.dylib
else
	CFLAGS += -fPIC
	TARGET = ext.so
endif

.PHONY: clean

$(TARGET): ext.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) ext.o

clean:
	rm -f $(TARGET) ext.o

CFLAGS += -I../../include
LDFLAGS += -shared -undefined dynamic_lookup

ifeq ($(shell uname -s), Darwin)
	TARGET = ext.dylib
else
	TARGET = ext.so
endif

.PHONY: clean

$(TARGET): ext.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) ext.o

clean:
	rm -f $(TARGET) ext.o

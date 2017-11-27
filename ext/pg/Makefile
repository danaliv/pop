CFLAGS += -std=c99 -I../../include -I$(shell pg_config --includedir)
LDFLAGS += -shared -L$(shell pg_config --libdir)
LIBS += -lpq

ifeq ($(shell uname -s), Darwin)
	LDFLAGS += -undefined dynamic_lookup
	TARGET = pg.dylib
else
	CFLAGS += -fPIC
	TARGET = pg.so
endif

.PHONY: clean

$(TARGET): pg.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) pg.o $(LIBS)

clean:
	rm -f $(TARGET) *.o

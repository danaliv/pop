CFLAGS += -std=c99 -I../../include -I/usr/local/opt/openssl/include
LDFLAGS += -shared
LIBS += -lcrypto

ifeq ($(shell uname -s), Darwin)
	LDFLAGS += -undefined dynamic_lookup
	TARGET = bcrypt.dylib
else
	CFLAGS += -fPIC
	TARGET = bcrypt.so
endif

.PHONY: clean

objs = bcrypt.o crypt_blowfish.o crypt_gensalt.o wrapper.o

$(TARGET): $(objs)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(objs) $(LIBS)

clean:
	rm -f $(TARGET) *.o

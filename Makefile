CFLAGS=-g -Wall -I/usr/include/libxml2 -DLINUX -D_GNU_SOURCE=1 -DDEBUG
LDFLAGS=-lcrypto -lcurl -lssl -lxml2 -lbsd

OBJS=s3string.o s3digest.o s3ops.o s3xml.o s3bucket.o

all: s3test s3datatest

s3test: $(OBJS) s3test.o
	$(CC) -o $@ s3test.o $(OBJS) $(LDFLAGS)

s3datatest: $(OBJS) s3datatest.o
	$(CC) -o $@ s3datatest.o $(OBJS) $(LDFLAGS)

valgrind: s3test
	valgrind --leak-check=full ./s3test

clean:
	rm -f $(OBJS) s3test s3datatest s3test.o s3datatest.o

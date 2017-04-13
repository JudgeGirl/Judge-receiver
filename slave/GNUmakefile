CC=gcc
CFLAGS=-std=c99 -O2 -Wall
RM=rm -f
TARGETS=sandbox sandbox-ex tiger

.PHONY:	all clean

all: $(TARGETS)

sandbox: ./src/sandbox.c ./include/config.h
	$(CC) $(CFLAGS) -Iinclude -o $@ $< -static -lrt
	sudo chown root:root sandbox
	sudo chmod +s sandbox

sandbox-ex: ./src/sandbox-ex.c ./include/config.h
	$(CC) $(CFLAGS) -Iinclude -o $@ $< -static -lrt
	sudo chown root:root $@
	sudo chmod +s $@

tiger: ./src/tiger.c ./include/config.h
	$(CC) $(CFLAGS) -Iinclude -o $@ $< -static -s

clean:
	$(RM) $(TARGETS)

hello: ./src/hello.c sandbox
	gcc ./src/hello.c -o hello
	cd ../scripts && sudo ./prepare && sudo ./mount
	sudo cp hello /sandbox/sandbox/app
	./sandbox 10 16777216 ../app/hello

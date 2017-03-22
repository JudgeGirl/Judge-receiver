CC=gcc
CFLAGS=-std=c99 -O2
RM=rm -f
.PHONY:	all clean
all:	sandbox tiger
sandbox:	sandbox.c config.h
	$(CC) $(CFLAGS) -o $@ $< -lrt
	sudo chown root:root sandbox
	sudo chmod +s sandbox
tiger:	tiger.c config.h
	$(CC) $(CFLAGS) -o $@ $< -static -s
clean:
	$(RM) sandbox tiger hello

test: hello.c sandbox
	gcc hello.c -o hello
	cd ../scripts && sudo ./prepare && sudo ./mount
	sudo cp hello /sandbox/sandbox/app
	./sandbox 10 16777216 ../app/hello

.PHONY: all info clean

# Use gcc as default compiler and linker
# May be changed by passing arguments to make
ifeq ($(origin CC), default)
    CC=gcc
endif
ifeq ($(origin LD), default)
    LD=gcc
endif


all: ringbuffer.o

test: ringbuffer.o
	@echo "\033[01;32m=> Compiling and linking test application ...\033[00;00m"
	$(CC) ringbuffer.o test.c -o $@
	@echo ""

ringbuffer.o: ringbuffer.c ringbuffer.h
	@echo "\033[01;32m=> Compiling '$<' ...\033[00;00m"
	$(CC) -c $(CFLAGS) ringbuffer.c -o $@
	@echo ""

info:
	@echo "Compiler is \"$(CC)\" defined by $(origin CC)"
	@echo "Linker is \"$(LD)\" defined by $(origin LD)"
	@echo ""

clean:
	rm -f ringbuffer.o
	rm -f test




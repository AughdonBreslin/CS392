CC     = gcc
CFLAGS = -g -Wall -Werror -pedantic-errors

all: dl_demo my_dl.so my_dl2.so
dl_demo: dl_demo.c
	$(CC) $(CFLAGS) -o dl_demo dl_demo.c -ldl
%.so: %.c
	$(CC) $(CFLAGS) -shared -fPIC -o $@ $<
clean:
	rm -f *.so dl_demo dl_demo.exe

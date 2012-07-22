#include <ulib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <file.h>
#include <dir.h>
#include <unistd.h>

int
main(int argc, char *argv[]) {
    int i;
    cprintf("Hello world!!.\n");
    cprintf("I am process %d.\n", getpid());
    cprintf("ARG: %d:\n", argc);
    for(i=0;i<argc;i++)
      cprintf("ARG %d: %s\n", i, argv[i]);

    int fd = -1;
    int test = 0x12345;
    fd = open("hzfchar:", O_RDWR);
    cprintf("fd = %d\n", fd);
    test = write(fd, &test, sizeof(int));
    cprintf("ret = %d\n", test);

    cprintf("hello pass.\n");
    return 0;
}


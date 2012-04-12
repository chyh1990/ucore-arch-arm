#include <stdio.h>
#include <ulib.h>

int
main(int argc, char *argv[]) {
    int i;
    cprintf("Hello world!!.\n");
    cprintf("I am process %d.\n", getpid());
    cprintf("ARG: %d:\n", argc);
    for(i=0;i<argc;i++)
      cprintf("ARG %d: %s\n", i, argv[i]);
    cprintf("hello pass.\n");
    return 0;
}


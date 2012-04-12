#include <stdio.h>
#include <kio.h>
#include <string.h>
#include <trap.h>
#include <monitor.h>
#include <kdebug.h>

/* *
 * Simple command-line kernel monitor useful for controlling the
 * kernel and exploring the system interactively.
 * */

struct command {
    const char *name;
    const char *desc;
    // return -1 to force monitor to exit
    int(*func)(int argc, char **argv, struct trapframe *tf);
};

static struct command commands[] = {
    {"help", "Display this list of commands.", mon_help},
    {"kerninfo", "Display information about the kernel.", mon_kerninfo},
    {"backtrace", "Print backtrace of stack frame.", mon_backtrace},
    {"dump", "Dump memory.", mon_dump},
};

#define NCOMMANDS (sizeof(commands)/sizeof(struct command))

/***** Kernel monitor command interpreter *****/

#define MAXARGS         16
#define WHITESPACE      " \t\n\r"

/* parse - parse the command buffer into whitespace-separated arguments */
static int
parse(char *buf, char **argv) {
    int argc = 0;
    while (1) {
        // find global whitespace
        while (*buf != '\0' && strchr(WHITESPACE, *buf) != NULL) {
            *buf ++ = '\0';
        }
        if (*buf == '\0') {
            break;
        }

        // save and scan past next arg
        if (argc == MAXARGS - 1) {
            kprintf("Too many arguments (max %d).\n", MAXARGS);
        }
        argv[argc ++] = buf;
        while (*buf != '\0' && strchr(WHITESPACE, *buf) == NULL) {
            buf ++;
        }
    }
    return argc;
}

/* *
 * runcmd - parse the input string, split it into separated arguments
 * and then lookup and invoke some related commands/
 * */
static int
runcmd(char *buf, struct trapframe *tf) {
    char *argv[MAXARGS];
    int argc = parse(buf, argv);
    if (argc == 0) {
        return 0;
    }
    int i;
    for (i = 0; i < NCOMMANDS; i ++) {
        if (strcmp(commands[i].name, argv[0]) == 0) {
            return commands[i].func(argc - 1, argv + 1, tf);
        }
    }
    kprintf("Unknown command '%s'\n", argv[0]);
    return 0;
}

/***** Implementations of basic kernel monitor commands *****/

void
monitor(struct trapframe *tf) {
    kprintf("Welcome to the kernel debug monitor!!\n");
    kprintf("Type 'help' for a list of commands.\n");

    if (tf != NULL) {
        print_trapframe(tf);
    }

    char *buf;
    while (1) {
        if ((buf = readline("K> ")) != NULL) {
            if (runcmd(buf, tf) < 0) {
                break;
            }
        }
    }
}

/* mon_help - print the information about mon_* functions */
int
mon_help(int argc, char **argv, struct trapframe *tf) {
    int i;
    for (i = 0; i < NCOMMANDS; i ++) {
        kprintf("%s - %s\n", commands[i].name, commands[i].desc);
    }
    return 0;
}

/* *
 * mon_kerninfo - call print_kerninfo in kern/debug/kdebug.c to
 * print the memory occupancy in kernel.
 * */
int
mon_kerninfo(int argc, char **argv, struct trapframe *tf) {
    print_kerninfo();
    return 0;
}

/* *
 * mon_backtrace - call print_stackframe in kern/debug/kdebug.c to
 * print a backtrace of the stack.
 * */
int
mon_backtrace(int argc, char **argv, struct trapframe *tf) {
    print_stackframe();
    return 0;
}

static int atoi16(const char* s)
{
  int res = 0;
  while(*s){
    if( (*s)>='0' && (*s)<='9')
      res = (res<<4)+(*s++)-'0';
    else if( (*s)>='A' && (*s)<='F')
      res = (res<<4)+(*s++)-'A'+10;
    else if( (*s)>='a' && (*s)<='f')
      res = (res<<4)+(*s++)-'a'+10;
    else 
      s++; //skip
  }
  return res;
}

int mon_dump(int argc, char **argv, struct trapframe *tf){
  if(argc==0){
    kprintf("usage: dump [start addr] [end addr] \n"
            "       (addr in hex without '0x')\n");
    return 0;
  }
  unsigned int start = atoi16(argv[0]);
  unsigned int end = start+32;
  if(argc>=2){
    end = atoi16(argv[1]);
    if(end<start)
      end = start + 32;
  }
    
  kprintf("DUMP 0x%08x:\n", start);
  for(;start<end;start+=4){
    kprintf("%08x\t0x%08x\n",start, *(uint32_t*)start);
  }
  return 0;
}


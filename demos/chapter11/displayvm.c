/*****************************************************************************
  Description/   : Displays virtual memory addresses of a running program.
  Purpose        : To show where various types of symbols are in virtual mem
  Usage          : displayvm
  Build with     : gcc -Wall -g -o displayvm -I../include  displayvm.c

Notes.
To make this more interesting, compile it a second time with PIE disabled:
   gcc -no-pie -fno-pie -O0 -o displayvm_no_pie  -I../include displayvm.c
and run this executable. You'll discover radically different virtual
addresses. Not only are they differrent but they will not be randomized
from one run to the next.
*****************************************************************************/


/*



*/


#include "common_hdrs.h"
typedef unsigned long long ull;

/* These are system variables, defined in the unistd.h header file */
extern int  etext, edata, end;  // because its definition, not declaration

/* A type definition - only used by the compiler and is not in memory. */
typedef struct {
    ull  loc;
    char name[16];
} symbol;


/* File scoped symbols */
char  *title = "Layout of virtual memory\n";  /* Initialized data           */
char   string[256];                           /* Uninitialized  data        */

void sort(symbol addresses[], int count)                    /* Text segment */
{
    int i,j;
    symbol temp;
    for ( i = 0; i < count; i++ ) {
        ull max = addresses[i].loc;
        int maxpos = i;
        for ( j = i+1; j < count; j++ )
            if ( addresses[j].loc > max ) {
                max = addresses[j].loc;
                maxpos = j;
            }
        temp = addresses[maxpos];
        addresses[maxpos] = addresses[i];
        addresses[i] = temp;
    }
}

// all local variable will be kept in stack
int main(int argc, char* argv[], char* envp[])              /* Text segment */
{
    int  i;                                               /* Stack variable */
    static long  diff;              /* Global in uninitialized data segment */
    void *origbreak = sbrk(0);
    char* inheap = (char *) malloc(4096);   /* In heap */
    int  num_symbols;
    void *progbreak = sbrk(0);

    symbol addresses[] = {
       {(ull) &main, "  main"},
       {(ull) &sort, "  sort"},
       {(ull) &strcpy, "  strcpy"},
       {(ull) &printf, "  printf"},
       {(ull) &write, "  write"},
       {(ull) &etext, "etext"},
       {(ull) &title, "  title"},
       {(ull) inheap, "  *inheap"},
       {(ull) &string, "  string"},
       {(ull) &diff, "  diff"},
       {(ull) &edata, "edata"},
       {(ull) &end,  "end"},
       {(ull) &argc, "  argc"},
       {(ull) &(argv[0]), "  argv"},
       {(ull) &(envp[0]), "  envp"},
       {(ull) progbreak, "progbreak"},
       {(ull) origbreak, "origbreak"},
       {(ull) &i, "  i"} };

    num_symbols = sizeof(addresses)/sizeof(addresses[0]);
    strcpy(string, title);
    write(1,string,strlen(string)+1);
    sort(addresses, num_symbols);
    printf("ID                          HEX_ADDR         DECIMAL_ADDR\n");
    for ( i = 0; i < num_symbols ; i++ )
        printf("%-10s   is at addr:%16llX%20llu\n", addresses[i].name,
                   addresses[i].loc, addresses[i].loc);
    return 0;
}
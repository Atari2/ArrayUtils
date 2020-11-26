## ArrayUtils

A simple library for array of fixed size elements in C.

* Documentation can be found at this webpage http://www.atarismwc.com/arrayutils/
* A navigable PDF can also be downloaded from : http://www.atarismwc.com/arrayutilsdocs.pdf

Minimal example usage:
```c
#include "ArrayUtils.h"

void custom_handler(int _) {
    (void)_;
    printf("Basically this just ignores thrown exceptions\n");
}

int main() {
    set_trace_lvl_arrayutils(Warn);
    override_signal_exception_arrayutils(custom_handler);
    Vector* v = VECTOR(int);
    ADD_VAL(int, 1, v)
    ADD_VAL(int, 2, v)
    ADD_VAL(int, 3, v)
    print_vect(v, "[%d] ");
    reverse(v);
    printf("\n");
    print_vect(v, "[%d] ");
    printf("\n");
    return 0;
}

```
Valgrind output of this little program:
```
 valgrind --leak-check=full --show-leak-kinds=all ./array
==1445== Memcheck, a memory error detector
==1445== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==1445== Using Valgrind-3.15.0 and LibVEX; rerun with -h for copyright info
==1445== Command: ./array
==1445==
[1] [2] [3]
[3] [2] [1]
==1445==
==1445== HEAP SUMMARY:
==1445==     in use at exit: 0 bytes in 0 blocks
==1445==   total heap usage: 7 allocs, 7 frees, 1,108 bytes allocated
==1445==
==1445== All heap blocks were freed -- no leaks are possible
==1445==
==1445== For lists of detected and suppressed errors, rerun with: -s
==1445== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
Compilation with GCC is highly recommended.

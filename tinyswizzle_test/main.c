//
//  main.c
//  tinyswizzle_test
//
//  Created by Silas Schwarz on 7/24/15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "TinySwizzle.h"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[0m\x1B[31m"
#define KLRD  "\x1B[0m\x1B[31;1m"
#define KGRN  "\x1B[0m\x1B[32m"
#define KLGN  "\x1B[0m\x1B[32;1m"
#define KYEL  "\x1B[0m\x1B[33m"
#define KLYL  "\x1B[0m\x1B[33;1m"
#define KBLU  "\x1B[0m\x1B[34m"
#define KLBL  "\x1B[0m\x1B[34;1m"
#define KMAG  "\x1B[0m\x1B[35m"
#define KLMG  "\x1B[0m\x1B[35;1m"
#define KCYN  "\x1B[0m\x1B[36m"
#define KLCN  "\x1B[0m\x1B[36;1m"
#define KWHT  "\x1B[0m\x1B[37m"
#define KLWT  "\x1B[0m\x1B[37;1m"

extern uint32_t _tinyswizzle_arm64_trampoline;

#define my_printf(...) printf(__VA_ARGS__); fflush(stdout)

#define MOV_PC(num) (   0xe300f000 | ( \
num & 0xfff | \
((num << 4) & 0xf0000) ))

_Static_assert(MOV_PC(0x8888) == 0xe308f888, "hi");

void my_function(void);
void hook_function(void);

int get_two(void);
int get_two() {
    return 2;
}

void my_function() {
    int a = 1 + 2;
    a *= 3;
    a += get_two();
    my_printf("Hello, World!\n");
}

TinySwizzle(void, my_function) {
    my_printf(KRED "Hahahaha\n" KYEL);
    _my_function();
    my_printf(KNRM);
}

int my_global = 0x12345678;

int main (int argc, const char * argv[]) {
    my_function();
    ts_return_t ret = TinySwizzleFunction(&my_function, TinySwizzled(my_function));
    if (ret) {
        my_printf("%d\n", ret);
    }
    my_function();
	return 0;
}

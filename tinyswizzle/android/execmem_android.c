//
//  execmem_android.c
//  tinyswizzle
//
//  Created by Silas Schwarz on 7/24/15.
//
//

#include "execmem_android.h"

#if defined(TS_OS_ANDROID)

#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

int manual_mprotect(void * a, size_t n, int p) {
#ifdef __arm__
    __asm__("mov R7, #0x7d");
    __asm__("svc 0x00");
    __asm__("bxpl LR");
#else
    return mprotect(a, n, p);
#endif
}

ts_return_t execmem_alloc(void** return_ptr, uintptr_t hint) {
    *return_ptr = mmap((void*)hint, sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    if (*return_ptr == MAP_FAILED) {
        return TS_ERR_VM;
    }
    return TS_ERR_OK;
}

ts_return_t execmem_seal(void* page) {
    if (manual_mprotect(page, sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_EXEC)) {
        return TS_ERR_VM;
    }
    return TS_ERR_OK;
}

void execmem_free(void* page) {
    munmap(page, sysconf(_SC_PAGE_SIZE));
}

#define TS_PAGE(ptr) ((void *)((uintptr_t)(ptr) & ~sysconf(_SC_PAGE_SIZE)))

ts_return_t execmem_write(void* destination, const void* source, size_t length) {
    void *page = TS_PAGE(destination);
    uintptr_t prot_len = sysconf(_SC_PAGE_SIZE);
    if (TS_PAGE(destination + length) != page) {
        prot_len *= 2;
    }
    if (manual_mprotect(page, prot_len, PROT_READ | PROT_WRITE)) {
        return TS_ERR_MEM;
    }
    memcpy(destination, source, length);
    if (manual_mprotect(page, prot_len, PROT_READ | PROT_EXEC)) {
        return TS_ERR_MEM;
    }
    return TS_ERR_OK;
}

#endif /* defined(TS_OS_ANDROID) */

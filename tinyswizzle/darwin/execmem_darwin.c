//
//  execmem_darwin.c
//  tinyswizzle
//
//  Created by Silas Schwarz on 7/24/15.
//
//

#include "execmem_darwin.h"

#if defined(TS_OS_IOS)

#include <mach/mach.h>
#include <sys/mman.h>
#include <sys/syscall.h>

#include "substitute_execmem.h"

ts_return_t execmem_alloc(void** return_ptr, uintptr_t hint) {
    *return_ptr = mmap((void*)hint, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    if (*return_ptr == MAP_FAILED) {
        return TS_ERR_VM;
    }
    return TS_ERR_OK;
}

ts_return_t execmem_seal(void* page) {
    if (mprotect(page, PAGE_SIZE, PROT_READ | PROT_EXEC)) {
        return TS_ERR_VM;
    }
    return TS_ERR_OK;
}

ts_return_t execmem_write(void* destination, const void* source, size_t length) {
    struct execmem_foreign_write write = { destination, source, length };
    int ret = execmem_foreign_write_with_pc_patch(&write, 1, NULL, NULL);
    if (ret) {
        return TS_ERR_VM;
    }
    return TS_ERR_OK;
}

#endif /* defined(TS_OS_IOS) */

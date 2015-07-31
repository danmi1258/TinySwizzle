//
//  swizzle_c_arm64.c
//  tinyswizzle
//
//  Created by Silas Schwarz on 7/24/15.
//
//

#include "swizzle_c_arm64.h"
#include "execmem.h"

#include <string.h>

#if __arm64__

extern uint32_t _tinyswizzle_arm64_trampoline;
#define B_NUM _tinyswizzle_arm64_trampoline

extern uint64_t _tinyswizzle_arm64_trampoline_long;

#define B_NUM_MAX ((1 << 27) - 1)
#define B_NUM_MIN (-(1 << 27))
#define B_NUM_MASK 0xFFFFFFF

struct arm64_trampoline {
    union {
        uint32_t b;
        uint32_t address;
    };
};

struct arm64_trampoline_long {
    uint64_t ldr_b;
    uint64_t address;
};

_Static_assert(sizeof(struct arm64_trampoline_long) == 16, "");

ts_return_t TinySwizzleFunction(TSFunction function, TSFunction replacement, TSFunction* original_ptr) {
    uintptr_t target = (uintptr_t)function;
    if (target & 1) {
        return TS_ERR_UNK;
    }
    
    if (original_ptr) {
        *original_ptr = NULL;
        
        /* Rewrite beginning of target function */
        void* page;
        ts_return_t ret = execmem_alloc(&page, target);
        if (ret) {
            return ret;
        }
        uintptr_t page_int = (uintptr_t)page;
        
        ptrdiff_t diff = (ptrdiff_t)target - (ptrdiff_t)page_int;
        if (diff > B_NUM_MAX || diff < B_NUM_MIN) {
            memcpy(page, target, sizeof(struct arm64_trampoline_long));
            
            /* Make trampoline for the rest of the target function */
            struct arm64_trampoline_long* tramp = (struct arm64_trampoline_long*)(page_int + sizeof(struct arm64_trampoline_long));
            tramp->ldr_b = _tinyswizzle_arm64_trampoline_long;
            tramp->address = (uintptr_t)target + sizeof(struct arm64_trampoline_long);
        } else {
            memcpy(page, target, sizeof(struct arm64_trampoline));
            
            /* Make trampoline for the rest of the target function */
            struct arm64_trampoline* tramp = (struct arm64_trampoline*)(page_int + sizeof(struct arm64_trampoline));
            tramp->b = B_NUM;
            tramp->address |= (B_NUM_MASK & diff) >> 2;
        }
        
        /* It has to be executable */
        ret = execmem_seal(page);
        if (ret) {
            return ret;
        }
        
        /* Give them the newly made original */
        *original_ptr = page;
    }
    ptrdiff_t diff = (ptrdiff_t)replacement - (ptrdiff_t)target;
    if (diff > B_NUM_MAX || diff < B_NUM_MIN) {
        /* Make trampoline hook to overwrite target function */
        struct arm64_trampoline_long hook_tramp_long;
        hook_tramp_long.ldr_b = _tinyswizzle_arm64_trampoline_long;
        hook_tramp_long.address = (uintptr_t)replacement;
        
        /* Commit swizzle */
        return execmem_write(target, &hook_tramp_long, sizeof(hook_tramp_long));
    } else {
        /* Make trampoline hook to overwrite target function */
        struct arm64_trampoline hook_tramp;
        hook_tramp.b = B_NUM;
        hook_tramp.address |= (B_NUM_MASK & diff) >> 2;
        
        /* Commit swizzle */
        return execmem_write(target, &hook_tramp, sizeof(hook_tramp));
    }
}

#endif /* __arm64__ */

//
//  swizzle_c_arm.c
//  tinyswizzle
//
//  Created by Silas Schwarz on 7/24/15.
//
//

#include "swizzle_c_arm.h"
#include "execmem.h"

#include <string.h>

#if __arm__

extern uint32_t _tinyswizzle_arm_trampoline;
extern uint32_t _tinyswizzle_thumb_trampoline;

#define LDR_PC(thumb) (thumb ? _tinyswizzle_thumb_trampoline : _tinyswizzle_arm_trampoline)

struct armv7_trampoline {
    uint32_t ldr_pc;
    uint32_t address;
};

ts_return_t TinySwizzleFunction(TSFunction function, TSFunction replacement, TSFunction* original_ptr) {
    uintptr_t target = (uintptr_t)function;
    bool thumb = target & 1;
    target &= ~1;
    
    if (original_ptr) {
        *original_ptr = NULL;
        
        /* Rewrite beginning of target function */
        void* page;
        ts_return_t ret = execmem_alloc(&page, target);
        if (ret) {
            return ret;
        }
        memcpy(page, target, 8);
        
        /* Make trampoline for the rest of the target function */
        struct armv7_trampoline* tramp = (struct armv7_trampoline*)((uintptr_t)page + 8);
        tramp->ldr_pc = LDR_PC(thumb);
        tramp->address = (uint32_t)(target + (thumb ? 9 : 8));
        
        /* It has to be executable */
        ret = execmem_seal(page);
        if (ret) {
            return ret;
        }
        
        /* Give them the newly made original */
        *original_ptr = page + (thumb ? 1 : 0);
    }
    
    /* Make trampoline hook to overwrite target function */
    struct armv7_trampoline hook_tramp;
    hook_tramp.ldr_pc = LDR_PC(thumb);
    hook_tramp.address = (uint32_t)replacement;
    
    /* Commit swizzle */
    return execmem_write(target, &hook_tramp, sizeof(hook_tramp));
}

#endif /* __arm__ */

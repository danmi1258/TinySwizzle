//
//  trampoline.S
//  tinyswizzle
//
//  Created by Silas Schwarz on 7/24/15.
//
//

#ifdef __arm64__

.align 4
.globl __tinyswizzle_arm64_trampoline
__tinyswizzle_arm64_trampoline:
    b 0

.globl __tinyswizzle_arm64_trampoline_long
__tinyswizzle_arm64_trampoline_long:
    ldr x9, #8
    br x9

#endif

//
//  trampoline.S
//  tinyswizzle
//
//  Created by Silas Schwarz on 7/24/15.
//
//

#ifdef __arm__

.align
.globl __tinyswizzle_arm_trampoline
__tinyswizzle_arm_trampoline:
    .arm
    ldr pc, [pc, #-4]

.globl __tinyswizzle_thumb_trampoline
__tinyswizzle_thumb_trampoline:
    .thumb
    ldr pc, [pc, #0]

#endif
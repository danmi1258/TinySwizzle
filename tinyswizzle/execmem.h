//
//  execmem.h
//  tinyswizzle
//
//  Created by Silas Schwarz on 7/24/15.
//
//

#ifndef __tinyswizzle__execmem__
#define __tinyswizzle__execmem__

#include <stdio.h>

#include "TinySwizzle.h"
#include "os_decls.h"

__BEGIN_DECLS

ts_return_t execmem_alloc(void** return_ptr, uintptr_t hint);
ts_return_t execmem_seal(void* page);
void execmem_free(void* page);

ts_return_t execmem_write(void* destination, const void* source, size_t length);

__END_DECLS

#endif /* defined(__tinyswizzle__execmem__) */

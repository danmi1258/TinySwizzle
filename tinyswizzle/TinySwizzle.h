//
//  TinySwizzle.h
//  tinyswizzle
//
//  Created by Silas Schwarz on 7/24/15.
//
//

#ifndef __tinyswizzle__TinySwizzle__
#define __tinyswizzle__TinySwizzle__

#include <stdio.h>

__BEGIN_DECLS

typedef void* TSFunction;

typedef enum ts_return ts_return_t;
enum ts_return {
    TS_ERR_OK = 0,
    TS_ERR_MEM,
    TS_ERR_VM,
    TS_ERR_NUL,
#if __APPLE__
    TS_ERR_OBJC_TYPE_ENCODING,
    TS_ERR_OBJC_SEL_NOT_FOUND,
    TS_ERR_OBJC_RUNTIME,
#endif /* __APPLE__ */
    TS_ERR_UNK
};

ts_return_t TinySwizzleFunction(TSFunction function, TSFunction replacement, TSFunction* original_ptr);

#define TinySwizzle(type, name, args...) \
__attribute__((__unused__)) static type (*_ ## name)(args); \
static type $ ## name(args)

#define TinySwizzled(name) &$ ## name, (void **) &_ ## name

#define TinyConstructor \
__attribute__((__constructor__)) static void _TinyConstructor ## __COUNTER__(void)

#if defined(__APPLE__)

#include <objc/runtime.h>

#define TinySwizzleInterface(target, swizzle, base) \
@class target; \
@interface $ ## swizzle : base { target *$self; } @end \
@implementation $ ## swizzle \
+ (void) initialize {} \
@end \
@interface swizzle : $ ## swizzle @end \
@implementation swizzle (TinySwizzleInterface) + (void) load { \
TinySwizzleClasses(objc_getClass(#target), self, class_getSuperClass(self)); \
} @end

ts_return_t TinySwizzleMessage(Class klass, SEL selector, TSFunction replacement, TSFunction* original_ptr);
ts_return_t TinySwizzleClasses(Class klass, Class replacement, Class original);

#endif /* defined(__APPLE__) */

__END_DECLS

#endif /* defined(__tinyswizzle__TinySwizzle__) */

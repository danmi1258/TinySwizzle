//
//  swizzle_objc.c
//  tinyswizzle
//
//  Created by Silas Schwarz on 7/24/15.
//
//

#include "TinySwizzle.h"

#include <stdlib.h>
#include <string.h>

ts_return_t TSReallySwizzleClasses(Class target, Class hook, Class original);
ts_return_t TSSwizzleClassMethods(Class target, Class hook, Class original);
ts_return_t TSSwizzleClassProperties(Class target, Class hook, Class original);
ts_return_t TSSwizzleMethod(Method method, Class target, Class hook, Class original);
ts_return_t TSSwizzleProperty(objc_property_t property, Class target, Class hook, Class original);

ts_return_t TinySwizzleMessage(Class klass, SEL selector, TSFunction replacement, TSFunction* original_ptr) {
    if (klass == NULL | selector == NULL | replacement == NULL) {
        return TS_ERR_NUL;
    }
    Method m;
    if (class_respondsToSelector(klass, selector)) {
        m = class_getInstanceMethod(klass, selector);
    } else if (class_respondsToSelector(object_getClass((id)klass), selector)) {
        m = class_getClassMethod(klass, selector);
    } else {
        return TS_ERR_OBJC_SEL_NOT_FOUND;
    }
    IMP old = method_setImplementation(m, (IMP)replacement);
    *original_ptr = (TSFunction)old;
    return TS_ERR_OK;
}

ts_return_t TinySwizzleClasses(Class target, Class hook, Class original) {
    if (target == NULL || hook == NULL) {
        return TS_ERR_NUL;
    }
    ts_return_t ret = TSReallySwizzleClasses(target, hook, original);
    if (ret) {
        return ret;
    }
    ret = TSReallySwizzleClasses(object_getClass((id)target), object_getClass((id)hook), object_getClass((id)original));
    return ret;
}

ts_return_t TSReallySwizzleClasses(Class target, Class hook, Class original) {
    ts_return_t ret = TSSwizzleClassMethods(target, hook, original);
    if (ret) {
        return ret;
    }
    ret = TSSwizzleClassProperties(target, hook, original);
    return ret;
}

ts_return_t TSSwizzleClassMethods(Class target, Class hook, Class original) {
    ts_return_t ret = TS_ERR_OK;
    unsigned int methodCount;
    Method *methods = class_copyMethodList(hook, &methodCount);
    for (unsigned int i = 0; i < methodCount && !ret; i ++) {
        ret = TSSwizzleMethod(methods[i], target, hook, original);
    }
    free(methods);
    return ret;
}

ts_return_t TSSwizzleClassProperties(Class target, Class hook, Class original) {
    BOOL ret = TS_ERR_OK;
    unsigned int propertyCount;
    objc_property_t *properties = class_copyPropertyList(hook, &propertyCount);
    for (unsigned int i = 0; i < propertyCount && !ret; i ++) {
        ret = TSSwizzleProperty(properties[i], target, hook, original);
    }
    free(properties);
    return ret;
}

ts_return_t TSSwizzleMethod(Method method, Class target, Class hook, Class original) {
    SEL selector = method_getName(method);
    if (class_respondsToSelector(target, selector)) {
        Method originalMethod = class_getInstanceMethod(target, selector);
        IMP originalImplementation = method_getImplementation(originalMethod);
        const char *hookTypeEncoding = method_getTypeEncoding(method);
        const char *originalTypeEncoding = method_getTypeEncoding(originalMethod);
        if (strcmp(hookTypeEncoding, originalTypeEncoding) != 0) {
            return TS_ERR_OBJC_TYPE_ENCODING;
        }
        class_addMethod(target, selector, originalImplementation, originalTypeEncoding);
        if (!class_addMethod(original, selector, originalImplementation, originalTypeEncoding)) {
            return TS_ERR_OBJC_RUNTIME;
        }
        method_setImplementation(class_getInstanceMethod(target, selector), method_getImplementation(method));
        return TS_ERR_OK;
    } else {
        return class_addMethod(target, selector, method_getImplementation(method), method_getTypeEncoding(method));
    }
}

ts_return_t TSSwizzleProperty(objc_property_t property, Class target, Class hook, Class original) {
    ts_return_t ret = TS_ERR_OK;
    const char *propertyName = property_getName(property);
    unsigned int attributeCount;
    objc_property_attribute_t *attributes = property_copyAttributeList(property, &attributeCount);
    if (class_getProperty(target, propertyName) == NULL) {
        if (!class_addProperty(target, propertyName, attributes, attributeCount)) {
            ret = TS_ERR_OBJC_RUNTIME;
        }
    } else {
        class_replaceProperty(target, propertyName, attributes, attributeCount);
    }
    free(attributes);
    return ret;
}

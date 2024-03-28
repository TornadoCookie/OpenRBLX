
#ifdef DEBUG_IMPL
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

const char *debugstr_vector3(Vector3 v)
{
    char *buf = malloc(1024);
    snprintf(buf, 1023, "{%f, %f, %f}", v.x, v.y, v.z);
    buf = realloc(buf, strlen(buf) + 1);
    return buf;
}

const char *debugstr_cframe(CFrame cf)
{
    static char buf[1024];
    snprintf(buf, 1024, "{ XYZ %s, RX0 %s, RX1 %s, RX2 %s }", debugstr_vector3((Vector3){cf.X, cf.Y, cf.Z}), debugstr_vector3((Vector3){cf.R00, cf.R10, cf.R20}), debugstr_vector3((Vector3){cf.R01, cf.R11, cf.R21}), debugstr_vector3((Vector3){cf.R02, cf.R12, cf.R22}));
    return buf;
}

void dbg_printf(const char *type, const char *channel, const char *func, const char *restrict format, ...)
{
    static char buf[1024];
    snprintf(buf, 1024, "%s:%s:%s %s", type, channel, func, format);
    
    va_list args;
    va_start(args, format);
    vfprintf(stderr, buf, args);
    va_end(args);
}

#endif

#ifndef _DEBUG_
#define _DEBUG_

#include <raymath.h>
#include "cframe.h"

const char *debugstr_vector3(Vector3 v);
const char *debugstr_cframe(CFrame cf);
void dbg_printf(const char *type, const char *channel, const char *func, const char *restrict format, ...);

#define FIXME(fmt, ...) dbg_printf("FIXME", __dbg_channel, __func__, fmt, __VA_ARGS__)

#define DEFAULT_DEBUG_CHANNEL(x) static const char *__dbg_channel = #x;

#endif

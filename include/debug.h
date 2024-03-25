
#ifdef DEBUG_IMPL
#include <stdio.h>
#include <stdarg.h>

const char *debugstr_vector3(Vector3 v)
{
    static char buf[1024];
    snprintf(buf, 1024, "{%f, %f, %f}", v.x, v.y, v.z);
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

const char *debugstr_vector3(Vector3 v);
void dbg_printf(const char *type, const char *channel, const char *func, const char *restrict format, ...);

#define FIXME(fmt, ...) dbg_printf("FIXME", __dbg_channel, __func__, fmt, __VA_ARGS__)

#define DEFAULT_DEBUG_CHANNEL(x) static const char *__dbg_channel = #x;

#endif

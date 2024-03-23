#ifndef _DEBUG_
#define _DEBUG_

#include <raymath.h>

#define FIXME(fmt, ...) printf("FIXME:"__FUNC__": "fmt, __VA_ARGS__)

const char *debugstr_vector3(Vector3 v)
{
    const char buf[1024];
    snprintf(buf, 1024, "{%f, %f, %f}", v.x, v.y, v.z);
    return buf;
}

#endif

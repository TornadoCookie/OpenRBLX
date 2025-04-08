#ifndef _FRAME_
#define _FRAME_

#include "guiobject.h"

typedef struct Frame {
    GuiObject guiobject;
} Frame;

Frame *Frame_new(const char *className, Instance *parent);
void serialize_Frame(Frame *frame, SerializeInstance *inst);

#endif


#ifndef _STYLESHEET_
#define _STYLESHEET_

#include "stylebase.h"

typedef struct StyleSheet {
    StyleBase stylebase;
} StyleSheet;

StyleSheet *StyleSheet_new(const char *className, Instance *parent);
void serialize_StyleSheet(StyleSheet *stylesheet, SerializeInstance *inst);

#endif


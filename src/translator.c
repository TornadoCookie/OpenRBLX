#include "translator.h"
#include "debug.h"

DEFAULT_DEBUG_CHANNEL(translator);

Translator *Translator_new(const char *className, Instance *parent)
{
    INSTANCE_STUB_CONSTRUCTOR(Translator, Instance);
}

const char *Translator_FormatByKey(Translator *this, const char *key)
{
    FIXME("key %s\n", key);
    return key;
}

void serialize_Translator(Translator *translator, SerializeInstance *inst)
{
    serialize_Instance(translator, inst);
}


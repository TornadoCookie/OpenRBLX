#include "filetypes.h"
#include "xml.h"
#include <stdio.h>
#include <stdlib.h>

#include "trusspart.h"

static char *xml_easy_string(struct xml_string *str)
{
    char *buf = malloc(xml_string_length(str) + 1);
    xml_string_copy(str, buf, xml_string_length(str));
    buf[xml_string_length(str)] = 0;
    return buf;
}

typedef enum {
    Serialize_bool,
    Serialize_float,
    Serialize_token,
    Serialize_int,
    Serialize_CoordinateFrame,
    Serialize_string,
    Serialize_Vector3,
} XMLSerializationType;

typedef struct XMLSerialization {
    XMLSerializationType type;
    const char *name;
    void *val;
} XMLSerialization;

typedef struct XMLSerializeInstance {
    XMLSerialization *serializations;
    int serializationCount;
} XMLSerializeInstance;

static void _xmlserialize_atomic(XMLSerializeInstance *inst, XMLSerialization serialization)
{
    inst->serializationCount++;
    inst->serializations = realloc(inst->serializations, inst->serializationCount * sizeof(XMLSerialization));
    inst->serializations[inst->serializationCount - 1] = serialization;
}

#define xmlserialize_atomic(type, thing, prop) _xmlserialize_atomic(inst, (XMLSerialization){Serialize_##type, #prop, &thing->prop})

static void xmlserialize_Instance(Instance *instance, XMLSerializeInstance *inst)
{
    xmlserialize_atomic(string, instance, Name);
    xmlserialize_atomic(bool, instance, archivable);
}

static void xmlserialize_PVInstance(PVInstance *pvinstance, XMLSerializeInstance *inst)
{
    xmlserialize_Instance(pvinstance, inst);
}

static void xmlserialize_BasePart(BasePart *basepart, XMLSerializeInstance *inst)
{
    xmlserialize_PVInstance(basepart, inst);

    xmlserialize_atomic(bool, basepart, Anchored);
    xmlserialize_atomic(float, basepart, BackParamA);
    xmlserialize_atomic(float, basepart, BackParamB);
    xmlserialize_atomic(token, basepart, BackSurface);
    xmlserialize_atomic(token, basepart, BackSurfaceInput);
    xmlserialize_atomic(float, basepart, BottomParamA);
    xmlserialize_atomic(float, basepart, BottomParamB);
    xmlserialize_atomic(token, basepart, BottomSurface);
    xmlserialize_atomic(token, basepart, BottomSurfaceInput);
    xmlserialize_atomic(int, basepart, BrickColor);
    xmlserialize_atomic(CoordinateFrame, basepart, CFrame);
    xmlserialize_atomic(bool, basepart, CanCollide);
    xmlserialize_atomic(bool, basepart, DraggingV1);
    xmlserialize_atomic(float, basepart, Elasticity);
    xmlserialize_atomic(float, basepart, Friction);
    xmlserialize_atomic(float, basepart, FrontParamA);
    xmlserialize_atomic(float, basepart, FrontParamB);
    xmlserialize_atomic(token, basepart, FrontSurface);
    xmlserialize_atomic(token, basepart, FrontSurfaceInput);
    xmlserialize_atomic(float, basepart, LeftParamA);
    xmlserialize_atomic(float, basepart, LeftParamB);
    xmlserialize_atomic(token, basepart, LeftSurface);
    xmlserialize_atomic(token, basepart, LeftSurfaceInput);
    xmlserialize_atomic(bool, basepart, Locked);
    xmlserialize_atomic(token, basepart, Material);
    xmlserialize_atomic(float, basepart, Reflectance);
    xmlserialize_atomic(float, basepart, RightParamA);
    xmlserialize_atomic(float, basepart, RightParamB);
    xmlserialize_atomic(token, basepart, RightSurface);
    xmlserialize_atomic(token, basepart, RightSurfaceInput);
    xmlserialize_atomic(Vector3, basepart, RotVelocity);
    xmlserialize_atomic(float, basepart, TopParamA);
    xmlserialize_atomic(float, basepart, TopParamB);
    xmlserialize_atomic(token, basepart, TopSurface);
    xmlserialize_atomic(token, basepart, TopSurfaceInput);
    xmlserialize_atomic(Vector3, basepart, Velocity);
    xmlserialize_atomic(Vector3, basepart, size);
}

static void xmlserialize_TrussPart(Model *mdl, XMLSerializeInstance *inst)
{
    TrussPart *trusspart = TrussPart_new(mdl);

    xmlserialize_BasePart(trusspart, inst);
    xmlserialize_atomic(token, trusspart, Style);

}

static void xmlserialize_coordinateframe(CoordinateFrame *cf, struct xml_node *node)
{
    for (int i = 0; i < xml_node_children(node); i++)
    {
        struct xml_node *child = xml_node_child(node, i);
        char *propName = xml_easy_string(xml_node_attribute_content(child, 0));
        char *prop = xml_easy_string(xml_node_content(child));
        float propI = atof(prop);

        switch (*propName)
        {
            case 'X': cf->X = propI; break;
            case 'Y': cf->Y = propI; break;
            case 'Z': cf->Z = propI; break;
            case 'R':
            {
                int next = atoi(propName + 1);
                switch (next)
                {
                    case 0: cf->R00 = propI; break;
                    case 1: cf->R01 = propI; break;
                    case 2: cf->R02 = propI; break;
                    case 10: cf->R10 = propI; break;
                    case 11: cf->R11 = propI; break;
                    case 12: cf->R12 = propI; break;
                    case 20: cf->R20 = propI; break;
                    case 21: cf->R21 = propI; break;
                    case 22: cf->R22 = propI; break;
                }
            }
        }

        free(propName);
        free(prop);
    }
}

static void xmlserialize_vector3(Vector3 *v, struct xml_node *node)
{
    for (int i = 0; i < xml_node_children(node); i++)
    {
        struct xml_node *child = xml_node_child(node, i);
        char *propName = xml_easy_string(xml_node_attribute_content(child, 0));
        char *prop = xml_easy_string(xml_node_content(child));
        float propI = atof(prop);

        switch (*propName)
        {
            case 'X': v->x = propI; break;
            case 'Y': v->y = propI; break;
            case 'Z': v->z = propI; break;
        }

        free(propName);
        free(prop);
    }
}

static void load_model_part_xml(Model *mdl, struct xml_node *node)
{
    char *className = xml_easy_string(xml_node_attribute_content(node, 0));

    struct xml_node *propertyNode = xml_node_child(node, 0);
    XMLSerializeInstance inst = { 0 };

    if (!strcmp(className, "TrussPart"))
    {
        xmlserialize_TrussPart(mdl, &inst);
    }
    else
    {
        printf("error: no serializer for classname %s.", className);
    }

    for (int i = 0; i < xml_node_children(node); i++)
    {
        struct xml_node *child = xml_node_child(node, i);
        char *propName = xml_easy_string(xml_node_attribute_content(child, 0));
        char *prop = xml_easy_string(xml_node_content(child));
        for (int j = 0; j < inst.serializationCount; j++)
        {
            if (!strcmp(inst.serializations[i].name, propName))
            {
                void *val = inst.serializations[i].val;
                switch (inst.serializations[i].type)
                {
                    case Serialize_bool:
                    {
                        *(bool*)val = !strcmp(prop, "true");
                    } break;
                    case Serialize_float:
                    {
                        *(float*)val = atof(prop);
                    } break;
                    case Serialize_token:
                    case Serialize_int:
                    {
                        *(int*)val = atoi(prop);
                    } break;
                    case Serialize_string:
                    {
                        char *str = malloc(strlen(prop) + 1);
                        memcpy(str, prop, strlen(prop));
                        str[strlen(prop)] = 0;
                        *(char**)val = str;
                    } break;
                    case Serialize_CoordinateFrame:
                    {
                        xmlserialize_coordinateframe(val, child);
                    }
                }
                break;
            }
        }
        free(propName);
        free(prop);
    }

    free(className);
}

Model *LoadModelRBXMX(const char *file)
{
    FILE *f = fopen(file, "r");
    struct xml_document *document = xml_open_document(f);

    struct xml_node *root = xml_document_root(document);
    Model *mdl = Model_new("Model", NULL);

    for (size_t i = 0; i < xml_node_children(root); i++)
    {
        struct xml_node *child = xml_node_child(root, i);
        uint8_t *name = xml_easy_name(child);
        if (!strcmp(name, "Item")) load_model_part_xml(mdl, child);
        free(name);
    }

    xml_document_free(document, true);

    return mdl;
}

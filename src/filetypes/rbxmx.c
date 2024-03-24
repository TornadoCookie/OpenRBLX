#include "filetypes.h"
#include "xml.h"
#include <stdio.h>
#include <stdlib.h>

#include "trusspart.h"
#include "part.h"

#include "debug.h"

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
    Serialize_Color3,
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

    xmlserialize_atomic(token, pvinstance, Controller);
    xmlserialize_atomic(bool, pvinstance, ControllerFlagShown);
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
    xmlserialize_atomic(float, basepart, Transparency);
    xmlserialize_atomic(Vector3, basepart, Velocity);
    xmlserialize_atomic(Vector3, basepart, size);
    xmlserialize_atomic(Color3, basepart, Color);
}

static TrussPart *xmlserialize_TrussPart(XMLSerializeInstance *inst)
{
    TrussPart *trusspart = TrussPart_new(NULL);

    xmlserialize_BasePart(trusspart, inst);
    xmlserialize_atomic(token, trusspart, Style);

    return trusspart;
}

static void xmlserialize_FormFactorPart(FormFactorPart *formfactorpart, XMLSerializeInstance *inst)
{
    xmlserialize_BasePart(formfactorpart, inst);
}

static Part *xmlserialize_Part(XMLSerializeInstance *inst)
{
    Part *part = Part_new(NULL);

    xmlserialize_FormFactorPart(part, inst);
    xmlserialize_atomic(token, part, shape);

    return part;
}

static Model *xmlserialize_Model(Model *model, XMLSerializeInstance *inst)
{
    xmlserialize_PVInstance(model, inst);
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

static void xmlserialize_color3(Color3 *c, struct xml_node *node)
{
    for (int i = 0; i < xml_node_children(node); i++)
    {
        struct xml_node *child = xml_node_child(node, i);
        char *propName = xml_easy_string(xml_node_attribute_content(child, 0));
        char *prop = xml_easy_string(xml_node_content(child));
        float propI = atof(prop);

        switch (*propName)
        {
            case 'R': c->R = propI; break;
            case 'G': c->G = propI; break;
            case 'B': c->B = propI; break;
        }

        free(propName);
        free(prop);
    }
}

static const char *tokenPropNames[5] = {
    "shape",
    "Controller",
    "Type",
    "Constraint",
    "SurfaceInput",
};

static const char *tokenTables[][50] = {
    { "Ball", "Cylinder", "Block", "Wedge", "CornerWedge", NULL }, // shape
    { "None", "Player", "KeyboardLeft", "KeyboardRight", "Joypad1", "Joypad2", "Chase", "Flee", NULL }, //Controller
    { "Smooth", "Bumps", "Spawn", NULL }, // Type
    { "None", "Hinge", "Motor", "SteppingMotor", NULL }, //Constraint
    { "LeftTread", "RightTread", "Steer", "Throttle", "Updown", "Action1", "Action2", "Action3", "Action4", "Action5", "Sin", "Constant", NULL }, // SurfaceInput
};

static void xmlserialize_token(int *val, char *prop, char *propName)
{
    int size = sizeof(tokenPropNames) / sizeof(char *);

    *val = atoi(prop);

    if (strstr(propName, "SurfaceInput"))
    {
        propName = "SurfaceInput";
    }
    else if (strstr(propName, "Surface"))
    {
        propName = "Type";
    }

    int index = -1;
    for (int i = 0; i < size; i++)
    {
        if (!strcmp(propName, tokenPropNames[i])) index = i;
    }

    for (int i = 0; ; i++)
    {
        if (tokenTables[index][i] == NULL) break;
        if (!strcmp(tokenTables[index][i], prop)) *val = i;
    }
}

static void serialize(XMLSerializeInstance inst, char *prop, char *propName, struct xml_node *child)
{
    bool done = false;

    char *type = xml_easy_string(xml_node_name(child));
    if (!strcmp(type, "Complex"))
    {
        char type_[128], surfaceInput[128], paramA[128], paramB[128];
        snprintf(type_, 128, "%sSurface", propName);
        snprintf(surfaceInput, 128, "%sSurfaceInput", propName);
        snprintf(paramA, 128, "%sParamA", propName);
        snprintf(paramB, 128, "%sParamB", propName);

        for (int i = 0; i < xml_node_children(child); i++)
        {
            struct xml_node *child2 = xml_node_child(child, i);
            serialize(inst, prop, type_, child2);
            serialize(inst, prop, surfaceInput, child2);
            serialize(inst, prop, paramA, child2);
            serialize(inst, prop, paramB, child2);
        }

        FIXME("no analogue for %s types\n", "Constraint");
        free(type);
        return;
    }
    free(type);

    for (int j = 0; j < inst.serializationCount; j++)
    {
        if (!strcmp(inst.serializations[j].name, propName) ||
            (!strcmp(inst.serializations[j].name, "CFrame") && !strcmp(propName, "CoordinateFrame")) ||
            (!strcmp(inst.serializations[j].name, "RotVelocity") && !strcmp(propName, "RotVel")) ||
            (!strcmp(inst.serializations[j].name, "Locked") && !strcmp(propName, "CanSelect")) ||
            (!strcmp(inst.serializations[j].name, "ClassName") && !strcmp(propName, "Keywords")))
        {
            done = true;
            void *val = inst.serializations[j].val;
            switch (inst.serializations[j].type)
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
                {
                    xmlserialize_token(val, prop, propName);
                } break;
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
                } break;
                case Serialize_Color3:
                {
                    xmlserialize_color3(val, child);
                } break;
            }
            break;
        }
    }
    if (!done)
    {
        printf("warning: property %s not serialized. value: %s.\n", propName, prop);
    }
}

static Instance *load_model_part_xml(struct xml_node *node)
{
    char *className = xml_easy_string(xml_node_attribute_content(node, 0));

    struct xml_node *propertyNode = xml_node_child(node, 0);
    XMLSerializeInstance inst = { 0 };
    Instance *ret;

    if (!strcmp(className, "TrussPart"))
    {
        ret = xmlserialize_TrussPart(&inst);
    }
    else if (!strcmp(className, "Part"))
    {
        ret = xmlserialize_Part(&inst);
    }
    else if (!strcmp(className, "Model"))
    {
        ret = Model_new("Model", NULL);
        xmlserialize_Model(ret, &inst);
    }
    else
    {
        printf("error: no serializer for classname %s.\n", className);
    }

    for (int i = 0; i < xml_node_children(propertyNode); i++)
    {
        struct xml_node *child = xml_node_child(propertyNode, i);

        char *type = xml_easy_string(xml_node_name(child));
        if (!strcmp(type, "Feature"))
        {
            char *pName = xml_easy_string(xml_node_attribute_content(child, 0));
            if (!strcmp(pName, "Card"))
            {
                FIXME("No support for %s Feature\n", "Card");
                free(pName);
                continue;
            }
            free(pName);
            for (int j = 0; j < xml_node_children(child); j++)
            {
                struct xml_node *child2 = xml_node_child(child, j);
                char *propName = xml_easy_string(xml_node_attribute_content(child2, 0));
                char *prop = xml_easy_string(xml_node_content(child2));
                serialize(inst, prop, propName, child2);
                free(propName);
                free(prop);
            }
        }
        else if (!strcmp(type, "Item"))
        {
            Instance *new = load_model_part_xml(child);
            Instance_SetParent(new, ret);
        }
        else
        {
            char *propName = xml_easy_string(xml_node_attribute_content(child, 0));
            char *prop = xml_easy_string(xml_node_content(child));
            serialize(inst, prop, propName, child);
            free(propName);
            free(prop);
        }
        free(type);
    }

    free(className);

    return ret;
}

Model *LoadModelRBXMX(const char *file)
{
    FILE *f = fopen(file, "r");
    struct xml_document *document = xml_open_document(f);

    struct xml_node *root = xml_document_root(document);

    Instance *ret;

    for (size_t i = 0; i < xml_node_children(root); i++)
    {
        struct xml_node *child = xml_node_child(root, i);
        uint8_t *name = xml_easy_name(child);
        if (!strcmp(name, "Item")) ret = load_model_part_xml(child);
        free(name);
    }

    xml_document_free(document, true);

    return ret;
}

#include "filetypes.h"
#include "xml.h"
#include <stdio.h>
#include <stdlib.h>

#include "part.h"
#include "trusspart.h"
#include "model.h"
#include "physicalcharacter.h"
#include "workspace.h"
#include "rootinstance.h"
#include "datamodel.h"
#include "camera.h"
#include "serialize.h"

#include "debug.h"

DEFAULT_DEBUG_CHANNEL(rbxmx)

static char *xml_easy_string(struct xml_string *str)
{
    char *buf = malloc(xml_string_length(str) + 1);
    xml_string_copy(str, buf, xml_string_length(str));
    buf[xml_string_length(str)] = 0;
    return buf;
}
/*
typedef enum {
    Serialize_bool,
    Serialize_float,
    Serialize_token,
    Serialize_int,
    Serialize_CoordinateFrame,
    Serialize_string,
    Serialize_Vector3,
    Serialize_Color3,
    Serialize_Ref,
    Serialize_double,
} SerializationType;

typedef struct Serialization {
    SerializationType type;
    const char *name;
    void *val;
} Serialization;

typedef struct SerializeInstance {
    Serialization *serializations;
    int serializationCount;
    CFrame modelOffset;
    uint32_t Color3uint8;
    int TopConstraint, BottomConstraint, LeftConstraint, RightConstraint, FrontConstraint, BackConstraint;
} SerializeInstance;*/

typedef struct XMLRef {
    const char *ref;
    Instance **val;
} XMLRef;

typedef struct XMLRefsInstance {
    int refCount;
    XMLRef *refs;
} XMLRefsInstance;
/*
static void _serialize_atomic(SerializeInstance *inst, Serialization serialization)
{
    inst->serializationCount++;
    inst->serializations = realloc(inst->serializations, inst->serializationCount * sizeof(Serialization));
    inst->serializations[inst->serializationCount - 1] = serialization;
}

#define serialize_atomic(type, thing, prop) _serialize_atomic(inst, (Serialization){Serialize_##type, #prop, &thing->prop})
*/

void serialize_TrussPart(TrussPart *trusspart, SerializeInstance *inst)
{
    //TrussPart *trusspart = TrussPart_new(NULL);

    serialize_BasePart(trusspart, inst);
    serialize_atomic(token, trusspart, Style);
}

void serialize_PhysicalCharacter(PhysicalCharacter *physicalcharacter, SerializeInstance *inst)
{
    serialize_Model(physicalcharacter, inst);

    serialize_atomic(token, physicalcharacter, PostureXML);
    serialize_atomic(Ref, physicalcharacter, Head);
    serialize_atomic(Ref, physicalcharacter, UpperBody);
    serialize_atomic(Ref, physicalcharacter, LowerBody);
    serialize_atomic(Ref, physicalcharacter, LeftLeg);
    serialize_atomic(Ref, physicalcharacter, RightLeg);
    serialize_atomic(Ref, physicalcharacter, LeftArm);
    serialize_atomic(Ref, physicalcharacter, RightArm);
}

void serialize_RootInstance(RootInstance *rootinstance, SerializeInstance *inst)
{
    serialize_Model(rootinstance, inst);
}

void serialize_Workspace(Workspace *workspace, SerializeInstance *inst)
{
    workspace = GetDataModel()->Workspace;
    serialize_Instance(workspace, inst);

    workspace->rootinstance.model.pvinstance.instance.ClassName = "Workspace";

    serialize_atomic(double, workspace, DistributedGameTime);
    serialize_atomic(Ref, workspace, CurrentCamera);
}

void serialize_Camera(Camera_Instance *camera, SerializeInstance *inst)
{
    DataModel *datamodel = GetDataModel();
    camera = datamodel->Workspace->CurrentCamera;

    serialize_Instance(camera, inst);

    camera->instance.ClassName = "Camera";

    serialize_atomic(Ref, camera, CameraSubject);
    serialize_atomic(token, camera, CameraType);
    serialize_atomic(CoordinateFrame, camera, CoordinateFrame);
    serialize_atomic(CoordinateFrame, camera, Focus);
}

static void xmlserialize_coordinateframe(CoordinateFrame *cf, struct xml_node *node)
{
    for (int i = 0; i < xml_node_children(node); i++)
    {
        struct xml_node *child = xml_node_child(node, i);
        char *propName = xml_easy_string(xml_node_name(child));
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
        char *propName = xml_easy_string(xml_node_name(child));
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
        char *propName = xml_easy_string(xml_node_name(child));
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

static const char *tokenPropNames[] = {
    "shape",
    "Controller",
    "Type",
    "Constraint",
    "SurfaceInput",
    "PostureXML",
};

static const char *tokenTables[][50] = {
    { "Ball", "Block", "Cylinder", "Wedge", "CornerWedge", NULL }, // shape
    { "None", "Player", "KeyboardLeft", "KeyboardRight", "Joypad1", "Joypad2", "Chase", "Flee", NULL }, //Controller
    { "Smooth", "Glue", "Weld", "Studs", "Inlet", "Universal", "Hinge", "Motor", "SteppingMotor", "Bumps", "Spawn", NULL }, // Type
    { "None", "Hinge", "Motor", "SteppingMotor", NULL }, //Constraint
    { "NoInput", "LeftTread", "RightTread", "Steer", "Throttle", "Updown", "Action1", "Action2", "Action3", "Action4", "Action5", "Sin", "Constant", NULL }, // SurfaceInput
    { "Stand", NULL } // PostureXML
};

static void xmlserialize_token(int *val, char *prop, char *propName)
{
    int size = sizeof(tokenPropNames) / sizeof(char *);

    char *endptr;
    unsigned long asNumber = strtoul(prop, &endptr, 10);

    if (endptr != prop)
    {
        *val = asNumber;
    }

    printf("serialize_token: prop %s, propName %s, asNumber %d.\n", prop, propName, asNumber);

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

    if (*val == 0 && index == -1 && strlen(prop) > 1)
    {
        FIXME("token not serialized: %s\n", prop);
    }
}

static void serialize(SerializeInstance *inst, char *prop, char *propName, struct xml_node *child, Instance *ret, XMLRefsInstance *refsInst)
{
    bool done = false;

    char *type = xml_easy_string(xml_node_name(child));
    if (!strcmp(type, "Complex"))
    {
        for (int i = 0; i < xml_node_children(child); i++)
        {
            struct xml_node *child2 = xml_node_child(child, i);
            char *prop2 = xml_easy_string(xml_node_content(child2));
            char *propName2 = xml_easy_string(xml_node_attribute_content(child2, 0));
            char *propName2_Concat[128];
            char *propName2_trans = propName2;
            if (!strcmp(propName2_trans, "Type")) propName2_trans = "Surface";
            snprintf(propName2_Concat, 128, "%s%s", propName, propName2_trans);
            serialize(inst, prop2, propName2_Concat, child2, ret, refsInst);
            free(propName2);
            free(prop2);
        }

        free(type);
        return;
    }
    free(type);

    for (int j = 0; j < inst->serializationCount; j++)
    {
        if (!strcmp(inst->serializations[j].name, propName) ||
            (!strcmp(inst->serializations[j].name, "CFrame") && !strcmp(propName, "CoordinateFrame")) ||
            (!strcmp(inst->serializations[j].name, "RotVelocity") && !strcmp(propName, "RotVel")) ||
            (!strcmp(inst->serializations[j].name, "Locked") && !strcmp(propName, "CanSelect")) ||
            (!strcmp(inst->serializations[j].name, "ClassName") && !strcmp(propName, "Keywords")))
        {
            done = true;
            void *val = inst->serializations[j].val;
            switch (inst->serializations[j].type)
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
                    if (Instance_IsA(ret, "Part") && !strcmp(propName, "shape"))
                    {
                        Part_SetShape(ret, ((Part*)ret)->shape);
                    }
                } break;
                case Serialize_int:
                {
                    *(int*)val = atoi(prop);
                    if (Instance_IsA(ret, "BasePart") && !strcmp(propName, "BrickColor"))
                    {
                        BasePart_SetBrickColor(ret, atoi(prop));
                    }
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
                    if (Instance_IsA(ret, "BasePart") && !strcmp(propName, "CFrame"))
                    {
                        BasePart_SetCFrame(ret, ((BasePart*)ret)->CFrame);
                    }
                } break;
                case Serialize_Color3:
                {
                    xmlserialize_color3(val, child);
                    if (Instance_IsA(ret, "BasePart") && !strcmp(propName, "Color"))
                    {
                        BasePart_SetColor(ret, ((BasePart*)ret)->Color);
                    }
                } break;
                case Serialize_Vector3:
                {
                    xmlserialize_vector3(val, child);
                    if (Instance_IsA(ret, "BasePart") && !strcmp(propName, "Position"))
                    {
                        BasePart_SetPosition(ret, ((BasePart*)ret)->Position);
                    }
                } break;
                case Serialize_double:
                {
                    *(double*)val = atof(prop);
                } break;
                case Serialize_Ref:
                {
                    refsInst->refCount++;
                    refsInst->refs = realloc(refsInst->refs, refsInst->refCount * sizeof(XMLRef));
                    refsInst->refs[refsInst->refCount - 1] = (XMLRef){prop, val};
                } break;
                default:
                {
                    FIXME("serialization type %d not implemented.\n", inst->serializations[j].type);
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

static int SurfaceType_from_Constraint05(int constraint05)
{
    switch (constraint05)
    {
        case Constraint05_None: return -1;
        case Constraint05_Hinge: return SurfaceType_Hinge;
        case Constraint05_Motor: return SurfaceType_Motor;
        case Constraint05_SteppingMotor: return SurfaceType_SteppingMotor;
    }
    return -1;
}

static Instance *load_model_part_xml(struct xml_node *node, XMLRefsInstance *refsInst)
{
    char *className = xml_easy_string(xml_node_attribute_content(node, 0));

    struct xml_node *propertyNode = xml_node_child(node, 0);
    SerializeInstance inst = { 0 };
    Instance *ret;

    ret = Instance_dynNew(className, NULL);
    Instance_Serialize(ret, &inst);
/*
    if (!strcmp(className, "TrussPart"))
    {
        ret = serialize_TrussPart(&inst);
    }
    else if (!strcmp(className, "Part"))
    {
        ret = serialize_Part(&inst);
    }
    else if (!strcmp(className, "Model"))
    {
        ret = Model_new("Model", NULL);
        serialize_Model(ret, &inst);
    }
    else if (!strcmp(className, "PhysicalCharacter"))
    {
        ret = serialize_PhysicalCharacter(&inst);
    }
    else if (!strcmp(className, "Workspace"))
    {
        ret = serialize_Workspace(&inst);
    }
    else if (!strcmp(className, "Camera"))
    {
        ret = serialize_Camera(&inst);
    }
    else
    {
        printf("error: no serializer for classname %s.\n", className);
        return NULL;
    }*/

    ret->xmlref = xml_easy_string(xml_node_attribute_content(node, 1));

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
                free(type);
                continue;
            }
            free(pName);
            for (int j = 0; j < xml_node_children(child); j++)
            {
                struct xml_node *child2 = xml_node_child(child, j);
                char *propName = xml_easy_string(xml_node_attribute_content(child2, 0));
                char *prop = xml_easy_string(xml_node_content(child2));
                serialize(&inst, prop, propName, child2, ret, refsInst);
                free(propName);
                free(prop);
            }
        }
        else
        {
            char *propName = xml_easy_string(xml_node_attribute_content(child, 0));
            char *prop = xml_easy_string(xml_node_content(child));
            serialize(&inst, prop, propName, child, ret, refsInst);
            free(propName);
            free(prop);
        }
        free(type);
    }

    free(inst.serializations);

    for (int i = 0; i < xml_node_children(node); i++)
    {
        struct xml_node *child = xml_node_child(node, i);
        char *type = xml_easy_string(xml_node_name(child));
        if (!strcmp(type, "Item"))
        {
            Instance *new = load_model_part_xml(child, refsInst);
            if (new) Instance_SetParent(new, ret);
        }
        free(type);
    }

    if (!strcmp(className, "Model"))
    {
        Model_MoveTo(ret, (Vector3){inst.modelOffset.X, inst.modelOffset.Y, inst.modelOffset.Z});
    }

    if (Instance_IsA(ret, "BasePart"))
    {
        BasePart *basepart = ret;
        #define X(face) if (inst.face##Constraint) basepart->face##Surface = SurfaceType_from_Constraint05(inst.face##Constraint);
        X(Top)
        X(Bottom)
        X(Left)
        X(Right)
        X(Front)
        X(Back)
        #undef X
        if (inst.Color3uint8)
        {
            int r, g, b;
            b = inst.Color3uint8 & 0x0000FF;
            g = inst.Color3uint8 & 0x00FF00;
            r = inst.Color3uint8 & 0xFF0000;

            g >>= 8;
            r >>= 16;
            BasePart_SetColor(basepart, (Color3){(float)r/255, (float)g/255, (float)b/255});
        }
    }

    free(className);

    return ret;
}

static void load_model_refs(XMLRefsInstance *inst, Instance *top)
{
    int descendantCount;
    Instance **descendants;
    
    if (!inst) return;

    descendants = Instance_GetDescendants(top, &descendantCount);

    for (int i = 0; i < descendantCount; i++)
    {
        if (!descendants[i]) continue;
        for (int j = 0; j < inst->refCount; j++)
        {
            if (!strcmp(descendants[i]->xmlref, inst->refs[j].ref))
            {
                *(inst->refs[j].val) = descendants[i];
            }
        }
    }

    free(descendants);
}

Instance **LoadModelRBXMX(const char *file, int *mdlCount)
{
    FILE *f = fopen(file, "r");
    if (!f)
    {
        fprintf(stderr, "an error occurred while parsing file \"%s\": ", file);
        perror("error");
        *mdlCount = 0;
        return NULL;
    }
    struct xml_document *document = xml_open_document(f);

    struct xml_node *root = xml_document_root(document);

    Instance **ret = NULL;
    *mdlCount = 0;

    XMLRefsInstance refsInst = { 0 };

    for (size_t i = 0; i < xml_node_children(root); i++)
    {
        struct xml_node *child = xml_node_child(root, i);
        uint8_t *name = xml_easy_name(child);
        if (!strcmp(name, "Item")) {
            (*mdlCount)++;
            ret = realloc(ret, sizeof(Instance *) * *mdlCount);
            ret[*mdlCount - 1] = load_model_part_xml(child, &refsInst);
        }
        free(name);
    }

    if (*mdlCount != 0)
    {
        if (*mdlCount > 1)
        {
            printf("limitation: refs don't work with multiple top-level instances in a model.\n");
        }
        else
        {
            load_model_refs(&refsInst, ret[0]);
        }
    }

    xml_document_free(document, true);

    return ret;
}

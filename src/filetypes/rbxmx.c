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

#include "debug.h"

DEFAULT_DEBUG_CHANNEL(filetypes)

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
    Serialize_Ref,
    Serialize_double,
} XMLSerializationType;

typedef struct XMLSerialization {
    XMLSerializationType type;
    const char *name;
    void *val;
} XMLSerialization;

typedef struct XMLSerializeInstance {
    XMLSerialization *serializations;
    int serializationCount;
    CFrame modelOffset;
    uint32_t Color3uint8;
    int TopConstraint, BottomConstraint, LeftConstraint, RightConstraint, FrontConstraint, BackConstraint;
} XMLSerializeInstance;

typedef struct XMLRef {
    const char *ref;
    Instance **val;
} XMLRef;

typedef struct XMLRefsInstance {
    int refCount;
    XMLRef *refs;
} XMLRefsInstance;

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

    xmlserialize_atomic(token, inst, TopConstraint);
    xmlserialize_atomic(token, inst, BottomConstraint);
    xmlserialize_atomic(token, inst, LeftConstraint);
    xmlserialize_atomic(token, inst, RightConstraint);
    xmlserialize_atomic(token, inst, FrontConstraint);
    xmlserialize_atomic(token, inst, BackConstraint);
    xmlserialize_atomic(int, inst, Color3uint8);
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

static void xmlserialize_Model(Model_Instance *model, XMLSerializeInstance *inst)
{
    xmlserialize_PVInstance(model, inst);

    _xmlserialize_atomic(inst, (XMLSerialization){Serialize_CoordinateFrame, "CoordinateFrame", &inst->modelOffset});
}

static PhysicalCharacter *xmlserialize_PhysicalCharacter(XMLSerializeInstance *inst)
{
    PhysicalCharacter *physicalcharacter = PhysicalCharacter_new(NULL);

    xmlserialize_Model(physicalcharacter, inst);

    xmlserialize_atomic(token, physicalcharacter, PostureXML);
    xmlserialize_atomic(Ref, physicalcharacter, Head);
    xmlserialize_atomic(Ref, physicalcharacter, UpperBody);
    xmlserialize_atomic(Ref, physicalcharacter, LowerBody);
    xmlserialize_atomic(Ref, physicalcharacter, LeftLeg);
    xmlserialize_atomic(Ref, physicalcharacter, RightLeg);
    xmlserialize_atomic(Ref, physicalcharacter, LeftArm);
    xmlserialize_atomic(Ref, physicalcharacter, RightArm);

    return physicalcharacter;
}

static void xmlserialize_RootInstance(RootInstance *rootinstance, XMLSerializeInstance *inst)
{
    xmlserialize_Model(rootinstance, inst);
}

static Workspace *xmlserialize_Workspace(XMLSerializeInstance *inst)
{
    Workspace *workspace = Workspace_new(NULL);
    DataModel *datamodel = GetDataModel();

    Instance_Remove(datamodel->Workspace);
    datamodel->Workspace = workspace;

    xmlserialize_Instance(workspace, inst);

    workspace->rootinstance.model.pvinstance.instance.ClassName = "Workspace";

    xmlserialize_atomic(double, workspace, DistributedGameTime);

    return workspace;
}

static Camera_Instance *xmlserialize_Camera(XMLSerializeInstance *inst)
{
    Camera_Instance *camera = Camera_new(NULL);
    DataModel *datamodel = GetDataModel();

    Instance_Remove(Instance_FindFirstChildOfClass(datamodel->Workspace, "Camera"));

    xmlserialize_Instance(camera, inst);

    camera->instance.ClassName = "Camera";

    xmlserialize_atomic(Ref, camera, CameraSubject);
    xmlserialize_atomic(token, camera, CameraType);
    xmlserialize_atomic(CoordinateFrame, camera, CoordinateFrame);
    xmlserialize_atomic(CoordinateFrame, camera, Focus);

    return camera;
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

    printf("xmlserialize_token: prop %s, propName %s, asNumber %d.\n", prop, propName, asNumber);

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

static void serialize(XMLSerializeInstance *inst, char *prop, char *propName, struct xml_node *child, Instance *ret, XMLRefsInstance *refsInst)
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
                    if (Instance_IsA(ret, "BasePart") && !strcmp(propName, "CoordinateFrame"))
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
    else if (!strcmp(className, "PhysicalCharacter"))
    {
        ret = xmlserialize_PhysicalCharacter(&inst);
    }
    else if (!strcmp(className, "Workspace"))
    {
        ret = xmlserialize_Workspace(&inst);
    }
    else if (!strcmp(className, "Camera"))
    {
        ret = xmlserialize_Camera(&inst);
    }
    else
    {
        printf("error: no serializer for classname %s.\n", className);
        return NULL;
    }

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
    if (!f) return NULL;
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

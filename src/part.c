
#include <raylib.h>
#include "part.h"
#include <stdlib.h>
#include "debug.h"
#include <stdio.h>
#include "meshcontentprovider.h"
#include "datamodel.h"
#include "rlgl.h"
#include "serialize.h"
#include <string.h>
#include "datamodelmesh.h"
#include "decal.h"
#include "texturecontentprovider.h"

DEFAULT_DEBUG_CHANNEL(part)

static Color rl_from_color3(Color3 col, float transparency)
{
    return (Color){col.R * 255, col.G * 255, col.B * 255, (1.0f-transparency) * 255};
}

static Matrix cf_to_rot_matrix(CFrame cf)
{
    return (Matrix) {
        // Right to Left, Up to Up, Back to Forward
        -cf.R00, cf.R01, -cf.R02, 0.0f,
        -cf.R10, cf.R11, -cf.R12, 0.0f,
        -cf.R20, cf.R21, -cf.R22, 0.0f,
           0.0f,   0.0f,    0.0f, 1.0f };
}

static Matrix cf_to_trans_matrix(CFrame cf)
{
    return MatrixTranslate(cf.X, cf.Y, cf.Z);
}

static Matrix size_to_scale_matrix(Vector3 size)
{
    return MatrixScale(size.x, size.y, size.z);
}

static Matrix cf_size_to_matrix(CFrame cf, Vector3 size)
{
    return MatrixMultiply(MatrixMultiply(size_to_scale_matrix(size), cf_to_rot_matrix(cf)), cf_to_trans_matrix(cf));
}

static Matrix cf_to_matrix(CFrame cf)
{
    return MatrixMultiply(cf_to_rot_matrix(cf), cf_to_trans_matrix(cf));
}

static Vector3 normal_from_NormalId(NormalId nid)
{
    switch (nid)
    {
        case NormalId_Right: return (Vector3){ 1, 0, 0};
        case NormalId_Top:   return (Vector3){ 0, 1, 0};
        case NormalId_Back:  return (Vector3){ 0, 0,-1};
        case NormalId_Left:  return (Vector3){-1, 0, 0};
        case NormalId_Bottom:return (Vector3){ 0,-1, 0};
        case NormalId_Front: return (Vector3){ 0, 0, 1};
    }
    return (Vector3){0, 0, 0};
}

void Draw3DSurface(SurfaceType sf, NormalId normal, Vector3 ptSize)
{
    if (sf != SurfaceType_Hinge && sf != SurfaceType_Motor && sf != SurfaceType_SteppingMotor)
    {
        return;
    }
    rlPushMatrix();
        Vector3 translate = Vector3Multiply(normal_from_NormalId(normal), Vector3Divide(ptSize, (Vector3){2, 2, 2}));
        rlTranslatef(translate.x, translate.y, translate.z);
        rlRotatef(90, 1, 0, 0);
        DrawCylinder((Vector3){0, -0.4f, 0}, 0.1f, 0.1f, 0.8f, 12, YELLOW);
        if (sf != SurfaceType_Hinge)
        {
            DrawCylinder((Vector3){0, -0.2f, 0}, 0.2f, 0.2f, 0.4f, 12, GRAY);
        }
    rlPopMatrix();
}

// Draw cube with texture piece applied to all faces
void DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color)
{
    float x = position.x;
    float y = position.y;
    float z = position.z;
    float texWidth = (float)texture.width;
    float texHeight = (float)texture.height;

    // Set desired texture to be enabled while drawing following vertex data
    rlSetTexture(texture.id);

    // We calculate the normalized texture coordinates for the desired texture-source-rectangle
    // It means converting from (tex.width, tex.height) coordinates to [0.0f, 1.0f] equivalent
    rlBegin(RL_QUADS);
        rlColor4ub(color.r, color.g, color.b, color.a);

        // Front face
        rlNormal3f(0.0f, 0.0f, 1.0f);
        rlTexCoord2f(source.x/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x - width/2, y - height/2, z + length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x + width/2, y - height/2, z + length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, source.y/texHeight);
        rlVertex3f(x + width/2, y + height/2, z + length/2);
        rlTexCoord2f(source.x/texWidth, source.y/texHeight);
        rlVertex3f(x - width/2, y + height/2, z + length/2);

        // Back face
        rlNormal3f(0.0f, 0.0f, - 1.0f);
        rlTexCoord2f((source.x + source.width)/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x - width/2, y - height/2, z - length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, source.y/texHeight);
        rlVertex3f(x - width/2, y + height/2, z - length/2);
        rlTexCoord2f(source.x/texWidth, source.y/texHeight);
        rlVertex3f(x + width/2, y + height/2, z - length/2);
        rlTexCoord2f(source.x/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x + width/2, y - height/2, z - length/2);

        // Top face
        rlNormal3f(0.0f, 1.0f, 0.0f);
        rlTexCoord2f(source.x/texWidth, source.y/texHeight);
        rlVertex3f(x - width/2, y + height/2, z - length/2);
        rlTexCoord2f(source.x/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x - width/2, y + height/2, z + length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x + width/2, y + height/2, z + length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, source.y/texHeight);
        rlVertex3f(x + width/2, y + height/2, z - length/2);

        // Bottom face
        rlNormal3f(0.0f, - 1.0f, 0.0f);
        rlTexCoord2f((source.x + source.width)/texWidth, source.y/texHeight);
        rlVertex3f(x - width/2, y - height/2, z - length/2);
        rlTexCoord2f(source.x/texWidth, source.y/texHeight);
        rlVertex3f(x + width/2, y - height/2, z - length/2);
        rlTexCoord2f(source.x/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x + width/2, y - height/2, z + length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x - width/2, y - height/2, z + length/2);

        // Right face
        rlNormal3f(1.0f, 0.0f, 0.0f);
        rlTexCoord2f((source.x + source.width)/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x + width/2, y - height/2, z - length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, source.y/texHeight);
        rlVertex3f(x + width/2, y + height/2, z - length/2);
        rlTexCoord2f(source.x/texWidth, source.y/texHeight);
        rlVertex3f(x + width/2, y + height/2, z + length/2);
        rlTexCoord2f(source.x/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x + width/2, y - height/2, z + length/2);

        // Left face
        rlNormal3f( - 1.0f, 0.0f, 0.0f);
        rlTexCoord2f(source.x/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x - width/2, y - height/2, z - length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x - width/2, y - height/2, z + length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, source.y/texHeight);
        rlVertex3f(x - width/2, y + height/2, z + length/2);
        rlTexCoord2f(source.x/texWidth, source.y/texHeight);
        rlVertex3f(x - width/2, y + height/2, z - length/2);

    rlEnd();

    rlSetTexture(0);
}

static void draw_block(Part *this)
{
    CFrame cf = this->formfactorpart.basepart.CFrame;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float width = 1.0f;
    float height = 1.0f;
    float length = 1.0f;

    Matrix mat = cf_size_to_matrix(this->formfactorpart.basepart.CFrame, this->formfactorpart.basepart.size);
    Color color = rl_from_color3(this->formfactorpart.basepart.Color, this->formfactorpart.basepart.Transparency);

    TextureContentProvider *texcont = this->texCont;

    Texture2D studs = texcont->studsTexture;
    Shader tilingShader = texcont->tilingShader;
    Vector3 size = this->formfactorpart.basepart.size;

    int tilePositionLoc = GetShaderLocation(tilingShader, "tilePosition");
    int tileSizeLoc = GetShaderLocation(tilingShader, "tileSize");

    //BeginShaderMode(tilingShader);

    const int Y_STUD_SCALE = 4;
    const int X_STUD_SCALE = 4;

    Decal *decals[6] = {NULL};
    
    int childCount;
    Instance **children = Instance_GetChildren(this, &childCount);

    /*for (int i = 0; i < childCount; i++)
    {
        if (!strcmp(children[i]->ClassName, "Decal"))
        {
            Decal *dc = children[i];
            decals[dc->faceinstance.Face] = dc;
            if (!IsTextureValid(dc->texture))
            {
                dc->texture = TextureContentProvider_LoadTextureAsset(ServiceProvider_GetService(GetDataModel(), "TextureContentProvider"), dc->Texture);
            }
        }
    }*/

    rlPushMatrix();
        rlMultMatrixf(MatrixToFloat(mat)); // pos, rot, scale for us
        DrawCubeTextureRec(studs, (Rectangle){0, 0, studs.width, studs.height}, (Vector3){0, 0, 0}, 1, 1, 1, color);
    rlPopMatrix();

    // 3D Surfaces
    rlPushMatrix();
        rlMultMatrixf(MatrixToFloat(cf_to_matrix(this->formfactorpart.basepart.CFrame)));

        BasePart pt = this->formfactorpart.basepart;

        // front back top bottom right left
        #define X(sf) Draw3DSurface(this->formfactorpart.basepart.sf##Surface, NormalId_##sf, this->formfactorpart.basepart.size)
        X(Front);
        X(Back);
        X(Bottom);
        X(Top);
        X(Right);
        X(Left);
        #undef X

    rlPopMatrix();

    //EndShaderMode();

    //draw_bumps(this);
}

static void draw_cylinder(Part *this)
{
    CFrame cf = this->formfactorpart.basepart.CFrame;
    float r00 = cf.R00, r10 = cf.R10, r20 = cf.R20;
    cf.R00 = cf.R01;
    cf.R10 = cf.R11;
    cf.R20 = cf.R21;
    cf.R01 = r00;
    cf.R11 = r10;
    cf.R21 = r20;
    cf.R02 = -cf.R02;
    cf.R12 = -cf.R12;
    cf.R22 = -cf.R22;
    cf.X -= this->formfactorpart.basepart.size.x / 2;
    cf.Y += this->formfactorpart.basepart.size.y / 4;
    cf.Z -= this->formfactorpart.basepart.size.z;
    DrawMesh(this->mesh, this->material, cf_size_to_matrix(cf, this->formfactorpart.basepart.size));
}

static void draw_ball(Part *this)
{
    CFrame cf = this->formfactorpart.basepart.CFrame;
    DrawMesh(this->mesh, this->material, cf_size_to_matrix(cf, this->formfactorpart.basepart.size));
}

void part_draw(Part *this)
{
    //printf("drawing part %p, position %s, size %s.\n", this, debugstr_vector3(this->formfactorpart.basepart.Position), debugstr_vector3(this->formfactorpart.basepart.size));
    this->material.maps[MATERIAL_MAP_DIFFUSE].color = rl_from_color3(this->formfactorpart.basepart.Color, this->formfactorpart.basepart.Transparency);

    if (this->formfactorpart.basepart.Transparency == 1) return;

    BoundingBox part = (BoundingBox){
        Vector3Subtract(this->formfactorpart.basepart.Position, Vector3Divide(this->formfactorpart.basepart.size, (Vector3){2, 2, 2})),
        Vector3Add(this->formfactorpart.basepart.Position, Vector3Divide(this->formfactorpart.basepart.size, (Vector3){2, 2, 2}))
    };

    Vector3 camPos = GetDataModel()->Workspace->CurrentCamera->camera.position;

    const int RENDER_DISTANCE = 500;
    BoundingBox camera = (BoundingBox){
        Vector3Subtract(camPos, (Vector3){RENDER_DISTANCE, RENDER_DISTANCE, RENDER_DISTANCE}),
        Vector3Add(camPos, (Vector3){RENDER_DISTANCE, RENDER_DISTANCE, RENDER_DISTANCE})
    };

    if (!CheckCollisionBoxes(part, camera)) return;

    int childCount;
    Instance **children = Instance_GetChildren(this, &childCount);
    bool meshOverridden = false;

    for (int i = 0; i < childCount; i++)
    {
        if (Instance_IsA(children[i], "DataModelMesh"))
        {
            meshOverridden = true;
        }
        if (Instance_IsA(children[i], "Decal"))
        {
            Decal *decal = children[i];
        }
    }

    BasePart_Draw(this);

    if (meshOverridden) return;

    //printf("color %s.\n", debugstr_color3(this->formfactorpart.basepart.Color));

    switch (this->shape)
    {
        case Shape_Block: draw_block(this); break;
        case Shape_Cylinder: draw_cylinder(this); break;
        case Shape_Ball: draw_ball(this); break;
        default: FIXME("not implemented: %d.\n", this->shape); break;
    }

}

Part *Part_new(const char *className, Instance *parent)
{
    Part *newInst = FormFactorPart_new(className, parent);

    newInst->formfactorpart.basepart.pvinstance.instance.DataCost = sizeof(Part);
    newInst = realloc(newInst, sizeof(Part));

    newInst->formfactorpart.basepart.pvinstance.drawFunc = part_draw;

    newInst->material = LoadMaterialDefault();
    newInst->material.maps[MATERIAL_MAP_DIFFUSE].texture = ((TextureContentProvider*)ServiceProvider_GetService(GetDataModel(), "TextureContentProvider"))->studsTexture;
    newInst->shape = Shape_Block;

    Part_SetShape(newInst, newInst->shape);

    MeshContentProvider *mcp = ServiceProvider_GetService(GetDataModel(), "MeshContentProvider");

    newInst->texCont = ServiceProvider_GetService(GetDataModel(), "TextureContentProvider");

    //newInst->material.maps[MATERIAL_MAP_DIFFUSE].texture = mcp->studs;

    if (parent && !strcmp(className, "Part")) Instance_SetParent(newInst, parent);

    return newInst;
}

void Part_SetShape(Part *this, Shape shape)
{
    MeshContentProvider *mcp = ServiceProvider_GetService(GetDataModel(), "MeshContentProvider");
    this->mesh = MeshContentProvider_GetPartMesh(mcp, this->shape);
}

void serialize_Part(Part *part, SerializeInstance *inst)
{
    //Part *part = Part_new(NULL);

    serialize_FormFactorPart(part, inst);
    serialize_atomic(token, part, shape);
}

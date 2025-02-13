
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

static void draw_bump_surface(Part *this, Vector3 unit)
{
    unit = Vector3Multiply(unit, Vector3Divide(this->formfactorpart.basepart.size, (Vector3){2, 2, 2}));
    Vector3 start = unit, end;
    if (unit.y)
    {
        start.x = -this->formfactorpart.basepart.size.x / 2;
        start.z = -this->formfactorpart.basepart.size.z / 2;
        end = Vector3Add(start, this->formfactorpart.basepart.size);

        for (int x = start.x; x < end.x; x++)
        {
            for (int z = start.z; z < end.z; z++)
            {
                printf("draw {%d, %f, %d}\n", x, unit.y, z);
                rlPushMatrix();
                Vector3 axis;
                float angle;
                QuaternionToAxisAngle(QuaternionFromMatrix(cf_to_rot_matrix(this->formfactorpart.basepart.CFrame)), &axis, &angle);\
                rlRotatef(angle, axis.x, axis.y, axis.z);
                rlTranslatef(this->formfactorpart.basepart.Position.x, this->formfactorpart.basepart.Position.y, this->formfactorpart.basepart.Position.z);
                DrawCube((Vector3){x, unit.y, z}, 0.75f, 0.75f, 0.75f, rl_from_color3(this->formfactorpart.basepart.Color, this->formfactorpart.basepart.Transparency));
                rlPopMatrix();
            }
        }   
    }
}

static void draw_bumps(Part *this)
{
    printf("topsurface %d.\n", this->formfactorpart.basepart.TopSurface);
    if (this->formfactorpart.basepart.BackSurface == SurfaceType_Bumps)
    {
        draw_bump_surface(this, (Vector3){0, 0, -1});    
    }
    if (this->formfactorpart.basepart.FrontSurface == SurfaceType_Bumps)
    {
        draw_bump_surface(this, (Vector3){0, 0, 1});    
    }
    if (this->formfactorpart.basepart.BottomSurface == SurfaceType_Bumps)
    {
        draw_bump_surface(this, (Vector3){0, -1, 0});    
    }
    if (this->formfactorpart.basepart.TopSurface == SurfaceType_Bumps)
    {
        draw_bump_surface(this, (Vector3){0, 1, 0});    
    }
    if (this->formfactorpart.basepart.LeftSurface == SurfaceType_Bumps)
    {
        draw_bump_surface(this, (Vector3){-1, 0, 0});    
    }
    if (this->formfactorpart.basepart.RightSurface == SurfaceType_Bumps)
    {
        draw_bump_surface(this, (Vector3){1, 0, 0});    
    }
}

void BeginSurfaceMode(SurfaceType surface, Shader tilingShader, int tilePositionLoc, Texture2D studs)
{
    int texOffset = 0;
    bool shouldDraw = surface != SurfaceType_Smooth && surface != SurfaceType_SmoothNoOutlines && surface != SurfaceType_Hinge && surface != SurfaceType_Motor && surface != SurfaceType_SteppingMotor;
    if (shouldDraw)
    {
        //printf("Begin Surface Mode %d\n", surface);
        switch (surface)
        {
            case SurfaceType_Bumps:
                FIXME("treating 2005 SurfaceType as studs (%d)\n", surface);
            case SurfaceType_Studs:
            {
                texOffset = 0;
            } break;
            case SurfaceType_Weld:
            {
                texOffset = 1;
            } break;
            case SurfaceType_Inlet:
            {
                texOffset = 2;
            } break;
            case SurfaceType_Universal:
            {
                texOffset = 3;
            } break;
            default:
            {
                FIXME("Unknown SurfaceType %d.\n", surface);
            } break;
        }
        rlSetTexture(studs.id);
        float tilePosition[2] = {0.0f, 0.25f*texOffset};
        SetShaderValue(tilingShader, tilePositionLoc, tilePosition, SHADER_UNIFORM_VEC2);
    }
    else
    {
        //printf("no surface.\n");
    }
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

void EndSurfaceMode()
{
    //printf("End surface.\n");
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

    BeginShaderMode(tilingShader);

    const int Y_STUD_SCALE = 32;
    const int X_STUD_SCALE = 2;

    Decal *decals[6] = {NULL};
    
    int childCount;
    Instance **children = Instance_GetChildren(this, &childCount);

    for (int i = 0; i < childCount; i++)
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
    }

    rlPushMatrix();
        // NOTE: Transformation is applied in inverse order (scale -> rotate -> translate)
        rlMultMatrixf(MatrixToFloat(mat));

        //DrawLine3D((Vector3){0, 0, 0}, (Vector3){0, 0, 1}, BLUE);

        //printf("Matrix: %s\n", debugstr_matrix(rlGetMatrixTransform()));
        //printf("Origin: %s\n", debugstr_matrix(mat));

        rlBegin(RL_QUADS);
            rlColor4ub(color.r, color.g, color.b, color.a);

            // Front face
            rlNormal3f(0.0f, 0.0f, 1.0f);

            //printf("FRONT ");

            //rlColor3f(1.0f, 0.0f, 1.0f);
            BeginSurfaceMode(this->formfactorpart.basepart.FrontSurface, tilingShader, tilePositionLoc, studs);
            if (decals[NormalId_Front]) rlEnableTexture(decals[NormalId_Front]->texture.id);

            rlTexCoord2f(0.0f, size.y/Y_STUD_SCALE);
            rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Left

            rlTexCoord2f(size.x/X_STUD_SCALE, size.y/Y_STUD_SCALE);
            rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right

            rlTexCoord2f(size.x/X_STUD_SCALE, 0.0f);
            rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Right

            rlTexCoord2f(0.0f, 0.0f);
            rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left

            EndSurfaceMode();

            // Back face
            rlNormal3f(0.0f, 0.0f, -1.0f);

            //printf("BACK ");

            //rlColor3f(1.0f, 1.0f, 0.0f);

            BeginSurfaceMode(this->formfactorpart.basepart.BackSurface, tilingShader, tilePositionLoc, studs);
            if (decals[NormalId_Back]) rlEnableTexture(decals[NormalId_Back]->texture.id);

            rlTexCoord2f(0.0f, 0.0f);
            rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left

            rlTexCoord2f(size.x/X_STUD_SCALE, 0.0f);
            rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right

            rlTexCoord2f(size.x/X_STUD_SCALE, size.y/Y_STUD_SCALE);
            rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right

            rlTexCoord2f(0.0f, size.y/Y_STUD_SCALE);
            rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Left

            EndSurfaceMode();

            // Top face
            rlNormal3f(0.0f, 1.0f, 0.0f);

            //printf("TOP ");

            //rlColor3f(0.0f, 1.0f, 1.0f);

            BeginSurfaceMode(this->formfactorpart.basepart.TopSurface, tilingShader, tilePositionLoc, studs);
            if (decals[NormalId_Top]) rlEnableTexture(decals[NormalId_Top]->texture.id);

            rlTexCoord2f(0.0f, size.z/Y_STUD_SCALE);
            rlVertex3f(-0.5f, 0.5f, 0.5f);  // Bottom Left

            rlTexCoord2f(size.x/X_STUD_SCALE, size.z/Y_STUD_SCALE);
            rlVertex3f(0.5f, 0.5f, 0.5f);  // Bottom Right

            rlTexCoord2f(size.x/X_STUD_SCALE, 0.0f);
            rlVertex3f(0.5f, 0.5f, -0.5f);  // Top Right

            rlTexCoord2f(0.0f, 0.0f);
            rlVertex3f(-0.5f, 0.5f, -0.5f);  // Top Left

            EndSurfaceMode();

            // Bottom face
            rlNormal3f(0.0f, -1.0f, 0.0f);

            //printf("BOTTOM ");

            //rlColor3f(1.0f, 1.0f, 1.0f);

            BeginSurfaceMode(this->formfactorpart.basepart.BottomSurface, tilingShader, tilePositionLoc, studs);
            if (decals[NormalId_Bottom]) rlEnableTexture(decals[NormalId_Bottom]->texture.id);

            rlTexCoord2f(0.0f, 0.0f);
            rlVertex3f(x - width/2, y - height/2, z - length/2);  // Top Left

            rlTexCoord2f(size.x/X_STUD_SCALE, 0.0f);
            rlVertex3f(x + width/2, y - height/2, z - length/2);  // Top Right

            rlTexCoord2f(size.x/X_STUD_SCALE, size.z/Y_STUD_SCALE);
            rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right
            
            rlTexCoord2f(0.0f, size.z/Y_STUD_SCALE);
            rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Left

            EndSurfaceMode();

            // Right face
            rlNormal3f(1.0f, 0.0f, 0.0f);

            //printf("RIGHT ");

            //rlColor3f(1.0f, 0.0f, 0.0f);

            BeginSurfaceMode(this->formfactorpart.basepart.RightSurface, tilingShader, tilePositionLoc, studs);
            if (decals[NormalId_Right]) rlEnableTexture(decals[NormalId_Right]->texture.id);

            rlTexCoord2f(0.0f, size.y/Y_STUD_SCALE);
            rlVertex3f(0.5f, -0.5f, 0.5f);  // Bottom Left

            rlTexCoord2f(size.z/X_STUD_SCALE, size.y/Y_STUD_SCALE);
            rlVertex3f(0.5f, -0.5f, -0.5f);  // Bottom Right

            rlTexCoord2f(size.z/X_STUD_SCALE, 0.0f);
            rlVertex3f(0.5f, 0.5f, -0.5f);  // Top Right

            rlTexCoord2f(0.0f, 0.0f);
            rlVertex3f(0.5f, 0.5f, 0.5f);  // Top Left

            EndSurfaceMode();

            // Left face
            rlNormal3f(-1.0f, 0.0f, 0.0f);

            //printf("LEFT ");
            //rlColor3f(0.0f, 0.0f, 1.0f);

            BeginSurfaceMode(this->formfactorpart.basepart.LeftSurface, tilingShader, tilePositionLoc, studs);
            if (decals[NormalId_Left]) rlEnableTexture(decals[NormalId_Left]->texture.id);

            rlTexCoord2f(0.0f, 0.0f);
            rlVertex3f(-0.5f, 0.5f, 0.5f);  // Top Left

            rlTexCoord2f(size.z/X_STUD_SCALE, 0.0f);
            rlVertex3f(-0.5f, 0.5f, -0.5f);  // Top Right

            rlTexCoord2f(size.z/X_STUD_SCALE, size.y/Y_STUD_SCALE);
            rlVertex3f(-0.5f, -0.5f, -0.5f);  // Bottom Right

            rlTexCoord2f(0.0f, size.y/Y_STUD_SCALE);
            rlVertex3f(-0.5f, -0.5f, 0.5f);  // Bottom Left

            EndSurfaceMode();
        rlEnd();
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

    EndShaderMode();

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
    //newInst->material.maps[MATERIAL_MAP_DIFFUSE].texture = ((TextureContentProvider*)ServiceProvider_GetService(GetDataModel(), "TextureContentProvider"))->studsTexture;
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

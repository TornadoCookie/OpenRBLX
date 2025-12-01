#include "renderer.h"
#include <raylib.h>
#include <rlgl.h>

#include "meshcontentprovider.h"
#include "datamodel.h"

static struct {
    RenderObject3D *objs;
    int objCount;
    MeshContentProvider *mcp;
    Material blankMaterial;
} renderer;

void InitRenderer()
{
    renderer.objCount = 0;
    renderer.objs = NULL;
    renderer.mcp = ServiceProvider_GetService(GetDataModel(), "MeshContentProvider");
    renderer.blankMaterial = LoadMaterialDefault();
}

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

static void setColor(Color3 col, float transparency)
{
    Color c = rl_from_color3(col, transparency);
    rlColor4ub(c.r, c.g, c.b, c.a);
}

static void renderBall(RenderObject3D obj)
{
}

static void renderBlock(RenderObject3D obj)
{
    // TODO
}

static void renderCylinder(RenderObject3D obj)
{
    CFrame cf = obj.cframe;
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
    cf.X -= obj.size.x / 2;
    cf.Y += obj.size.y / 4;
    cf.Z -= obj.size.z;
    DrawMesh(renderer.mcp->cylinderMesh, renderer.blankMaterial, cf_size_to_matrix(cf, obj.size));
}

static void renderMesh(RenderObject3D obj)
{
    // TODO
}

static void renderObject(RenderObject3D obj)
{
    setColor(obj.color, obj.transparency);
    switch (obj.shape)
    {
        case RS_Ball:
        {
            renderBall(obj);
        } break;
        case RS_Block:
        {
            renderBlock(obj);
        } break;
        case RS_Cylinder:
        {
            renderCylinder(obj);
        } break;
        case RS_Mesh:
        {
            renderMesh(obj);
        } break;
    }
}

void RenderGame3D()
{
    for (int i = 0; i < renderer.objCount; i++)
    {
        renderObject(renderer.objs[i]);
    }
}

void Add3DRenderObject(RenderObject3D obj)
{
    renderer.objCount++;
    renderer.objs = realloc(renderer.objs, renderer.objCount * sizeof(RenderObject3D));
    renderer.objs[renderer.objCount - 1] = obj;
}


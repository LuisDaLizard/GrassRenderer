//
// Created by Luis on 3/26/2024.
//

#include "Model.h"

#include <cstring>
#include <OBJ_Loader.h>

Model::Model(Graphics &graphics, Pipeline &pipeline, const Triangle *pTriangles, int numTriangles)
    : mGraphics(graphics), mPipeline(pipeline), mMesh(nullptr), mNumTriangles(numTriangles)
{
    mTriangles = new Triangle[numTriangles];
    memcpy(mTriangles, pTriangles, mNumTriangles * sizeof(Triangle));

    if (!GenerateMesh())
        WriteError(1, "Unable to generate mesh");
}

Model::Model(Graphics &graphics, Pipeline &pipeline, const char *pFilename)
    : mGraphics(graphics), mPipeline(pipeline), mMesh(nullptr), mTriangles(nullptr), mNumTriangles(0)
{
    objl::Loader loader;

    bool loaded = loader.LoadFile(pFilename);

    if (!loaded)
        return;

    int totalVertices = loader.LoadedVertices.size();
    mNumTriangles = totalVertices / 3;
    mTriangles = new Triangle[mNumTriangles];

    for (int i = 0; i < totalVertices; i += 3)
    {
        Triangle tri;

        Vec3 p0 = {loader.LoadedVertices[i].Position.X, loader.LoadedVertices[i].Position.Y, loader.LoadedVertices[i].Position.Z};
        Vec3 n0 = {loader.LoadedVertices[i].Normal.X, loader.LoadedVertices[i].Normal.Y, loader.LoadedVertices[i].Normal.Z};
        Vec3 p1 = {loader.LoadedVertices[i + 1].Position.X, loader.LoadedVertices[i + 1].Position.Y, loader.LoadedVertices[i + 1].Position.Z};
        Vec3 n1 = {loader.LoadedVertices[i + 1].Normal.X, loader.LoadedVertices[i + 1].Normal.Y, loader.LoadedVertices[i + 1].Normal.Z};
        Vec3 p2 = {loader.LoadedVertices[i + 2].Position.X, loader.LoadedVertices[i + 2].Position.Y, loader.LoadedVertices[i + 2].Position.Z};
        Vec3 n2 = {loader.LoadedVertices[i + 2].Normal.X, loader.LoadedVertices[i + 2].Normal.Y, loader.LoadedVertices[i + 2].Normal.Z};

        tri.a = {p0, n0};
        tri.b = {p1, n1};
        tri.c = {p2, n2};

        mTriangles[i / 3] = tri;
    }

    if (!GenerateMesh())
        WriteError(1, "Unable to generate mesh");
}

Model::~Model()
{
    delete[] mTriangles;
    if (mMesh)
        MeshDestroy(mGraphics, mMesh);
}

Triangle Model::GetTriangle(int index) const
{
    if (index < 0 || index >= mNumTriangles)
        return {};
    return mTriangles[index];
}

int Model::NumTriangles() const
{
    return mNumTriangles;
}

void Model::Draw() const
{
    if (!mMesh)
        return;
    PipelineBind(mGraphics, mPipeline);
    MeshDraw(mGraphics, mMesh);
}

float Model::GetArea() const
{
    return mArea;
}

bool Model::GenerateMesh()
{
    mArea = 0;
    for (int i = 0; i < mNumTriangles; i++)
        mArea += Triangle::GetArea(mTriangles[i]);

    unsigned int stride = sizeof(Vertex);

    MeshCreateInfo createInfo = {};
    createInfo.vertexCount = mNumTriangles * 3;
    createInfo.bufferCount = 1;
    createInfo.strides = &stride;
    createInfo.ppData = (void **)&mTriangles;

    return MeshCreate(mGraphics, &createInfo, &mMesh);
}

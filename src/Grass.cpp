//
// Created by Luis on 3/26/2024.
//
#include "Grass.h"

Grass::Grass()
    : mGenerator(std::random_device{}()), mGrassCreateInfo({}), mNumBlades(0)
{
    mGrassCreateInfo.topology = TOPOLOGY_POINTS;
    mGrassCreateInfo.stride = sizeof(GrassBladeVertex);
    mGrassCreateInfo.numAttributes = 2;
    mGrassCreateInfo.pAttributeComponents = new int[] {3, 3};
}

Grass::~Grass()
{
    delete[] mGrassBlades;
}

void Grass::Generate(Model *pModel, int numBlades, int patchSize)
{
    if (!pModel || !numBlades)
        return;

    mPatchSize = patchSize;
    mNumBlades = numBlades;

    if (mNumBlades % mPatchSize != 0)
        mNumBlades += (mPatchSize - (numBlades % mPatchSize));

    printf("Generating blades: %d\n", mNumBlades);

    GenerateBlades(pModel);

    GeneratePatches();

    if (mGrassMesh)
        MeshDestroy(mGrassMesh);

    if (!MeshCreate(&mGrassCreateInfo, &mGrassMesh))
        WriteError(1, "Unable to create grass mesh");
}

void Grass::GenerateBlades(Model *pModel)
{
    delete[] mGrassBlades;
    mGrassBlades = new GrassBlade[mNumBlades];

    GrassBladeVertex *vertices = new GrassBladeVertex[mNumBlades];
    int triangles = pModel->NumTriangles();

    std::uniform_real_distribution<float> indexDistribution(0, (float)triangles);
    std::uniform_real_distribution<float> positionDistribution(0, 1);

    for (int i = 0; i < mNumBlades; i++)
    {
        int rIndex = (int)indexDistribution(mGenerator);
        Triangle rTriangle = pModel->GetTriangle(rIndex);

        float r1 = positionDistribution(mGenerator);
        float r2 = positionDistribution(mGenerator);

        float rootR1 = sqrtf(r1);

        mGrassBlades[i].v0.x = (1 - rootR1) * rTriangle.a.position.x + (rootR1 * (1 - r2)) * rTriangle.b.position.x + (rootR1 * r2) * rTriangle.c.position.x;
        mGrassBlades[i].v0.y = (1 - rootR1) * rTriangle.a.position.y + (rootR1 * (1 - r2)) * rTriangle.b.position.y + (rootR1 * r2) * rTriangle.c.position.y;
        mGrassBlades[i].v0.z = (1 - rootR1) * rTriangle.a.position.z + (rootR1 * (1 - r2)) * rTriangle.b.position.z + (rootR1 * r2) * rTriangle.c.position.z;

        vertices[i].position = mGrassBlades[i].v0;
        vertices[i].normal = {0, 1, 0};
    }

    mGrassCreateInfo.numVertices = mNumBlades;
    mGrassCreateInfo.pVertexData = (float *)vertices;
}

void Grass::Draw()
{
    if (mGrassMesh)
        MeshDraw(mGrassMesh);
}

void Grass::GeneratePatches()
{

}

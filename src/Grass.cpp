//
// Created by Luis on 3/26/2024.
//
#include "Grass.h"

#include <cfloat>

Grass::Grass()
    : mGenerator(std::random_device{}()), mGrassCreateInfo({}), mNumBlades(0), mPatchSize(0), mGenerationThread()
{
    mGrassCreateInfo.topology = TOPOLOGY_POINTS;
    mGrassCreateInfo.stride = sizeof(GrassBladeVertex);
    mGrassCreateInfo.numAttributes = 3;
    mGrassCreateInfo.pAttributeComponents = new int[] {3, 3, 1};

    ProgramCreateInfo grassShader = {};
    grassShader.pVertexSource = FileReadText("shaders/Grass.vert");
    grassShader.pGeometrySource = FileReadText("shaders/Grass.geom");
    grassShader.pFragmentSource = FileReadText("shaders/Grass.frag");

    if (!ProgramCreate(&grassShader, &mGrassShader))
        WriteError(1, "Unable to create grass shader");
}

Grass::~Grass()
{
    delete[] mGrassBlades;
}

void Grass::Generate(Model *pModel, int numBlades, int patchSize)
{
    if (!pModel || !numBlades)
        return;

    if (!mDoneGenerating)
        return;

    mPatchSize = patchSize;
    mNumBlades = numBlades;

    if (mNumBlades % mPatchSize != 0)
        mNumBlades += (mPatchSize - (numBlades % mPatchSize));

    mDoneGenerating = false;
    if (mGenerationThread.joinable())
        mGenerationThread.join();
    mGenerationThread = std::thread(&Grass::GenerationThread, this, pModel);
}

void Grass::FinishGeneration()
{
    if (!mDoneGenerating || !mPatchColors || !mGrassBlades)
        return;

    GenerateMesh();
}

void Grass::Draw(Matrix projection, Matrix view, float height, bool showPatches)
{
    if (!mGrassMesh)
        return;

    ProgramUse(mGrassShader);
    ProgramUploadMatrix(mGrassShader, ProgramUniformLocation(mGrassShader, "uWorld"), MatrixIdentity());
    ProgramUploadMatrix(mGrassShader, ProgramUniformLocation(mGrassShader, "uView"), view);
    ProgramUploadMatrix(mGrassShader, ProgramUniformLocation(mGrassShader, "uProjection"), projection);
    ProgramUploadFloat(mGrassShader, ProgramUniformLocation(mGrassShader, "uHeight"), height);
    ProgramUploadBool(mGrassShader, ProgramUniformLocation(mGrassShader, "uShowPatches"), showPatches);
    UniformBufferBind(mColorBuffer, 0);
    MeshDraw(mGrassMesh);
}

bool Grass::DoneGenerating()
{
    return mDoneGenerating;
}

void Grass::GenerationThread(Model *pModel)
{
    GenerateBlades(pModel);

    GeneratePatches();

    mDoneGenerating = true;
}

void Grass::GenerateBlades(Model *pModel)
{
    GrassBlade *blades = new GrassBlade[mNumBlades];
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

        blades[i].v0.x = (1 - rootR1) * rTriangle.a.position.x + (rootR1 * (1 - r2)) * rTriangle.b.position.x + (rootR1 * r2) * rTriangle.c.position.x;
        blades[i].v0.y = (1 - rootR1) * rTriangle.a.position.y + (rootR1 * (1 - r2)) * rTriangle.b.position.y + (rootR1 * r2) * rTriangle.c.position.y;
        blades[i].v0.z = (1 - rootR1) * rTriangle.a.position.z + (rootR1 * (1 - r2)) * rTriangle.b.position.z + (rootR1 * r2) * rTriangle.c.position.z;

        blades[i].patch = 0;
    }

    delete[] mGrassBlades;
    mGrassBlades = blades;
}

void Grass::GeneratePatches()
{
    std::uniform_real_distribution<float> color(0, 1);

    mNumPatches = mNumBlades / mPatchSize;

    Vec4 *colors = new Vec4[mNumPatches];

    // Generate patch colors
    for (int i = 0; i < mNumPatches; i++)
    {
        float r = color(mGenerator);
        float g = color(mGenerator);
        float b = color(mGenerator);

        colors[i].x = r;
        colors[i].y = g;
        colors[i].z = b;
        colors[i].w = 1;
    }

    delete[] mPatchColors;
    mPatchColors = colors;

    // Generate patches
    for (int i = 1; i <= mNumPatches; i++)
    {
        Vec3 startPoint;

        // Find first blade not in a patch
        for (int j = 0; j < mNumBlades; j++)
        {
            if (!mGrassBlades[j].patch)
            {
                startPoint = mGrassBlades[j].v0;
                mGrassBlades[j].patch = i;
                break;
            }
        }

        for (int j = 1; j < mPatchSize; j++)
        {
            float smallestDistance = FLT_MAX;
            int smallestIndex = 0;
            for (int k = 0; k < mNumBlades; k++)
            {
                if (mGrassBlades[k].patch)
                    continue;

                float distance = Vec3Magnitude(Vec3Sub(mGrassBlades[k].v0, startPoint));

                if (distance < smallestDistance)
                {
                    smallestDistance = distance;
                    smallestIndex = k;
                }
            }

            mGrassBlades[smallestIndex].patch = i;
        }
    }
}

void Grass::GenerateMesh()
{
    GrassBladeVertex *vertices = new GrassBladeVertex[mNumBlades];

    for (int i = 0; i < mNumBlades; i++)
    {
        vertices[i].position = mGrassBlades[i].v0;
        vertices[i].normal = {0, 1, 0};
        vertices[i].patch = mGrassBlades[i].patch;

        mGrassCreateInfo.numVertices = mNumBlades;
        mGrassCreateInfo.pVertexData = (float *)vertices;
    }

    if (mGrassMesh)
        MeshDestroy(mGrassMesh);


    if (!MeshCreate(&mGrassCreateInfo, &mGrassMesh))
        WriteError(1, "Unable to create grass mesh");


    delete[] vertices;

    UniformBufferCreateInfo buffer = {};
    buffer.size = mNumPatches * sizeof(Vec4);
    buffer.pData = mPatchColors;
    buffer.isStatic = true;

    if (mColorBuffer)
        UniformBufferDestroy(mColorBuffer);

    if (!UniformBufferCreate(&buffer, &mColorBuffer))
        WriteError(1, "Unable to create uniform buffer");

    UniformBufferBindToProgram(mColorBuffer, mGrassShader, "Colors", 0);


}

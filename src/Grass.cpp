//
// Created by Luis on 3/26/2024.
//
#include "Grass.h"

#include <cfloat>

Grass::Grass(Graphics &graphics)
    : mGraphics(graphics), mGenerator(std::random_device{}()), mGrassCreateInfo({}), mNumBlades(0), mPatchSize(0), mGenerationThread()
{
    mGrassCreateInfo.stride = sizeof(GrassBladeVertex);

    UniformBufferCreateInfo bufferInfo;
    bufferInfo.binding = 0;
    bufferInfo.size = sizeof(VertexUniforms);
    UniformBufferCreate(mGraphics, &bufferInfo, &mVertexUniformBuffer);
    bufferInfo.size = sizeof(MatrixUniforms);
    UniformBufferCreate(mGraphics, &bufferInfo, &mMatrixUniformBuffer);
    bufferInfo.size = sizeof(FragmentUniforms);
    UniformBufferCreate(mGraphics, &bufferInfo, &mFragmentUniformBuffer);

    Attribute attribs[4];
    attribs[0].location = 0;
    attribs[0].offset = 0;
    attribs[0].components = 3;

    attribs[1].location = 1;
    attribs[1].offset = offsetof(GrassBladeVertex, normal);
    attribs[1].components = 3;

    attribs[2].location = 2;
    attribs[2].offset = offsetof(GrassBladeVertex, direction);
    attribs[2].components = 3;

    attribs[3].location = 3;
    attribs[3].offset = offsetof(GrassBladeVertex, patch);
    attribs[3].components = 1;

    Descriptor descriptors[3];

    descriptors[0].location = 0;
    descriptors[0].stage = STAGE_VERTEX;
    descriptors[0].count = 1;
    descriptors[0].type = DESCRIPTOR_TYPE_UNIFORM;
    descriptors[0].uniform = mVertexUniformBuffer;

    descriptors[1].location = 1;
    descriptors[1].stage = STAGE_TESSELATION_EVALUATION;
    descriptors[1].count = 1;
    descriptors[1].type = DESCRIPTOR_TYPE_UNIFORM;
    descriptors[1].uniform = mMatrixUniformBuffer;

    descriptors[2].location = 2;
    descriptors[2].stage = STAGE_FRAGMENT;
    descriptors[2].count = 1;
    descriptors[2].type = DESCRIPTOR_TYPE_UNIFORM;
    descriptors[2].uniform = mFragmentUniformBuffer;

    int size = 0;
    PipelineCreateInfo grassPipeline;
    grassPipeline.stride = sizeof(GrassBladeVertex);
    grassPipeline.topology = TOPOLOGY_PATCH_LIST;
    grassPipeline.patchSize = 1;
    grassPipeline.pVertexShaderCode = (unsigned int *)FileReadBytes("shaders/bin/Grass.vert", &size);
    grassPipeline.vertexShaderSize = size;
    grassPipeline.pTessCtrlCode = (unsigned int *)FileReadBytes("shaders/bin/Grass.tesc", &size);
    grassPipeline.tessCtrlShaderSize = size;
    grassPipeline.pTessEvalCode = (unsigned int *)FileReadBytes("shaders/bin/Grass.tese", &size);
    grassPipeline.tessEvalShaderSize = size;
    grassPipeline.pFragmentShaderCode = (unsigned int *)FileReadBytes("shaders/bin/Grass.frag", &size);
    grassPipeline.fragmentShaderSize = size;
    grassPipeline.attributeCount = 4;
    grassPipeline.pAttributes = attribs;
    grassPipeline.descriptorCount = 3;
    grassPipeline.pDescriptors = descriptors;

    if (!PipelineCreate(mGraphics, &grassPipeline, &mGrassPipeline))
    {
        WriteError(1, "Failed to create grass pipeline");
    }
}

Grass::~Grass()
{
    delete[] mGrassBlades;

    if (mGenerationThread.joinable())
        mGenerationThread.join();

    if (mGrassMesh)
        MeshDestroy(mGraphics, mGrassMesh);
    UniformBufferDestroy(mGraphics, mVertexUniformBuffer);
    UniformBufferDestroy(mGraphics, mMatrixUniformBuffer);
    UniformBufferDestroy(mGraphics, mFragmentUniformBuffer);
    PipelineDestroy(mGraphics, mGrassPipeline);
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
    if (!mDoneGenerating || !mGrassBlades)
        return;

    GenerateMesh();
}

void Grass::Draw(Matrix projection, Matrix view, float height, float width, bool showPatches)
{
    if (!mGrassMesh)
        return;

    mMatrixUniforms.projection = projection;
    mMatrixUniforms.view = view;
    mMatrixUniforms.world = MatrixIdentity();
    UniformBufferSetData(mMatrixUniformBuffer, &mMatrixUniforms, sizeof(MatrixUniforms));

    mVertexUniforms.width = width;
    mVertexUniforms.height = height;
    UniformBufferSetData(mVertexUniformBuffer, &mVertexUniforms, sizeof(VertexUniforms));

    mFragmentUniforms.showPatches = showPatches;
    UniformBufferSetData(mFragmentUniformBuffer, &mFragmentUniforms, sizeof(FragmentUniforms));


    PipelineBind(mGraphics, mGrassPipeline);
    MeshDraw(mGraphics, mGrassMesh);
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
    std::uniform_real_distribution<float> directionGen(-1, 1);

    for (int i = 0; i < mNumBlades; i++)
    {
        int rIndex = (int)indexDistribution(mGenerator);
        Triangle rTriangle = pModel->GetTriangle(rIndex);

        float r1 = positionDistribution(mGenerator);
        float r2 = positionDistribution(mGenerator);

        float rootR1 = sqrtf(r1);

        blades[i].pos.x = (1 - rootR1) * rTriangle.a.position.x + (rootR1 * (1 - r2)) * rTriangle.b.position.x + (rootR1 * r2) * rTriangle.c.position.x;
        blades[i].pos.y = (1 - rootR1) * rTriangle.a.position.y + (rootR1 * (1 - r2)) * rTriangle.b.position.y + (rootR1 * r2) * rTriangle.c.position.y;
        blades[i].pos.z = (1 - rootR1) * rTriangle.a.position.z + (rootR1 * (1 - r2)) * rTriangle.b.position.z + (rootR1 * r2) * rTriangle.c.position.z;

        blades[i].norm = rTriangle.a.normal;

        blades[i].dir.x = directionGen(mGenerator);
        blades[i].dir.y = 0;
        blades[i].dir.z = directionGen(mGenerator);

        blades[i].dir = Vec3Normalize(blades[i].dir);

        blades[i].patch = 0;
    }

    delete[] mGrassBlades;
    mGrassBlades = blades;
}

void Grass::GeneratePatches()
{
    std::uniform_real_distribution<float> color(0, 1);

    mNumPatches = mNumBlades / mPatchSize;

    // Generate patch colors
    for (int i = 0; i < MAX_COLORS; i++)
    {
        float r = color(mGenerator);
        float g = color(mGenerator);
        float b = color(mGenerator);

        mFragmentUniforms.colors[i].x = r;
        mFragmentUniforms.colors[i].y = g;
        mFragmentUniforms.colors[i].z = b;
        mFragmentUniforms.colors[i].w = 1;
    }

    // Generate patches
    for (int i = 1; i <= mNumPatches; i++)
    {
        Vec3 startPoint;

        // Find first blade not in a patch
        for (int j = 0; j < mNumBlades; j++)
        {
            if (!mGrassBlades[j].patch)
            {
                startPoint = mGrassBlades[j].pos;
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

                float distance = Vec3Magnitude(Vec3Sub(mGrassBlades[k].pos, startPoint));

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
        vertices[i].position = mGrassBlades[i].pos;
        vertices[i].normal = mGrassBlades[i].norm;
        vertices[i].direction = mGrassBlades[i].dir;
        vertices[i].patch = (float)mGrassBlades[i].patch;

        mGrassCreateInfo.vertexCount = mNumBlades;
        mGrassCreateInfo.pVertices = (float *)vertices;
    }

    if (mGrassMesh)
        MeshDestroy(mGraphics, mGrassMesh);


    if (!MeshCreate(mGraphics, &mGrassCreateInfo, &mGrassMesh))
        WriteError(1, "Unable to create grass mesh");


    delete[] vertices;
}

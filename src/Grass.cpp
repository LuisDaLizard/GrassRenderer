//
// Created by Luis on 3/26/2024.
//
#include "Grass.h"
#include "Cluster.h"

#include <cfloat>
#include <mutex>

Attribute attributes[4] =
        {
                {0, 0, 4, 0},
                {1, 1, 4, 0},
                {2, 2, 4, 0},
                {3, 3, 4, 0},
        };

Descriptor grassDescriptors[2] =
        {
                {0, 1, STAGE_TESSELATION_EVALUATION, DESCRIPTOR_TYPE_UNIFORM, nullptr},
                {1, 1, STAGE_FRAGMENT, DESCRIPTOR_TYPE_UNIFORM, nullptr},
        };

Descriptor physicalDescriptors[4] =
        {
                {0, 1, STAGE_COMPUTE, DESCRIPTOR_TYPE_STORAGE, nullptr},
                {1, 1, STAGE_COMPUTE, DESCRIPTOR_TYPE_STORAGE, nullptr},
                {2, 1, STAGE_COMPUTE, DESCRIPTOR_TYPE_STORAGE, nullptr},
                {3, 1, STAGE_COMPUTE, DESCRIPTOR_TYPE_STORAGE, nullptr},
        };

InputBinding bindings[4] =
        {
                {0, sizeof(Vec4)},
                {1, sizeof(Vec4)},
                {2, sizeof(Vec4)},
                {3, sizeof(Vec4)},
        };

Grass::Grass(Graphics &graphics)
    : mGraphics(graphics), mGenerator(std::random_device{}()), mGrassCreateInfo({}), mNumMeshes(0), mNumPatches(0), mGenerationThread()
{
    InitPipelines();
}

Grass::~Grass()
{
    if (mGenerationThread.joinable())
        mGenerationThread.join();

    if (mPatchMeshes)
    {
        for (int i = 0; i < mNumMeshes; i++)
            MeshDestroy(mGraphics, mPatchMeshes[i]);
    }

    delete[] mPatchMeshes;
    UniformBufferDestroy(mGraphics, mMatrixUniformBuffer);
    UniformBufferDestroy(mGraphics, mFragmentUniformBuffer);
    PipelineDestroy(mGraphics, mGrassPipeline);
    PipelineDestroy(mGraphics, mPhysicalPipeline);
}

void Grass::Generate(Model *pModel, float density, int patchSize, float bladeWidth, float bladeHeight)
{
    if (!pModel)
        return;

    if (!mDoneGenerating)
        return;

    mDoneGenerating = false;
    if (mGenerationThread.joinable())
        mGenerationThread.join();
    mGenerationThread = std::thread(&Grass::GenerationThread, this, pModel, density, patchSize, bladeWidth, bladeHeight);
}

void Grass::FinishGeneration()
{
    if (!mDoneGenerating || !mPatches)
        return;

    if (mPatchMeshes)
    {
        for (int i = 0; i < mNumMeshes; i++)
            MeshDestroy(mGraphics, mPatchMeshes[i]);

        delete[] mPatchMeshes;
    }

    mPatchMeshes = GenerateMeshes(mPatches, mNumPatches);
    GeneratePatchColors();
    mNumMeshes = mNumPatches;
    mPatchSize = mPatches[0].numBlades;
    mPatches = nullptr;
}

void Grass::Update()
{
    if (!mPatchMeshes)
        return;

    for (int i = 0; i < mNumMeshes; i++)
    {
        physicalDescriptors[0].storage = MeshGetStorageBuffer(mPatchMeshes[i], 0);
        physicalDescriptors[1].storage = MeshGetStorageBuffer(mPatchMeshes[i], 1);
        physicalDescriptors[2].storage = MeshGetStorageBuffer(mPatchMeshes[i], 2);
        physicalDescriptors[3].storage = MeshGetStorageBuffer(mPatchMeshes[i], 3);
        PipelineUpdateDescriptor(mGraphics, mPhysicalPipeline, physicalDescriptors[0]);
        PipelineUpdateDescriptor(mGraphics, mPhysicalPipeline, physicalDescriptors[1]);
        PipelineUpdateDescriptor(mGraphics, mPhysicalPipeline, physicalDescriptors[2]);
        PipelineUpdateDescriptor(mGraphics, mPhysicalPipeline, physicalDescriptors[3]);

        PipelineComputeDispatch(mGraphics, mPhysicalPipeline, mPatchSize / 32, 1, 1);
    }
}

void Grass::Draw(Matrix projection, Matrix view, bool showPatches)
{
    if (!mPatchMeshes)
        return;

    mMatrixUniforms.projection = projection;
    mMatrixUniforms.view = view;
    mMatrixUniforms.world = MatrixIdentity();
    UniformBufferSetData(mMatrixUniformBuffer, &mMatrixUniforms, sizeof(MatrixUniforms));

    mFragmentUniforms.showPatches = showPatches;
    UniformBufferSetData(mFragmentUniformBuffer, &mFragmentUniforms, sizeof(FragmentUniforms));

    PipelineBind(mGraphics, mGrassPipeline);
    for (int i = 0; i < mNumMeshes; i++)
        MeshDraw(mGraphics, mPatchMeshes[i]);
}

bool Grass::DoneGenerating()
{
    mDoneGeneratingLock.lock();
    bool done = mDoneGenerating;
    mDoneGeneratingLock.unlock();

    return done;
}

void Grass::InitPipelines()
{
    UniformBufferCreateInfo bufferInfo;
    bufferInfo.binding = 0;
    bufferInfo.size = sizeof(MatrixUniforms);
    UniformBufferCreate(mGraphics, &bufferInfo, &mMatrixUniformBuffer);
    bufferInfo.size = sizeof(FragmentUniforms);
    UniformBufferCreate(mGraphics, &bufferInfo, &mFragmentUniformBuffer);

    grassDescriptors[0].uniform = mMatrixUniformBuffer;
    grassDescriptors[1].uniform = mFragmentUniformBuffer;

    int size = 0;
    PipelineCreateInfo grassPipeline;
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
    grassPipeline.bindingCount = 4;
    grassPipeline.pBindings = bindings;
    grassPipeline.attributeCount = 4;
    grassPipeline.pAttributes = attributes;
    grassPipeline.descriptorCount = 2;
    grassPipeline.pDescriptors = grassDescriptors;

    if (!PipelineCreate(mGraphics, &grassPipeline, &mGrassPipeline))
        WriteError(1, "Failed to create grass pipeline");

    PipelineComputeCreateInfo physicalPipeline;
    physicalPipeline.pComputeShaderCode = (unsigned int *)FileReadBytes("shaders/bin/GrassPhysical.comp", &size);
    physicalPipeline.computeShaderSize = size;
    physicalPipeline.descriptorCount = 4;
    physicalPipeline.pDescriptors = physicalDescriptors;

    if (!PipelineComputeCreate(mGraphics, &physicalPipeline, &mPhysicalPipeline))
        WriteError(1, "Failed to create physical compute pipeline");
}

void Grass::GenerationThread(Model *pModel, float density, int patchSize, float bladeWidth, float bladeHeight)
{
    float area = pModel->GetArea();
    int temp = (int)(area * BLADES_PER_UNIT * density);

    int bladeCount = temp + (patchSize - temp % patchSize);
    int numPatches = bladeCount / patchSize;

    Vec3 *normals = new Vec3[bladeCount];
    Vec3 *points = GeneratePoints(pModel, bladeCount, normals);

    Cluster clustering(numPatches, patchSize, 30);
    int *labels = clustering.Fit(points, bladeCount);

    GrassPatch *patches = GeneratePatches(points, normals, labels, numPatches, patchSize, bladeWidth, bladeHeight);

    if (mPatches)
    {
        for (int i = 0; i < mNumPatches; i++)
        {
            delete[] mPatches[i].v0;
            delete[] mPatches[i].v1;
            delete[] mPatches[i].v2;
            delete[] mPatches[i].attribs;
        }

        delete[] mPatches;
    }

    mPatches = patches;

    delete[] labels;
    delete[] points;
    delete[] normals;

    mNumPatchesLock.lock();
    mNumPatches = numPatches;
    mNumPatchesLock.unlock();

    mDoneGeneratingLock.lock();
    mDoneGenerating = true;
    mDoneGeneratingLock.unlock();
}

Vec3 *Grass::GeneratePoints(Model *pModel, int bladeCount, Vec3 *pNormals)
{
    Vec3 *points = new Vec3[bladeCount];
    int triangles = pModel->NumTriangles();

    std::uniform_real_distribution<float> indexDistribution(0, (float)triangles);
    std::uniform_real_distribution<float> positionDistribution(0, 1);

    for (int i = 0; i < bladeCount; i++)
    {
        int rIndex = (int)indexDistribution(mGenerator);
        Triangle rTriangle = pModel->GetTriangle(rIndex);

        float r1 = positionDistribution(mGenerator);
        float r2 = positionDistribution(mGenerator);

        float rootR1 = sqrtf(r1);

        points[i].x = (1 - rootR1) * rTriangle.a.position.x + (rootR1 * (1 - r2)) * rTriangle.b.position.x + (rootR1 * r2) * rTriangle.c.position.x;
        points[i].y = (1 - rootR1) * rTriangle.a.position.y + (rootR1 * (1 - r2)) * rTriangle.b.position.y + (rootR1 * r2) * rTriangle.c.position.y;
        points[i].z = (1 - rootR1) * rTriangle.a.position.z + (rootR1 * (1 - r2)) * rTriangle.b.position.z + (rootR1 * r2) * rTriangle.c.position.z;

        pNormals[i] = rTriangle.a.normal;
    }

    return points;
}

Grass::GrassPatch *Grass::GeneratePatches(Vec3 *pPoints, Vec3 *pNormals, int *patchLabels, int patchCount, int patchSize, float width, float height)
{
    GrassPatch *patches = new GrassPatch[patchCount];
    std::uniform_real_distribution<float> rotationDistribution(0, 2 * M_PI);

    for (int i = 0; i < patchCount; i++)
    {
        patches[i].v0 = new Vec4[patchSize];
        patches[i].v1 = new Vec4[patchSize];
        patches[i].v2 = new Vec4[patchSize];
        patches[i].attribs = new Vec4[patchSize];
        patches[i].numBlades = 0;
        patches[i].patch = i;
    }

    for (int i = 0; i < patchCount * patchSize; i++)
    {
        int patch = patchLabels[i];
        int count = patches[patch].numBlades;
        Vec3 pos = pPoints[i];
        Vec3 up = pNormals[i];

        patches[patch].v0[count] = {pos.x, pos.y, pos.z, rotationDistribution(mGenerator)};
        patches[patch].v1[count] = {0, 0, 0, width};
        patches[patch].v2[count] = {0, 0, 0, height};
        patches[patch].attribs[count] = {up.x, up.y, up.z, 0};
        patches[patch].numBlades += 1;
    }

    return patches;
}

Mesh *Grass::GenerateMeshes(GrassPatch *pPatches, int patchCount)
{
    unsigned int strides[4] = {sizeof(Vec4), sizeof(Vec4), sizeof(Vec4), sizeof(Vec4)};

    mGrassCreateInfo.bufferCount = 4;
    mGrassCreateInfo.strides = strides;

    Mesh *meshes = new Mesh[patchCount];

    for (int i = 0; i < patchCount; i++)
    {
        mGrassCreateInfo.vertexCount = pPatches[i].numBlades;
        mGrassCreateInfo.ppData = (void **)&pPatches[i];

        if (!MeshCreate(mGraphics, &mGrassCreateInfo, &meshes[i]))
            return nullptr;
    }

    return meshes;
}

void Grass::GeneratePatchColors()
{
    std::uniform_real_distribution<float> color(0, 1);

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
}
//
// Created by Luis on 3/26/2024.
//

#ifndef GRASSRENDERER_GRASS_H
#define GRASSRENDERER_GRASS_H

#include "Model.h"

#include <ShlibVK/ShlibVK.h>
#include <random>
#include <mutex>
#include <thread>

#ifndef BLADES_PER_UNIT
#define BLADES_PER_UNIT 100
#endif
#define MAX_COLORS 256

class Grass
{
private:
    struct GrassPatch
    {
        Vec4 *v0;
        Vec4 *v1;
        Vec4 *v2;
        Vec4 *attribs;
        int patch;
        int numBlades;
    };

    struct MatrixUniforms
    {
        Matrix projection;
        Matrix view;
        Matrix world;
    };

    struct FragmentUniforms
    {
        Vec4 colors[MAX_COLORS];
        int showPatches;
    };

private:
    Graphics &mGraphics;
    Mesh *mPatchMeshes = nullptr;
    Pipeline mGrassPipeline = nullptr, mPhysicalPipeline = nullptr;
    MatrixUniforms mMatrixUniforms = {};
    FragmentUniforms mFragmentUniforms = {};
    UniformBuffer mMatrixUniformBuffer = nullptr, mFragmentUniformBuffer = nullptr;
    int mNumMeshes, mNumPatches, mPatchSize;
    GrassPatch *mPatches = nullptr;

    MeshCreateInfo mGrassCreateInfo;
    std::default_random_engine mGenerator;

    std::thread mGenerationThread;
    std::mutex mDoneGeneratingLock, mNumPatchesLock;
    bool mDoneGenerating = true;


public:
    explicit Grass(Graphics &graphics);
    ~Grass();

    void Generate(Model *pModel, float density, int patchSize, float bladeWidth, float bladeHeight);
    void FinishGeneration();
    void Update();
    void Draw(Matrix projection, Matrix view, bool showPatches);

    bool DoneGenerating();

private:
    void InitPipelines();

    void GenerationThread(Model *pModel, float density, int patchSize, float bladeWidth, float bladeHeight);
    Vec3 *GeneratePoints(Model *pModel, int bladeCount, Vec3 *pNormals);
    GrassPatch *GeneratePatches(Vec3 *pPoints, Vec3 *pNormals, int *patchLabels, int patchCount, int patchSize, float width, float height);
    Mesh *GenerateMeshes(GrassPatch *pPatches, int patchCount);
    void GeneratePatchColors();
    //void GenerateMesh();
};

#endif //GRASSRENDERER_GRASS_H

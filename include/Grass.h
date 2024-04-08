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

struct GrassBlade
{
    Vec3 pos;
    Vec3 norm;
    Vec3 dir;

    int patch;
};

class Grass
{
private:
    struct GrassBladeVertex
    {
        Vec3 position;
        Vec3 normal;
        Vec3 direction;
        float patch;
    };

private:
    Mesh mGrassMesh = nullptr;
    Program mGrassShader = nullptr;
    UniformBuffer mColorBuffer = nullptr;
    GrassBlade *mGrassBlades = nullptr;
    Vec4 *mPatchColors = nullptr;
    int mNumBlades, mPatchSize, mNumPatches;

    MeshCreateInfo mGrassCreateInfo;
    std::default_random_engine mGenerator;

    std::thread mGenerationThread;
    std::mutex mColorBufferMutex, mGrassMeshMutex;
    bool mDoneGenerating = true;

    void GenerationThread(Model *pModel);
    void GenerateBlades(Model *pModel);
    void GeneratePatches();
    void GenerateMesh();
public:
    Grass();
    ~Grass();

    void Generate(Model *pModel, int numBlades, int patchSize);
    void FinishGeneration();
    void Draw(Matrix projection, Matrix view, float height, float width, bool showPatches);

    bool DoneGenerating();
};

#endif //GRASSRENDERER_GRASS_H

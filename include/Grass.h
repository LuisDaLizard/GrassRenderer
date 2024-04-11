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

#define MAX_COLORS 256

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

    struct VertexUniforms
    {
        float width;
        float height;
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
    Mesh mGrassMesh = nullptr;
    Pipeline mGrassPipeline = nullptr;
    VertexUniforms mVertexUniforms;
    MatrixUniforms mMatrixUniforms;
    FragmentUniforms mFragmentUniforms;
    UniformBuffer mVertexUniformBuffer = nullptr, mMatrixUniformBuffer = nullptr, mFragmentUniformBuffer = nullptr;
    GrassBlade *mGrassBlades = nullptr;
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
    Grass(Graphics &graphics);
    ~Grass();

    void Generate(Model *pModel, int numBlades, int patchSize);
    void FinishGeneration();
    void Draw(Matrix projection, Matrix view, float height, float width, bool showPatches);

    bool DoneGenerating();
};

#endif //GRASSRENDERER_GRASS_H

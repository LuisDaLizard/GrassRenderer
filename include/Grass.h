//
// Created by Luis on 3/26/2024.
//

#ifndef GRASSRENDERER_GRASS_H
#define GRASSRENDERER_GRASS_H

#include "Model.h"

#include <ShlibGraphics/ShlibGraphics.h>
#include <random>

struct GrassBlade
{
    Vec3 v0;
    Vec3 v1;
    Vec3 v2;
};

class Grass
{
private:
    struct GrassBladeVertex
    {
        Vec3 position;
        Vec3 normal;
    };

private:
    Mesh mGrassMesh = nullptr;
    GrassBlade *mGrassBlades = nullptr;
    int mNumBlades, mPatchSize;

    MeshCreateInfo mGrassCreateInfo;
    std::default_random_engine mGenerator;

    void GenerateBlades(Model *pModel);
    void GeneratePatches();
public:
    Grass(int patchSize = 32);
    ~Grass();

    void Generate(Model *pModel, int numBlades);

    void Draw();
};

#endif //GRASSRENDERER_GRASS_H

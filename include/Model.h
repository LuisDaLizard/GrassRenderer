//
// Created by Luis on 3/26/2024.
//

#ifndef GRASSRENDERER_MODEL_H
#define GRASSRENDERER_MODEL_H

#include <ShlibVK/ShlibVK.h>

struct Vertex
{
    Vec3 position;
    Vec3 normal;
};

struct Triangle
{
    Vertex a;
    Vertex b;
    Vertex c;
};

class Model
{
private:
    Graphics &mGraphics;
    Pipeline &mPipeline;
    Mesh mMesh;
    Triangle *mTriangles;
    int mNumTriangles;

    bool GenerateMesh();
public:
    explicit Model(Graphics &graphics, Pipeline &pipeline, const char *pFilename);
    Model(Graphics &graphics, Pipeline &pipeline, const Triangle *pTriangles, int numTriangles);
    ~Model();

    Triangle GetTriangle(int index) const;
    int NumTriangles() const;

    void Draw() const;
};

namespace Meshes
{
    const Triangle PlaneMesh[] =
            {
                    {{{1, 0, 1}, {0, 1, 0}}, {{1, 0, -1}, {0, 1, 0}}, {{-1, 0, -1}, {0, 1, 0}}},
                    {{{-1, 0, -1}, {0, 1, 0}}, {{-1, 0, 1}, {0, 1, 0}}, {{1, 0, 1}, {0, 1, 0}}},
            };
}

#endif //GRASSRENDERER_MODEL_H

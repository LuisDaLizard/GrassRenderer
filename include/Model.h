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

    static float GetArea(Triangle triangle)
    {
        Vec3 a = triangle.a.position;
        Vec3 b = triangle.b.position;
        Vec3 c = triangle.c.position;

        Vec3 edgeA = {b.x - a.x, b.y - a.y, b.z - a.z};
        Vec3 edgeB = {c.x - a.x, c.y - a.y, c.z - a.z};

        return 0.5f * Vec3Magnitude(Vec3Cross(edgeA, edgeB));
    }
};

class Model
{
private:
    Graphics &mGraphics;
    Pipeline &mPipeline;
    Mesh mMesh;
    Triangle *mTriangles;
    int mNumTriangles;
    float mArea;

public:
    explicit Model(Graphics &graphics, Pipeline &pipeline, const char *pFilename);
    Model(Graphics &graphics, Pipeline &pipeline, const Triangle *pTriangles, int numTriangles);
    ~Model();

    Triangle GetTriangle(int index) const;
    int NumTriangles() const;

    float GetArea() const;

    void Draw() const;

private:
    bool GenerateMesh();
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

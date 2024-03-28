//
// Created by Luis on 3/26/2024.
//

#ifndef GRASSRENDERER_MODEL_H
#define GRASSRENDERER_MODEL_H

#include <ShlibGraphics/ShlibGraphics.h>

struct Vertex
{
    Vec3 position;
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
    Triangle *mTriangles;
    int mNumTriangles;

public:
    Model(const Triangle *pTriangles, int numTriangles);
    ~Model();

    Triangle GetTriangle(int index) const;
    int NumTriangles() const;
};

namespace Meshes
{
    const Triangle PlaneMesh[] =
            {
                    {{{1, 0, 1}}, {{1, 0, -1}}, {{-1, 0, -1}}},
                    {{{-1, 0, -1}}, {{-1, 0, 1}}, {{1, 0, 1}}},
            };
}

#endif //GRASSRENDERER_MODEL_H

//
// Created by Luis on 3/26/2024.
//

#include "Model.h"

#include <cstring>

Model::Model(const Triangle *pTriangles, int numTriangles)
    : mNumTriangles(numTriangles)
{
    mTriangles = new Triangle[numTriangles];
    memcpy(mTriangles, pTriangles, mNumTriangles * sizeof(Triangle));
}

Model::~Model()
{
    delete[] mTriangles;
}

Triangle Model::GetTriangle(int index) const
{
    if (index < 0 || index >= mNumTriangles)
        return {};
    return mTriangles[index];
}

int Model::NumTriangles() const
{
    return mNumTriangles;
}

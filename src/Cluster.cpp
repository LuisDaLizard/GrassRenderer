#include "Cluster.h"
#include <cfloat>


Cluster::Cluster(int numClusters, int maxClusterSize, int maxIterations)
    : mNumClusters(numClusters), mMaxClusterSize(maxClusterSize), mMaxIterations(maxIterations), mCentroids(nullptr), mGenerator(std::random_device{}())
{ }

Cluster::~Cluster()
{
    delete[] mCentroids;
}

int *Cluster::Fit(Vec3 *points, int numPoints)
{
    if (mCentroids)
        return nullptr;

    std::uniform_int_distribution<int> indexGeneration(0, numPoints);

    mCentroids = new Vec3[mNumClusters];
    for (int i = 0; i < mNumClusters; i++)
        mCentroids[i] = points[indexGeneration(mGenerator)];

    int *labels = new int[numPoints];

    for (int iter = 0; iter < mMaxIterations; iter++)
    {
        int *newLabels = new int[numPoints];
        int *clusterSizes = new int[mNumClusters];
        memset(clusterSizes, 0, sizeof(int) * mNumClusters);
        for (int i = 0; i < numPoints; i++)
            newLabels[i] = AssignCluster(points[i], clusterSizes);

        // Update Centroids
        Vec3 *newCentroids = new Vec3[mNumClusters];
        memset(newCentroids, 0, sizeof(Vec3) * mNumClusters);

        for (int i = 0; i < numPoints; i++)
        {
            int cluster = newLabels[i];

            newCentroids[cluster].x += points[i].x;
            newCentroids[cluster].y += points[i].y;
            newCentroids[cluster].z += points[i].z;
        }

        for (int i = 0; i < mNumClusters; i++)
        {
            newCentroids[i].x /= (float)clusterSizes[i];
            newCentroids[i].y /= (float)clusterSizes[i];
            newCentroids[i].z /= (float)clusterSizes[i];
        }

        delete[] clusterSizes;

        // Check for Convergence
        bool equal = true;
        for (int i = 0; i < mNumClusters; i++)
        {
            if (mCentroids[i].x != newCentroids[i].x || mCentroids[i].y != newCentroids[i].y || mCentroids[i].z != newCentroids[i].z)
            {
                equal = false;
                break;
            }
        }

        if (equal)
        {
            delete[] labels;
            labels = newLabels;
            break;
        }

        delete[] mCentroids;
        delete[] labels;
        mCentroids = newCentroids;
        labels = newLabels;
    }

    return labels;
}

int Cluster::AssignCluster(Vec3 point, int *clusterSizes)
{
    double minDist = DBL_MAX;
    int cluster = -1;

    for (int i = 0; i < mNumClusters; i++)
    {
        double dist = FindDistance(point, mCentroids[i]);
        if (dist < minDist && clusterSizes[i] < mMaxClusterSize)
        {
            minDist = dist;
            cluster = i;
        }
    }

    clusterSizes[cluster]++;
    return cluster;
}

double Cluster::FindDistance(Vec3 p1, Vec3 p2)
{
    double sum = pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2) + pow(p1.z - p2.z, 2);
    return sqrt(sum);
}
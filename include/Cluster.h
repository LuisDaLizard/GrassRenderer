//
// Created by Luis on 4/14/2024.
//

#ifndef GRASSRENDERER_CLUSTER_H
#define GRASSRENDERER_CLUSTER_H

#include <ShlibVK/ShlibVK.h>
#include <random>

class Cluster
{
private:
    int mNumClusters;
    int mMaxClusterSize;
    int mMaxIterations;
    Vec3 *mCentroids;
    std::default_random_engine mGenerator;

public:
    explicit Cluster(int numClusters, int maxClusterSize, int maxIterations = 300);
    ~Cluster();

    int *Fit(Vec3 *points, int numPoints);

private:
    int AssignCluster(Vec3 point, int *clusterSizes);
    double FindDistance(Vec3 p1, Vec3 p2);

};


#endif //GRASSRENDERER_CLUSTER_H

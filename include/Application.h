//
// Created by Luis on 3/26/2024.
//

#ifndef GRASSRENDERER_APPLICATION_H
#define GRASSRENDERER_APPLICATION_H

#include <ShlibVK/ShlibVK.h>
#include "Model.h"
#include "Grass.h"

class Application
{
private:
    struct UniformMatrices
    {
        Matrix projection;
        Matrix view;
        Matrix world;
    };

private:
    Window mWindow;
    Graphics mGraphics;
    UniformBuffer mUniformMatrices;
    Pipeline mModelPipeline;
    Model *mModel;
    double mDeltaTime;

    // Grass Related Variables
    Grass *mGrass;
    int mPatchSize = 32;
    int mNumBlades = 0;
    float mBladeHeight = 0.5f;
    float mBladeWidth = 0.25f;
    bool mShowPatches = false;

    // Camera Related Variables
    float mCameraSensitivity = 0.1f;
    float mZoomSensitivity = 0.01f;
    float mCameraYaw = 0, mCameraPitch = 10;
    float mCameraRadius = 5;
    double mGenerationTime = 0;
    Vec2 mMouse, mPrevMouse, mGrassGenerationWindowSize, mGrassGenerationWindowPos;
    Vec3 mCameraPos = {0, 0, 0}, mCameraTarget = {0, 0, 0}, mLightPos = {0, 100, 100};
    UniformMatrices mCameraMatrices;

    void InitWindow();
    void InitGraphics();
    void InitGui();
    void InitUniforms();
    void InitPipelines();
    void InitModels();

    void UpdateGui();
    void DrawGui();

    void UpdateCamera();

    void Cleanup();
public:
    Application() = default;
    ~Application() = default;

    void Run();
};

#endif //GRASSRENDERER_APPLICATION_H

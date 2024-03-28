//
// Created by Luis on 3/26/2024.
//

#ifndef GRASSRENDERER_APPLICATION_H
#define GRASSRENDERER_APPLICATION_H

#include <ShlibGraphics/ShlibGraphics.h>
#include "Model.h"
#include "Grass.h"

class Application
{
private:
    Window mWindow;
    Model *mModel;
    double mDeltaTime;

    // Grass Related Variables
    Grass *mGrass;
    int mPatchSize = 32;
    int mNumBlades = 0;
    float mBladeHeight = 0.5f;
    bool mShowPatches = false;

    // Camera Related Variables
    float mCameraSensitivity = 0.1f;
    float mZoomSensitivity = 0.01f;
    float mCameraYaw = 0, mCameraPitch = 0;
    float mCameraRadius = 5;
    double mGenerationTime = 0;
    Vec2 mMouse, mPrevMouse, mGrassGenerationWindowSize, mGrassGenerationWindowPos;
    Vec3 mCameraPos = {0, 0, 0}, mCameraTarget = {0, 0, 0};
    Matrix mCameraProjection, mCameraView;

    void InitWindow();
    void InitGui();
    void InitPrograms();
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

//
// Created by Luis on 3/26/2024.
//

#include "Application.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void Application::Run()
{
    InitWindow();
    InitGui();
    InitPrograms();
    InitModels();

    double start = GetTime();
    double end = start;

    while(!WindowShouldClose(mWindow))
    {
        mDeltaTime = end - start;
        start = GetTime();

        WindowPollEvents(mWindow);

        UpdateGui();
        UpdateCamera();

        GLClear();
        mGrass->Draw(mCameraProjection, mCameraView, mBladeHeight, mShowPatches);

        DrawGui();

        WindowSwapBuffers(mWindow);
        end = GetTime();
    }

    Cleanup();
}

void Application::InitWindow()
{
    WindowCreateInfo createInfo = {  };
    createInfo.glVersionMajor = 4;
    createInfo.glVersionMinor = 0;
    createInfo.width = 800;
    createInfo.height = 600;
    createInfo.pTitle = "Grass Renderer";

    if (!WindowInit(&createInfo, &mWindow))
        WriteError(1, "Unable to create window!");
}

void Application::InitGui()
{
    ImGui::CreateContext();
    ImGui::StyleColorsLight();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)mWindow->pHandle, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Application::InitPrograms()
{
}

void Application::InitModels()
{
    mGrass = new Grass();
    mModel = new Model(Meshes::PlaneMesh, 2);
}

void Application::UpdateGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (mGrass->DoneGenerating())
        mGrass->FinishGeneration();
    else
        mGenerationTime += mDeltaTime;

    {
        ImGui::SetNextWindowPos({10, 10});
        ImGui::SetNextWindowSize({0, 0}, 0);
        ImGui::Begin("Grass Editor");

        if (ImGui::CollapsingHeader("Grass Generation"))
        {
            ImGui::InputInt("Patch Size", &mPatchSize, 32, 32);

            if (mPatchSize % 32 != 0)
                mPatchSize += 32 - (mPatchSize % 32);
            if (mPatchSize <= 0)
                mPatchSize = 32;

            ImGui::InputInt("Number of blades", &mNumBlades, mPatchSize, mPatchSize);

            if (mNumBlades % mPatchSize != 0)
                mNumBlades += mPatchSize - (mNumBlades % mPatchSize);
            if (mNumBlades <= 0)
                mNumBlades = 32;

            if (ImGui::Button("Generate"))
            {
                mGrass->Generate(mModel, mNumBlades, mPatchSize);
                mGenerationTime = 0;
            }
            ImGui::SameLine();
            ImGui::Text("%.4fs", mGenerationTime);
        }

        if (ImGui::CollapsingHeader("Grass Variables"))
        {
            ImGui::SliderFloat("Height", &mBladeHeight, 0.01f, 1.0f);
            ImGui::Checkbox("Show Patches", &mShowPatches);
        }

        ImVec2 size = ImGui::GetWindowSize();
        mGrassGenerationWindowSize.x = size.x;
        mGrassGenerationWindowSize.y = size.y;

        ImVec2 pos = ImGui::GetWindowPos();
        mGrassGenerationWindowPos.x = pos.x;
        mGrassGenerationWindowPos.y = pos.y;

        ImGui::End();
    }

    ImGui::Render();
}

void Application::DrawGui()
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::UpdateCamera()
{
    mPrevMouse = mMouse;
    WindowGetMousePos(mWindow, &mMouse.x, &mMouse.y);

    if (mMouse.x >= mGrassGenerationWindowPos.x && mMouse.x <= mGrassGenerationWindowPos.x + mGrassGenerationWindowSize.x &&
        mMouse.y >= mGrassGenerationWindowPos.y && mMouse.y <= mGrassGenerationWindowPos.y + mGrassGenerationWindowSize.y)
        return;

    if (WindowIsMouseButtonDown(mWindow, MOUSE_LEFT))
    {
        Vec2 mouseDelta;
        mouseDelta.x = mMouse.x - mPrevMouse.x;
        mouseDelta.y = mMouse.y - mPrevMouse.y;

        mCameraYaw -= mouseDelta.x * mCameraSensitivity;
        mCameraPitch += mouseDelta.y * mCameraSensitivity;
        mCameraPitch = CLAMP(-89.0f, mCameraPitch, 89.0f);
    }

    if (WindowIsMouseButtonDown(mWindow, MOUSE_RIGHT))
    {
        float mouseYDelta = mMouse.y - mPrevMouse.y;

        mCameraRadius += (float)(mouseYDelta * mZoomSensitivity);
        mCameraRadius = MAX(mCameraRadius, 0.01f);
    }

    mCameraPos.x = sinf(mCameraYaw * DEG2RADF) * cosf(mCameraPitch * DEG2RADF) * mCameraRadius;
    mCameraPos.y = sinf(mCameraPitch * DEG2RADF) * mCameraRadius + mCameraTarget.y;
    mCameraPos.z = cosf(mCameraYaw * DEG2RADF) * cosf(mCameraPitch * DEG2RADF) * mCameraRadius;

    mCameraView = MatrixLookAt(mCameraPos, mCameraTarget, {0, 1, 0});
    mCameraProjection = MatrixPerspective((float)mWindow->width / (float)mWindow->height, 45, 0.01f, 1000.0f);
}

void Application::Cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    WindowFree(mWindow);
}

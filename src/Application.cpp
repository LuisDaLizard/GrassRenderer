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

    while(!WindowShouldClose(mWindow))
    {
        WindowPollEvents(mWindow);

        UpdateCamera();
        UpdateGui();

        GLClear();

        ProgramUse(mGrassShader);
        ProgramUploadMatrix(mGrassShader, ProgramUniformLocation(mGrassShader, "uWorld"), MatrixIdentity());
        ProgramUploadMatrix(mGrassShader, ProgramUniformLocation(mGrassShader, "uView"), mCameraView);
        ProgramUploadMatrix(mGrassShader, ProgramUniformLocation(mGrassShader, "uProjection"), mCameraProjection);
        ProgramUploadFloat(mGrassShader, ProgramUniformLocation(mGrassShader, "uHeight"), mBladeHeight);
        mGrass.Draw();

        DrawGui();

        WindowSwapBuffers(mWindow);
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
    ProgramCreateInfo grassShader = {};
    grassShader.pVertexSource = FileReadText("shaders/Grass.vert");
    grassShader.pGeometrySource = FileReadText("shaders/Grass.geom");
    grassShader.pFragmentSource = FileReadText("shaders/Grass.frag");

    if (!ProgramCreate(&grassShader, &mGrassShader))
        WriteError(1, "Unable to create grass shader");
}

void Application::InitModels()
{
    mModel = new Model(Meshes::PlaneMesh, 2);
}

void Application::UpdateGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::SetNextWindowPos({10, 10});
        ImGui::SetNextWindowSize({0, 0}, 0);
        ImGui::Begin("Grass Editor");

        if (ImGui::CollapsingHeader("Grass Generation"))
        {
            ImGui::SliderInt("Blades", &mNumBlades, 0, 10000);

            if (ImGui::Button("Generate"))
                mGrass.Generate(mModel, mNumBlades);
        }

        if (ImGui::CollapsingHeader("Grass Variables"))
        {
            ImGui::SliderFloat("Height", &mBladeHeight, 0.01f, 1.0f);
        }

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

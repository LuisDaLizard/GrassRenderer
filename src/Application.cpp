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

    while(!WindowShouldClose(mWindow))
    {
        WindowPollEvents(mWindow);

        UpdateGui();

        GLClear();

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

void Application::UpdateGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("Hello World!");

        ImGui::Text("This is some useful text.");

        ImGui::End();
    }

    ImGui::Render();
}

void Application::DrawGui()
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::Cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    WindowFree(mWindow);
}

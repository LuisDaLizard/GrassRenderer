#include <ShlibGraphics.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

Window gWindow;

void InitWindow();
void InitImGui();

void UpdateGUI();
void DrawGUI();

int main()
{
    InitWindow();
    InitImGui();

    while(!WindowShouldClose(gWindow))
    {
        WindowPollEvents(gWindow);

        UpdateGUI();

        GLClear();

        DrawGUI();

        WindowSwapBuffers(gWindow);
    }

    return 0;
}

void InitWindow()
{
    WindowCreateInfo createInfo = {  };
    createInfo.glVersionMajor = 4;
    createInfo.glVersionMinor = 0;
    createInfo.width = 800;
    createInfo.height = 600;
    createInfo.pTitle = "Grass Renderer";

    if (!WindowInit(&createInfo, &gWindow))
        WriteError(1, "Unable to create window!");
}

void InitImGui()
{
    ImGui::CreateContext();
    ImGui::StyleColorsLight();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)gWindow->pHandle, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void UpdateGUI()
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

void DrawGUI()
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
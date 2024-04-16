#include "Application.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

void Application::Run()
{
    InitWindow();
    InitGraphics();
    InitUniforms();
    InitPipelines();
    InitGui();
    InitModels();

    double start = GetTime();
    double end = start;

    while(!WindowShouldClose(mWindow))
    {
        mDeltaTime = end - start;
        start = GetTime();

        WindowPollEvents(mWindow);

        mGrass->Update();

        GraphicsBeginRenderPass(mGraphics);

        UpdateGui();
        UpdateCamera();

        mModel->Draw();
        mGrass->Draw(mCameraMatrices.projection, mCameraMatrices.view, mShowPatches);

        DrawGui();

        GraphicsEndRenderPass(mGraphics);
        end = GetTime();
    }

    Cleanup();
}

void Application::InitWindow()
{
    WindowCreateInfo createInfo = {  };
    createInfo.width = 800;
    createInfo.height = 600;
    createInfo.pTitle = "Grass Renderer";

    if (!WindowCreate(&createInfo, &mWindow))
        WriteError(1, "Unable to create window!");
}

void Application::InitGraphics()
{
    GraphicsCreateInfo createInfo = { };
    createInfo.window = mWindow;
    createInfo.debug = true;
    createInfo.pAppName = "Grass Renderer";
    createInfo.pEngineName = "Grass Engine";

    if (!GraphicsCreate(&createInfo, &mGraphics))
        WriteError(1, "Failed to initialize graphics!");
}

void Application::InitGui()
{
    ImGui::CreateContext();
    ImGui::StyleColorsLight();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    ImGui_ImplGlfw_InitForVulkan((GLFWwindow *)mWindow->pHandle, true);
    ImGui_ImplVulkan_InitInfo initInfo = { };
    initInfo.Instance = (VkInstance)(mGraphics->vkInstance);
    initInfo.PhysicalDevice = (VkPhysicalDevice)(mGraphics->vkPhysicalDevice);
    initInfo.Device = (VkDevice)(mGraphics->vkDevice);
    initInfo.QueueFamily = mGraphics->vkGraphicsQueueIndex;
    initInfo.Queue = (VkQueue)(mGraphics->vkGraphicsQueue);
    initInfo.DescriptorPool = (VkDescriptorPool)(mGraphics->vkDescriptorPool);
    initInfo.RenderPass = (VkRenderPass)(mGraphics->vkRenderPass);
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = mGraphics->vkSwapChainImageCount;
    initInfo.MSAASamples = (VkSampleCountFlagBits)(1);
    if (!ImGui_ImplVulkan_Init(&initInfo))
        WriteError(1, "Failed to initialize GUI");

    if (!ImGui_ImplVulkan_CreateFontsTexture())
        WriteError(1, "Failed to create and upload font textures");
}

void Application::InitUniforms()
{
    mCameraMatrices.projection = MatrixTranspose(MatrixPerspective((float)mWindow->width / (float)mWindow->height, 45.0f, 0.01f, 1000.0f));
    mCameraMatrices.view = MatrixTranspose(MatrixLookAt({0, 3, 4}, {0, 0, 0}, {0, -1, 0}));
    mCameraMatrices.world = MatrixIdentity();

    UniformBufferCreateInfo bufferInfo = { 0 };
    bufferInfo.binding = 0;
    bufferInfo.size = sizeof(UniformMatrices);

    UniformBufferCreate(mGraphics, &bufferInfo, &mUniformMatrices);
    UniformBufferSetData(mUniformMatrices, &mCameraMatrices, sizeof(UniformMatrices));
}

void Application::InitPipelines()
{
    Descriptor matrices = { 0 };
    matrices.location = 0;
    matrices.type = DESCRIPTOR_TYPE_UNIFORM;
    matrices.count = 1;
    matrices.stage = STAGE_VERTEX;
    matrices.uniform = mUniformMatrices;

    InputBinding binding = { 0 };
    binding.binding = 0;
    binding.stride = sizeof(Vertex);

    Attribute position = { 0 };
    position.location = 0;
    position.offset = 0;
    position.components = 3;

    Attribute normal = { 0 };
    normal.location = 1;
    normal.offset = sizeof(Vec3);
    normal.components = 3;

    Attribute attribs[2] = {position, normal};

    int size = 0;
    PipelineCreateInfo modelPipeline = { };
    modelPipeline.topology = TOPOLOGY_TRIANGLE_LIST;
    modelPipeline.pVertexShaderCode = (unsigned int *)FileReadBytes("shaders/bin/Model.vert", &size);
    modelPipeline.vertexShaderSize = size;
    modelPipeline.pFragmentShaderCode = (unsigned int *)FileReadBytes("shaders/bin/Model.frag", &size);
    modelPipeline.fragmentShaderSize = size;
    modelPipeline.bindingCount = 1;
    modelPipeline.pBindings = &binding;
    modelPipeline.attributeCount = 2;
    modelPipeline.pAttributes = attribs;
    modelPipeline.descriptorCount = 1;
    modelPipeline.pDescriptors = &matrices;

    if (!PipelineCreate(mGraphics, &modelPipeline, &mModelPipeline))
        WriteError(1, "Unable to load model shader");
}

void Application::InitModels()
{
    mGrass = new Grass(mGraphics);
    mModel = new Model(mGraphics, mModelPipeline, Meshes::PlaneMesh, 2);
}

void Application::UpdateGui()
{
    ImGui_ImplVulkan_NewFrame();
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

            ImGui::SliderFloat("Grass Density", &mGrassDensity, 0.01f, 1.0f);
            ImGui::SliderFloat("Grass Width", &mBladeWidth, 0.01f, 1.0f);
            ImGui::SliderFloat("Grass Height", &mBladeHeight, 0.01f, 2.0f);

            if (ImGui::Button("Generate"))
            {
                mGrass->Generate(mModel, mGrassDensity, mPatchSize, mBladeWidth, mBladeHeight);
                mGenerationTime = 0;
            }
            ImGui::SameLine();
            ImGui::Text("%.2fs", mGenerationTime);
        }

        if (ImGui::CollapsingHeader("Grass Variables"))
        {
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
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), (VkCommandBuffer)mGraphics->vkCommandBuffer);
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

        mCameraYaw += mouseDelta.x * mCameraSensitivity;
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

    mCameraMatrices.view = MatrixTranspose(MatrixLookAt(mCameraPos, mCameraTarget, {0, -1, 0}));
    mCameraMatrices.projection = MatrixTranspose(MatrixPerspective((float)mWindow->width / (float)mWindow->height, 45, 0.01f, 1000.0f));

    UniformBufferSetData(mUniformMatrices, &mCameraMatrices, sizeof(UniformMatrices));
}

void Application::Cleanup()
{
    delete mModel;
    delete mGrass;

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    UniformBufferDestroy(mGraphics, mUniformMatrices);
    PipelineDestroy(mGraphics, mModelPipeline);
    GraphicsDestroy(mGraphics);
    WindowDestroy(mWindow);
}

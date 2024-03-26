//
// Created by Luis on 3/26/2024.
//

#ifndef GRASSRENDERER_APPLICATION_H
#define GRASSRENDERER_APPLICATION_H

#include <ShlibGraphics/ShlibGraphics.h>

class Application
{
private:
    Window mWindow;

    void InitWindow();
    void InitGui();

    void UpdateGui();
    void DrawGui();

    void Cleanup();
public:
    Application() = default;
    ~Application() = default;

    void Run();
};

#endif //GRASSRENDERER_APPLICATION_H

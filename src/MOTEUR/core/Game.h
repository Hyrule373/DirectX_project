#pragma once
#include "Window.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Transform.h"
#include "Entity.h"


class Game {
public:
    Game();
    ~Game();

    bool Initialize(HINSTANCE hInstance, int nCmdShow);
    void Update();
    void Render();
    void Run();

    std::vector<Entity> entities; 
private:
    Window window;
    Renderer renderer;
};
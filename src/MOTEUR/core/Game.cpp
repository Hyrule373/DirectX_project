#include "pch.h"
#include "Game.h"
#include "Entity.h"
#include "Transform.h"
#include "Mesh.h"
#include "RenderableObject.h"

//////////// CONSTRUCTOR/DESTRUCTOR //////////

Game::Game() = default;

Game::~Game() {
    renderer.Cleanup();
}

//////////// INITIALIZATION //////////

bool Game::Initialize(HINSTANCE hInstance, int nCmdShow) {
    // Initialisation de la fenêtre
    if (!window.Initialize(hInstance, nCmdShow, 1920, 1080, L"Unlimited Engine <DX12>")) {
        return false;
    }

    // Initialisation du renderer
    if (!renderer.Initialize(window.GetHandle(), window.GetWidth(), window.GetHeight())) {
        return false;
    }

    // Création de la sphère
    for (int i = -2; i < 2; i++) {
        for (int j = -2; j < 2; j++) {

                Entity cube;
                cube.AddComponent<Transform>();
                cube.GetComponent<Transform>()->SetPosition(i, j, 0);
                cube.GetComponent<Transform>()->SetScale(0.3, 0.3, 0.3);

                cube.AddComponent<Mesh>();
                if (auto mesh = cube.GetComponent<Mesh>()) {
                    if (i == 1) {
                        mesh->LoadCubeGeometry(renderer.GetDevice());
                        cube.GetComponent<Transform>()->SetScale(i+2, 1, j+2);
                    }
                    else if (j == 1) {
						mesh->LoadTriangleGeometry(renderer.GetDevice());
                        cube.GetComponent<Transform>()->SetScale(i+2, j+2, 1);
					}
                    else {
                        mesh->LoadSphereGeometry(renderer.GetDevice());
                    }
                }
                entities.push_back(cube);
            
        }
    }


    return true;
}

//////////// MAIN LOOP //////////

void Game::Run() {
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                break;
            }

            Update();
            Render();
        }
    }
}

//////////// UPDATE //////////

void Game::Update() {
    static float rotation = 0.0f;
    rotation += 0.01f;

    for (auto& entity : entities) {
        if (entity.HasComponent<Transform>()) {
            entity.GetComponent<Transform>().get()->SetRotation(rotation, rotation/2, rotation*2);
        }
    }
}

//////////// RENDERING //////////

void Game::Render() {
    std::vector<RenderableObject> renderables;

    for (auto& entity : entities) {
        if (entity.HasComponent<Mesh>() && entity.HasComponent<Transform>()) {
            auto mesh = entity.GetComponent<Mesh>();
            auto transform = entity.GetComponent<Transform>();

            if (mesh && transform) {
                RenderableObject obj;
                obj.mesh = mesh;
                obj.worldMatrix = transform->GetWorldMatrix();

                // Initialiser le constant buffer pour cet objet
                obj.constantBuffer.Initialize(renderer.GetDevice());

                renderables.push_back(obj);
            }
        }
    }

    renderer.Render(renderables);
}
#include "pch.h"
//#include "App.h"
//
//// Window procedure
//LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
//    switch (message) {
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        return 0;
//    }
//    return DefWindowProc(hWnd, message, wParam, lParam);
//}
//
//// Initialize window
//void App::InitWindow(HINSTANCE hInstance, int nCmdShow) {
//    WNDCLASSEX wc = {};
//    wc.cbSize = sizeof(WNDCLASSEX);
//    wc.style = CS_HREDRAW | CS_VREDRAW;
//    wc.lpfnWndProc = WindowProc;
//    wc.hInstance = hInstance;
//    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
//    wc.lpszClassName = L"DX12WindowClass";
//
//    wc.hIcon = (HICON)LoadImage(nullptr, iconPath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
//    wc.hIconSm = (HICON)LoadImage(nullptr, iconPath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
//
//
//    RegisterClassEx(&wc);
//
//    RECT windowRect = { 0, 0, windowWidth, windowHeight };
//    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
//    hwnd = CreateWindow(
//        wc.lpszClassName,
//        L"Unity <DX12>",
//        WS_OVERLAPPEDWINDOW,
//        CW_USEDEFAULT, CW_USEDEFAULT,
//        windowRect.right - windowRect.left,
//        windowRect.bottom - windowRect.top,
//        nullptr,
//        nullptr,
//        hInstance,
//        nullptr);
//
//    ShowWindow(hwnd, nCmdShow);
//}
//
//App::~App() {
//    WaitForPreviousFrame();
//}
//
//void App::Run() {
//    MSG msg = {};
//    auto lastTime = std::chrono::high_resolution_clock::now();
//
//    while (msg.message != WM_QUIT) {
//        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }
//        else {
//            // Calcul delta time
//            auto currentTime = std::chrono::high_resolution_clock::now();
//            float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
//            lastTime = currentTime;
//
//            m_camera.Update(static_cast<float>(windowWidth) / windowHeight);
//
//            // Gestion des inputs pour changer la forme
//            if (GetAsyncKeyState('1') & 0x8000) {
//                SetShape(ShapeType::Pyramid);
//            }
//            if (GetAsyncKeyState('2') & 0x8000) {
//                SetShape(ShapeType::Cube); 
//            }
//            if (GetAsyncKeyState('3') & 0x8000) {
//                SetShape(ShapeType::Sphere);
//            }
//            if (GetAsyncKeyState('4') & 0x8000) {
//                SetShape(ShapeType::Triangle);
//            }
//            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
//                PostQuitMessage(0);
//            }
//            //Camera Angle
//            if (GetAsyncKeyState('Q') & 0x8000) {
//                m_camera.Rotate(0, -0.02);
//            }
//            if (GetAsyncKeyState('S') & 0x8000) {
//                m_camera.Rotate(0, 0.02);
//            }
//
//            if (GetAsyncKeyState('Z') & 0x8000) {
//                m_camera.Rotate(-0.02, 0);
//            }
//            if (GetAsyncKeyState('W') & 0x8000) {
//                m_camera.Rotate(0.02, 0);
//            }
//            float speed = 0.08;
//            // Mouvement
//            if (GetAsyncKeyState('J') & 0x8000) {
//                DirectX::XMFLOAT3 pos = m_camera.GetPosition();
//                pos.x += speed;
//                m_camera.SetPosition(pos);
//            }
//            if (GetAsyncKeyState('L') & 0x8000) {
//                DirectX::XMFLOAT3 pos = m_camera.GetPosition();
//                pos.x -= speed;
//                m_camera.SetPosition(pos);
//            }
//            if (GetAsyncKeyState('K') & 0x8000) {
//                DirectX::XMFLOAT3 pos = m_camera.GetPosition();
//                pos.z += speed;
//                m_camera.SetPosition(pos);
//            }
//            if (GetAsyncKeyState('I') & 0x8000) {
//                DirectX::XMFLOAT3 pos = m_camera.GetPosition();
//                pos.z -= speed;
//                m_camera.SetPosition(pos);
//            }
//            
//            static bool keyF5WasPressed = false;
//            if (GetAsyncKeyState(VK_F5) & 0x8000) {
//                if (!keyF5WasPressed) {  // Permet de ne basculer qu'une fois par pression
//                    usePostProcessing = !usePostProcessing;
//                    keyF5WasPressed = true;
//                }
//            }
//            else {
//                keyF5WasPressed = false;
//            }
//
//            // Rendu
//            PopulateCommandList();
//            ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
//            commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
//            swapChain->Present(1, 0);
//            WaitForPreviousFrame();
//
//            UpdateFPS();
//            UpdateWindowTitle();
//        }
//    }
//}
//
////Geometry
//void App::SetShape(ShapeType shape) {
//    currentShape = shape;
//
//    LoadGeometry(currentShape);
//}
//
//void App::LoadGeometry(ShapeType shape) {
//    switch (shape) {
//    case ShapeType::Pyramid:
//        LoadPyramidGeometry();
//        break;
//    case ShapeType::Cube:
//        LoadCubeGeometry();
//        break;
//    case ShapeType::Sphere:
//        LoadSphereGeometry();
//        break;
//    case ShapeType::Triangle:
//        LoadTriangleGeometry();
//        break;
//    default:
//        break;
//    }
//}
//
//void App::LoadPyramidGeometry() {
//    // Définition des sommets
//    Vertex pyramidVertices[] = {
//        { {  0.0f,  0.5f,  0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
//        { { -0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
//        { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
//        { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
//        { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f } }
//    };
//
//    UINT16 indices[] = {
//        0, 1, 2,   0, 2, 4,   0, 4, 3,   0, 3, 1,   1, 2, 4,   1, 4, 3,
//    };
//
//    LoadMeshGeometry(pyramidVertices, _countof(pyramidVertices), indices, _countof(indices),
//        vertexBuffer, vertexBufferView, indexBuffer, indexBufferView);
//}
//
//void App::LoadCubeGeometry() {
//    const float halfSize = 0.5f; // Taille réduite pour mieux voir
//
//    std::vector<Vertex> vertices = {
//        // Face avant (Z+)
//        { {-halfSize, -halfSize,  halfSize}, {0.0f, 0.0f, 1.0f, 1.0f} }, // Bleu
//        { { halfSize, -halfSize,  halfSize}, {1.0f, 0.0f, 0.0f, 1.0f} }, // Rouge
//        { { halfSize,  halfSize,  halfSize}, {0.0f, 1.0f, 0.0f, 1.0f} }, // Vert
//        { {-halfSize,  halfSize,  halfSize}, {1.0f, 1.0f, 0.0f, 1.0f} }, // Jaune
//
//        // Face arrière (Z-)
//        { { halfSize, -halfSize, -halfSize}, {1.0f, 0.0f, 1.0f, 1.0f} }, // Magenta
//        { {-halfSize, -halfSize, -halfSize}, {0.0f, 1.0f, 1.0f, 1.0f} }, // Cyan
//        { {-halfSize,  halfSize, -halfSize}, {1.0f, 0.5f, 0.0f, 1.0f} }, // Orange
//        { { halfSize,  halfSize, -halfSize}, {0.5f, 0.0f, 1.0f, 1.0f} }, // Violet
//
//        // Face droite (X+)
//        { { halfSize, -halfSize,  halfSize}, {1.0f, 0.0f, 0.0f, 1.0f} }, // Rouge
//        { { halfSize, -halfSize, -halfSize}, {1.0f, 0.0f, 1.0f, 1.0f} }, // Magenta
//        { { halfSize,  halfSize, -halfSize}, {0.5f, 0.0f, 1.0f, 1.0f} }, // Violet
//        { { halfSize,  halfSize,  halfSize}, {0.0f, 1.0f, 0.0f, 1.0f} }, // Vert
//
//        // Face gauche (X-)
//        { {-halfSize, -halfSize, -halfSize}, {0.0f, 1.0f, 1.0f, 1.0f} }, // Cyan
//        { {-halfSize, -halfSize,  halfSize}, {0.0f, 0.0f, 1.0f, 1.0f} }, // Bleu
//        { {-halfSize,  halfSize,  halfSize}, {1.0f, 1.0f, 0.0f, 1.0f} }, // Jaune
//        { {-halfSize,  halfSize, -halfSize}, {1.0f, 0.5f, 0.0f, 1.0f} }, // Orange
//
//        // Face supérieure (Y+)
//        { {-halfSize,  halfSize,  halfSize}, {1.0f, 1.0f, 0.0f, 1.0f} }, // Jaune
//        { { halfSize,  halfSize,  halfSize}, {0.0f, 1.0f, 0.0f, 1.0f} }, // Vert
//        { { halfSize,  halfSize, -halfSize}, {0.5f, 0.0f, 1.0f, 1.0f} }, // Violet
//        { {-halfSize,  halfSize, -halfSize}, {1.0f, 0.5f, 0.0f, 1.0f} }, // Orange
//
//        // Face inférieure (Y-)
//        { {-halfSize, -halfSize, -halfSize}, {0.0f, 1.0f, 1.0f, 1.0f} }, // Cyan
//        { { halfSize, -halfSize, -halfSize}, {1.0f, 0.0f, 1.0f, 1.0f} }, // Magenta
//        { { halfSize, -halfSize,  halfSize}, {1.0f, 0.0f, 0.0f, 1.0f} }, // Rouge
//        { {-halfSize, -halfSize,  halfSize}, {0.0f, 0.0f, 1.0f, 1.0f} }  // Bleu
//    };
//
//    // Définir les indices pour dessiner le cube (triangles)
//    std::vector<UINT16> indices = {
//        // Face avant
//        0, 1, 2,  0, 2, 3,
//        // Face arrière
//        4, 5, 6,  4, 6, 7,
//        // Face droite
//        8, 9, 10, 8, 10, 11,
//        // Face gauche
//        12, 13, 14, 12, 14, 15,
//        // Face supérieure
//        16, 17, 18, 16, 18, 19,
//        // Face inférieure
//        20, 21, 22, 20, 22, 23
//    };
//
//    // Charger les données du maillage dans les tampons GPU
//    LoadMeshGeometry(
//        vertices.data(), static_cast<UINT>(vertices.size()),
//        indices.data(), static_cast<UINT>(indices.size()),
//        vertexBuffer, vertexBufferView,
//        indexBuffer, indexBufferView
//    );
//}
//
//void App::LoadSphereGeometry() {
//    const float radius = 1.0f;
//    const UINT numSlices = 32;
//    const UINT numStacks = 32;
//
//    std::vector<Vertex> vertices;
//    std::vector<UINT16> indices;
//
//    // Génération des sommets
//    // Pôle nord
//    vertices.push_back({
//        { 0.0f, radius, 0.0f },
//        { 1.0f, 0.0f, 0.0f, 1.0f } // Rouge
//        });
//
//    // Stacks intermédiaires
//    for (UINT stack = 1; stack < numStacks; ++stack) {
//        float phi = XM_PI * stack / numStacks;
//        float y = radius * cos(phi);
//        float r = radius * sin(phi);
//
//        for (UINT slice = 0; slice <= numSlices; ++slice) {
//            float theta = 2 * XM_PI * slice / numSlices;
//
//            Vertex v;
//            v.position = {
//                r * cos(theta),
//                y,
//                r * sin(theta)
//            };
//
//            // Couleur en fonction de la position
//            v.color = {
//                (v.position.x + 1.0f) * 0.5f, // R (rouge) varie de 0 à 1
//                (v.position.y + 1.0f) * 0.5f, // G (vert) varie de 0 à 1
//                (v.position.z + 1.0f) * 0.5f, // B (bleu) varie de 0 à 1
//                1.0f // Alpha (opacité)
//            };
//
//            vertices.push_back(v);
//        }
//    }
//
//    // Pôle sud
//    vertices.push_back({
//        { 0.0f, -radius, 0.0f },
//        { 0.0f, 0.0f, 1.0f, 1.0f } // Bleu
//        });
//
//    // Génération des indices (identique à la version précédente)
//    // Calotte supérieure
//    for (UINT i = 1; i <= numSlices; ++i) {
//        indices.push_back(0);
//        indices.push_back(i);
//        indices.push_back(i + 1);
//    }
//
//    // Faces intermédiaires
//    for (UINT stack = 0; stack < numStacks - 2; ++stack) {
//        UINT start = 1 + stack * (numSlices + 1);
//
//        for (UINT slice = 0; slice < numSlices; ++slice) {
//            indices.push_back(start + slice);
//            indices.push_back(start + slice + numSlices + 1);
//            indices.push_back(start + slice + 1);
//
//            indices.push_back(start + slice + 1);
//            indices.push_back(start + slice + numSlices + 1);
//            indices.push_back(start + slice + numSlices + 2);
//        }
//    }
//
//    // Calotte inférieure
//    UINT bottomIndex = static_cast<UINT>(vertices.size()) - 1;
//    UINT start = bottomIndex - (numSlices + 1);
//
//    for (UINT i = 0; i < numSlices; ++i) {
//        indices.push_back(bottomIndex);
//        indices.push_back(start + i + 1);
//        indices.push_back(start + i);
//    }
//
//    // Appel à la méthode existante
//    LoadMeshGeometry(
//        vertices.data(), static_cast<UINT>(vertices.size()),
//        indices.data(), static_cast<UINT>(indices.size()),
//        vertexBuffer,  // Utilisation du membre existant
//        vertexBufferView,
//        indexBuffer,   // Utilisation du membre existant
//        indexBufferView
//    );
//}
//
//void App::LoadTriangleGeometry() {
//    // Un tétraèdre comporte 4 sommets et 4 faces (3 faces latérales et la base)
//    // Définition des sommets
//    Vertex tetraVertices[] = {
//        // Sommet de l'apex (en haut) – couleur rouge
//        { {  0.0f,  0.5f,  0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
//        // Sommets de la base (triangle) – couleurs variées
//        { { -0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
//        { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
//        { {  0.0f, -0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f, 1.0f } }
//    };
//
//    // Définition des indices (chaque face est un triangle)
//    // Faces latérales
//    // Face 1 : apex, sommet 1, sommet 2
//    // Face 2 : apex, sommet 2, sommet 3
//    // Face 3 : apex, sommet 3, sommet 1
//    // Base (face inférieure) : sommet 1, sommet 2, sommet 3
//    UINT16 tetraIndices[] = {
//        0, 1, 2,  // Face latérale 1
//        0, 2, 3,  // Face latérale 2
//        0, 3, 1,  // Face latérale 3
//        1, 2, 3   // Base
//    };
//
//    LoadMeshGeometry(tetraVertices, _countof(tetraVertices), tetraIndices, _countof(tetraIndices),
//        vertexBuffer, vertexBufferView, indexBuffer, indexBufferView);
//}
//
  //void App::LoadMeshGeometry(Vertex* vertices, UINT vertexCount, UINT16* indices, UINT indexCount,
//    ComPtr<ID3D12Resource>& outVertexBuffer, D3D12_VERTEX_BUFFER_VIEW& outVertexBufferView,
//    ComPtr<ID3D12Resource>& outIndexBuffer, D3D12_INDEX_BUFFER_VIEW& outIndexBufferView) {
//
//    m_indexCount = indexCount;
//    const UINT vertexBufferSize = sizeof(Vertex) * vertexCount;
//    const UINT indexBufferSize = sizeof(UINT16) * indexCount;
//
//    // Création du vertex buffer
//    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
//    CD3DX12_RESOURCE_DESC vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
//    HRESULT hr = device->CreateCommittedResource(
//        &heapProps,
//        D3D12_HEAP_FLAG_NONE,
//        &vertexBufferDesc,
//        D3D12_RESOURCE_STATE_GENERIC_READ,
//        nullptr,
//        IID_PPV_ARGS(&outVertexBuffer)
//    );
//    if (FAILED(hr)) {
//        std::cerr << "Failed to create vertex buffer!" << std::endl;
//        return;
//    }
//
//    // Copier les sommets
//    UINT8* pVertexDataBegin = nullptr;
//    CD3DX12_RANGE readRange(0, 0);
//    hr = outVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
//    if (FAILED(hr)) {
//        std::cerr << "Failed to map vertex buffer!" << std::endl;
//        return;
//    }
//    memcpy(pVertexDataBegin, vertices, vertexBufferSize);
//    outVertexBuffer->Unmap(0, nullptr);
//
//    // Initialiser la vue du vertex buffer
//    outVertexBufferView.BufferLocation = outVertexBuffer->GetGPUVirtualAddress();
//    outVertexBufferView.StrideInBytes = sizeof(Vertex);
//    outVertexBufferView.SizeInBytes = vertexBufferSize;
//
//    // Création de l'index buffer
//    CD3DX12_RESOURCE_DESC indexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
//    hr = device->CreateCommittedResource(
//        &heapProps,
//        D3D12_HEAP_FLAG_NONE,
//        &indexBufferDesc,
//        D3D12_RESOURCE_STATE_GENERIC_READ,
//        nullptr,
//        IID_PPV_ARGS(&outIndexBuffer)
//    );
//    if (FAILED(hr)) {
//        std::cerr << "Failed to create index buffer!" << std::endl;
//        return;
//    }
//
//    // Copier les indices
//    UINT8* pIndexDataBegin = nullptr;
//    hr = outIndexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin));
//    if (FAILED(hr)) {
//        std::cerr << "Failed to map index buffer!" << std::endl;
//        return;
//    }
//    memcpy(pIndexDataBegin, indices, indexBufferSize);
//    outIndexBuffer->Unmap(0, nullptr);
//
//    // Initialiser la vue de l'index buffer
//    outIndexBufferView.BufferLocation = outIndexBuffer->GetGPUVirtualAddress();
//    outIndexBufferView.Format = DXGI_FORMAT_R16_UINT;
//    outIndexBufferView.SizeInBytes = indexBufferSize;
//}
//
//void App::UpdateFPS() {
//    using namespace std::chrono;
//
//    // Calcul du temps écoulé depuis la dernière frame
//    auto now = steady_clock::now();
//    duration<float> elapsed = now - lastFrameTime;
//    lastFrameTime = now;
//
//    // Mettre à jour le FPS
//    frameTime = elapsed.count();
//    fps = 1.0f / frameTime;
//
//}
//
//void App::UpdateWindowTitle() {
//    using namespace std::chrono;
//
//    auto now = steady_clock::now();
//    duration<float> elapsed = now - lastTitleUpdateTime;
//
//    // Si 1 seconde s'est écoulée
//    if (elapsed.count() >= 1.0f) {
//        lastTitleUpdateTime = now; 
//
//        wchar_t title[256];
//        swprintf_s(title, 256, L"Unlimited Engine <DX12>   FPS: %d", static_cast<int>(fps));
//        SetWindowText(hwnd, title);
//    }
//}

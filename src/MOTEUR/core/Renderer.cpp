#include "pch.h"
#include "Renderer.h"
#include "Mesh.h"

Renderer::Renderer() 
{
    device = nullptr;
    commandQueue = nullptr;
    swapChain = nullptr;
    rtvHeap = nullptr;
    commandAllocator = nullptr;
    pipelineState = nullptr;
    rootSignature = nullptr;
    dsvHeap = nullptr;
    depthStencilBuffer = nullptr;
    offscreenRTVHeap = nullptr;
    srvHeap = nullptr;
    postProcessPipelineState = nullptr;
}

Renderer::~Renderer() {
    Cleanup();
}

Renderer& Renderer::GetInstance() {
    static Renderer instance;
    return instance;
}

bool Renderer::Initialize(HWND hwnd, int width, int height) {
    this->hwnd = hwnd;
    this->width = width;
    this->height = height;

    iconPath = GetConfigValue(L"Paths", L"Icon", L"icon.ico", configPath);
    shaderPath = GetConfigValue(L"Paths", L"Shader", L"shaders.hlsl", configPath);
    postProcessPath = GetConfigValue(L"Paths", L"PostProcessing", L"postprocess.hlsl", configPath);

    if (LoadPipeline() == false)
        return false;

    if (LoadAssets() == false)
        return false;

    return true;
}

void Renderer::Render(const std::vector<RenderableObject>& renderables) {
    PopulateCommandList(renderables);

    ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    swapChain->Present(1, 0);
    WaitForPreviousFrame();
}

void Renderer::Cleanup() {
    // Libérer les ressources DirectX
}

bool Renderer::LoadPipeline() {
    UINT dxgiFactoryFlags = 0;
#ifdef _DEBUG
    Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        debugController->EnableDebugLayer();
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif

    // Création de la factory DXGI
    Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
    if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)))) {
        OutputDebugStringA("Failed to create DXGI Factory!");
        return false;
    }

    // Création de l'appareil D3D12
    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)))) {
        OutputDebugStringA("Failed to create D3D12 Device!");
        return false;
    }

    // Création de la Command Queue
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)))) {
        OutputDebugStringA("Failed to create Command Queue!");
        return false;
    }

    // Création de la Swap Chain
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = kFrameCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    Microsoft::WRL::ComPtr<IDXGISwapChain1> tempSwapChain;
    if (FAILED(factory->CreateSwapChainForHwnd(
        commandQueue.Get(),
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &tempSwapChain))) {
        OutputDebugStringA("Failed to create Swap Chain!");
        return false;
    }
    tempSwapChain.As(&swapChain);

    // Création du heap des RTV pour le back buffer
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = kFrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)))) {
        OutputDebugStringA("Failed to create RTV Heap!");
        return false;
    }
    rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < kFrameCount; i++) {
        if (FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])))) {
            OutputDebugStringA("Failed to get Swap Chain Buffer!");
            return false;
        }
        device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, rtvDescriptorSize);
    }

    // Création du Command Allocator
    if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)))) {
        OutputDebugStringA("Failed to create Command Allocator!");
        return false;
    }

    // Création du heap pour la Depth Stencil View (DSV) principal
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    if (FAILED(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap)))) {
        OutputDebugStringA("Failed to create DSV Heap\n");
        return false;
    }

    // --- Création du Depth Stencil Buffer principal ---
    CD3DX12_HEAP_PROPERTIES mainHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_CLEAR_VALUE mainDepthOptimizedClearValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
    CD3DX12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
        DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
    );
    if (FAILED(device->CreateCommittedResource(
        &mainHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &mainDepthOptimizedClearValue,
        IID_PPV_ARGS(&depthStencilBuffer)
    ))) {
        OutputDebugStringA("Failed to create Depth Stencil Buffer!");
        return false;
    }

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvDesc,
        dsvHeap->GetCPUDescriptorHandleForHeapStart());

    // --- Création du heap RTV pour l'offscreen render target ---
    D3D12_DESCRIPTOR_HEAP_DESC offscreenRTVHeapDesc = {};
    offscreenRTVHeapDesc.NumDescriptors = 1;
    offscreenRTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    offscreenRTVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    if (FAILED(device->CreateDescriptorHeap(&offscreenRTVHeapDesc, IID_PPV_ARGS(&offscreenRTVHeap)))) {
        OutputDebugStringA("Failed to create offscreen RTV Heap");
        return false;
    }

    // --- Création de la texture offscreen (cible de rendu intermédiaire) ---
    D3D12_RESOURCE_DESC offscreenTexDesc = {};
    offscreenTexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    offscreenTexDesc.Alignment = 0;
    offscreenTexDesc.Width = width;
    offscreenTexDesc.Height = height;
    offscreenTexDesc.DepthOrArraySize = 1;
    offscreenTexDesc.MipLevels = 1;
    offscreenTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    offscreenTexDesc.SampleDesc.Count = 1;
    offscreenTexDesc.SampleDesc.Quality = 0;
    offscreenTexDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    offscreenTexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    // Utilisation d'une heap properties locale pour la texture offscreen
    CD3DX12_HEAP_PROPERTIES offscreenTexHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_CLEAR_VALUE offscreenClearValue = {};
    offscreenClearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    offscreenClearValue.Color[0] = 0.0f;
    offscreenClearValue.Color[1] = 0.0f;
    offscreenClearValue.Color[2] = 0.0f;
    offscreenClearValue.Color[3] = 1.0f;
    Microsoft::WRL::ComPtr<ID3D12Resource> offscreenResource;
    if (FAILED(device->CreateCommittedResource(
        &offscreenTexHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &offscreenTexDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &offscreenClearValue,
        IID_PPV_ARGS(&offscreenResource)
    ))) {
        OutputDebugStringA("Failed to create offscreen render target!");
        return false;
    }
    // Assigner la ressource offscreen à l'objet offscreenRenderTarget.
    offscreenRenderTarget.SetResource(offscreenResource);

    // Création du RTV pour l'offscreen render target
    CD3DX12_CPU_DESCRIPTOR_HANDLE offscreenRTVHandle(offscreenRTVHeap->GetCPUDescriptorHandleForHeapStart());
    D3D12_RENDER_TARGET_VIEW_DESC offscreenRTVDesc = {};
    offscreenRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    offscreenRTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    offscreenRTVDesc.Texture2D.MipSlice = 0;
    offscreenRTVDesc.Texture2D.PlaneSlice = 0;
    device->CreateRenderTargetView(offscreenRenderTarget.GetResource(), &offscreenRTVDesc, offscreenRTVHandle);

    // --- Création d'un Depth Stencil Buffer pour l'offscreen render target ---
    D3D12_DESCRIPTOR_HEAP_DESC offscreenDSVHeapDesc = {};
    offscreenDSVHeapDesc.NumDescriptors = 1;
    offscreenDSVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    offscreenDSVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    if (FAILED(device->CreateDescriptorHeap(&offscreenDSVHeapDesc, IID_PPV_ARGS(&offscreenDSVHeap)))) {
        OutputDebugStringA("Failed to create offscreen DSV Heap!\n");
        return false;
    }

    // Pour le Depth Stencil Buffer de l'offscreen target, utiliser des noms distincts
    CD3DX12_HEAP_PROPERTIES offscreenDepthHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_CLEAR_VALUE offscreenDepthOptimizedClearValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
    CD3DX12_RESOURCE_DESC offscreenDepthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
        DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
    );
    if (FAILED(device->CreateCommittedResource(
        &offscreenDepthHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &offscreenDepthStencilDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &offscreenDepthOptimizedClearValue,
        IID_PPV_ARGS(&offscreenDepthStencilBuffer)
    ))) {
        OutputDebugStringA("Failed to create offscreen Depth Stencil Buffer!");
        return false;
    }

    D3D12_DEPTH_STENCIL_VIEW_DESC offscreenDSVDesc = {};
    offscreenDSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
    offscreenDSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    device->CreateDepthStencilView(
        offscreenDepthStencilBuffer.Get(),
        &offscreenDSVDesc,
        offscreenDSVHeap->GetCPUDescriptorHandleForHeapStart()
    );

    // --- Création du heap SRV pour la texture offscreen ---
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (FAILED(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)))) {
        OutputDebugStringA("Failed to create SRV Heap!");
        return false;
    }
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    device->CreateShaderResourceView(offscreenRenderTarget.GetResource(), &srvDesc, srvHeap->GetCPUDescriptorHandleForHeapStart());

    // Création de la Command List
    HRESULT hrCmdList = device->CreateCommandList(
        0, // Node mask (pour un GPU unique)
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        commandAllocator.Get(),
        nullptr, // Pipeline state initial (peut être nullptr)
        IID_PPV_ARGS(&commandList)
    );
    if (FAILED(hrCmdList)) {
        OutputDebugStringA("Failed to create Command List!\n");
        return false;
    }
    // Fermer la Command List après création
    commandList->Close();

    return true;
}

bool Renderer::LoadAssets() {
    // 1. Créer un root signature adapté aux shaders
    // On définit 2 paramètres :
    // - Paramètre 0 : Constant Buffer View à b0.
    // - Paramètre 1 : Table de descripteurs pour une texture (SRV à t0).
    CD3DX12_ROOT_PARAMETER rootParameters[2];
    rootParameters[0].InitAsConstantBufferView(0);
    CD3DX12_DESCRIPTOR_RANGE srvRange;
    srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    rootParameters[1].InitAsDescriptorTable(1, &srvRange, D3D12_SHADER_VISIBILITY_PIXEL);

    // Définir un sampler statique pour register s0 (filtrage linéaire)
    CD3DX12_STATIC_SAMPLER_DESC staticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

    // Décrire le root signature avec 2 paramètres et 1 sampler statique
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 1, &staticSampler,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // Sérialiser et créer le root signature
    Microsoft::WRL::ComPtr<ID3DBlob> signature;
    Microsoft::WRL::ComPtr<ID3DBlob> error;
    if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        &signature, &error)))
    {
        std::cerr << "Failed to serialize root signature!" << std::endl;
        if (error)
            std::cerr << (char*)error->GetBufferPointer() << std::endl;
        return false;
    }
    if (FAILED(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature))))
    {
        std::cerr << "Failed to create root signature!" << std::endl;
        return false;
    }

    // 2. Compiler les shaders
    Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
    Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;
    Microsoft::WRL::ComPtr<ID3DBlob> ppVertexShader;
    Microsoft::WRL::ComPtr<ID3DBlob> ppPixelShader;

#ifdef _DEBUG
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif
    // Shader pour Rendu principal
    if (FAILED(D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &error))) {
        OutputDebugStringA("Failed to compile vertex shader!\n");
        if (error)
            std::cerr << (char*)error->GetBufferPointer() << std::endl;
        return false;
    }
    if (FAILED(D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &error))) {
        OutputDebugStringA("Failed to compile pixel shader!\n");
        if (error)
            std::cerr << (char*)error->GetBufferPointer() << std::endl;
        return false;
    }

    // Shaders pour le post-processing
    if (FAILED(D3DCompileFromFile(postProcessPath.c_str(), nullptr, nullptr, "PPVS", "vs_5_0", compileFlags, 0, &ppVertexShader, &error))) {
        OutputDebugStringA("Failed to compile post process vertex shader!\n");
        if (error)
            std::cerr << (char*)error->GetBufferPointer() << std::endl;
        return false;
    }
    if (FAILED(D3DCompileFromFile(postProcessPath.c_str(), nullptr, nullptr, "PPPS", "ps_5_0", compileFlags, 0, &ppPixelShader, &error))) {
        OutputDebugStringA("Failed to compile post process pixel shader!");
        if (error)
            std::cerr << (char*)error->GetBufferPointer() << std::endl;
        return false;
    }

    // 3. Définir la disposition des sommets pour le pipeline principal
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,   0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // 4. Créer l'état du pipeline (PSO) pour le rendu principal
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = rootSignature.Get();
    psoDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
    psoDesc.PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.DSVFormat = DepthBufferFormat;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;
    if (FAILED(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)))) {
        std::cerr << "Failed to create pipeline state!" << std::endl;
        return false;
    }

    // 5. Définir la disposition des sommets pour le post-processing
    D3D12_INPUT_ELEMENT_DESC ppInputElementDescs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // Créer le PSO pour le post-processing
    D3D12_GRAPHICS_PIPELINE_STATE_DESC ppPsoDesc = {};
    ppPsoDesc.InputLayout = { ppInputElementDescs, _countof(ppInputElementDescs) };
    ppPsoDesc.pRootSignature = rootSignature.Get();
    ppPsoDesc.VS = { ppVertexShader->GetBufferPointer(), ppVertexShader->GetBufferSize() };
    ppPsoDesc.PS = { ppPixelShader->GetBufferPointer(), ppPixelShader->GetBufferSize() };
    ppPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    ppPsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    ppPsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    ppPsoDesc.DepthStencilState.DepthEnable = FALSE;
    ppPsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    ppPsoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
    ppPsoDesc.SampleMask = UINT_MAX;
    ppPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    ppPsoDesc.NumRenderTargets = 1;
    ppPsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    ppPsoDesc.SampleDesc.Count = 1;
    if (FAILED(device->CreateGraphicsPipelineState(&ppPsoDesc, IID_PPV_ARGS(&postProcessPipelineState)))) {
        std::cerr << "Failed to create post process pipeline state!" << std::endl;
        return false;
    }

    // 6. Initialiser le tampon constant
    constantBuffer.Initialize(device.Get());

    // 7. Initialiser la cible de rendu offscreen
    offscreenRenderTarget.Initialize(device.Get(), width, height);

    // 8. Initialiser le post-processor (quad)
    postProcessor.Initialize(device.Get());

    // 9. Créer et exécuter la liste de commandes initiale
    commandList->Close();
    ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    WaitForPreviousFrame();
    return true;
}

void Renderer::PopulateCommandList(const std::vector<RenderableObject>& renderables) {
    // Récupérer l'index de la frame actuelle
    int frameIndex = swapChain->GetCurrentBackBufferIndex();

    // Réinitialiser l'allocateur de commandes et la liste de commandes
    commandAllocator->Reset();
    // Note : On choisit le pipelineState en fonction du rendu direct (sera modifié plus tard si post-processing)
    commandList->Reset(commandAllocator.Get(), pipelineState.Get());

    // Définir la signature racine (root signature)
    commandList->SetGraphicsRootSignature(rootSignature.Get());

    // Mise à jour de la caméra (si nécessaire)
    static float angle = 0.0f;
    angle += 0.01f;
    m_camera.Update(static_cast<float>(width) / height);

    // Configuration du viewport et du scissor rect
    D3D12_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
    D3D12_RECT scissorRect = { 0, 0, width, height };
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    if (usePostProcessing) {
        // --- 1. Rendu de la scène vers l'offscreen ---
        // Si l'offscreen était utilisé précédemment en tant que texture, on doit le passer en état RENDER_TARGET
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            offscreenDepthStencilBuffer.Get(),
            D3D12_RESOURCE_STATE_COMMON, 
            D3D12_RESOURCE_STATE_DEPTH_WRITE
        );
        commandList->ResourceBarrier(1, &barrier);

        commandList->ResourceBarrier(1, &barrier);

        // Définir l'offscreen render target et le depth stencil (pour la profondeur)
        CD3DX12_CPU_DESCRIPTOR_HANDLE offscreenRTVHandle(offscreenRTVHeap->GetCPUDescriptorHandleForHeapStart());
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(dsvHeap->GetCPUDescriptorHandleForHeapStart());
        commandList->OMSetRenderTargets(1, &offscreenRTVHandle, FALSE, &dsvHandle);

        // Effacer l'offscreen render target et le tampon de profondeur
        const float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
        commandList->ClearRenderTargetView(offscreenRTVHandle, clearColor, 0, nullptr);

        commandList->ClearDepthStencilView(
            offscreenDSVHeap->GetCPUDescriptorHandleForHeapStart(),
            D3D12_CLEAR_FLAG_DEPTH,
            1.0f, 0, 0, nullptr
        );


        // Rendu de chaque objet
        for (auto& obj : renderables) {
            ConstantBufferData cbData;
            XMStoreFloat4x4(&cbData.world, XMMatrixTranspose(obj.worldMatrix));
            XMStoreFloat4x4(&cbData.view, XMMatrixTranspose(m_camera.GetViewMatrix()));
            XMStoreFloat4x4(&cbData.projection, XMMatrixTranspose(m_camera.GetProjectionMatrix()));
            obj.constantBuffer.Update(cbData);
            commandList->SetGraphicsRootConstantBufferView(0, obj.constantBuffer.GetGPUVirtualAddress());
            obj.mesh->Draw(commandList.Get());
        }

        // Transition de l'offscreen vers l'état PIXEL_SHADER_RESOURCE pour le post-processing
        barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            offscreenRenderTarget.GetResource(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );
        commandList->ResourceBarrier(1, &barrier);

        // --- 2. Rendu du post-processing ---
        // Rediriger le rendu vers le back buffer
        CD3DX12_CPU_DESCRIPTOR_HANDLE backBufferHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
        commandList->OMSetRenderTargets(1, &backBufferHandle, FALSE, nullptr);

        // Changer le pipeline pour le post-processing
        commandList->SetPipelineState(postProcessPipelineState.Get());
        // Utiliser la texture offscreen via le SRV
        commandList->SetGraphicsRootDescriptorTable(1, srvHeap->GetGPUDescriptorHandleForHeapStart());

        // Dessiner le quad de post-processing
        postProcessor.Draw(commandList.Get());
    }
    else {
        // --- Rendu direct vers le back buffer ---
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(dsvHeap->GetCPUDescriptorHandleForHeapStart());
        commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

        const float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
        commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        for (auto& obj : renderables) {
            ConstantBufferData cbData;
            XMStoreFloat4x4(&cbData.world, XMMatrixTranspose(obj.worldMatrix));
            XMStoreFloat4x4(&cbData.view, XMMatrixTranspose(m_camera.GetViewMatrix()));
            XMStoreFloat4x4(&cbData.projection, XMMatrixTranspose(m_camera.GetProjectionMatrix()));
            obj.constantBuffer.Update(cbData);
            commandList->SetGraphicsRootConstantBufferView(0, obj.constantBuffer.GetGPUVirtualAddress());
            obj.mesh->Draw(commandList.Get());
        }
    }

    // Transition du back buffer vers l'état de présentation
    CD3DX12_RESOURCE_BARRIER presentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        renderTargets[frameIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    );
    commandList->ResourceBarrier(1, &presentBarrier);

    // Fermer la liste de commandes
    commandList->Close();
}

void Renderer::WaitForPreviousFrame() 
{
    static UINT frameIndex = 0;
    static UINT64 fenceValue = 0;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;

    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    fenceValue++;

    commandQueue->Signal(fence.Get(), fenceValue);

    if (fence->GetCompletedValue() < fenceValue) {
        HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        fence->SetEventOnCompletion(fenceValue, eventHandle);
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }

    frameIndex = swapChain->GetCurrentBackBufferIndex();
}
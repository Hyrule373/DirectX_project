#pragma once
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include "Mesh.h"

#include "Camera.h"
#include "PostProcessor.h"
#include "ConstantBuffer.h"
#include "OffscreenRenderTarget.h"
#include "RenderableObject.h"

class Mesh;

class Renderer {
public:
    Renderer();
    ~Renderer();

    static Renderer& GetInstance();
    ID3D12Device* GetDevice() { return device.Get(); }

    bool Initialize(HWND hwnd, int width, int height);
    void Render(const std::vector<RenderableObject>& renderables);
    void Cleanup();

    Camera m_camera;



private:
    bool LoadPipeline();
    bool LoadAssets();
    void PopulateCommandList(const std::vector<RenderableObject>& renderables);
    void WaitForPreviousFrame();

    HWND hwnd;
    int width;
    int height;

    std::wstring configPath = L"../config.ini";
    std::wstring iconPath;
    std::wstring shaderPath;
    std::wstring postProcessPath;

    bool usePostProcessing = false;

    Microsoft::WRL::ComPtr<ID3D12Device> device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
    Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
    Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[kFrameCount];
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    UINT rtvDescriptorSize;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> offscreenRTVHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> postProcessPipelineState;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> offscreenDSVHeap;
    Microsoft::WRL::ComPtr<ID3D12Resource> offscreenDepthStencilBuffer;

    static constexpr DXGI_FORMAT DepthBufferFormat = DXGI_FORMAT_D32_FLOAT;

    static constexpr UINT kFrameCount = 2;

    PostProcessor postProcessor;
    ConstantBuffer constantBuffer;
    OffscreenRenderTarget offscreenRenderTarget;
};
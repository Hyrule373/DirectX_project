#include "pch.h"
#include "OffscreenRenderTarget.h"

OffscreenRenderTarget::OffscreenRenderTarget() {
    // Constructeur vide
}

OffscreenRenderTarget::~OffscreenRenderTarget() {
    // Les ressources DirectX seront libérées automatiquement par ComPtr
}

void OffscreenRenderTarget::Initialize(ID3D12Device* device, int width, int height) {
    // Créer la texture offscreen
    D3D12_RESOURCE_DESC offscreenTexDesc = {};
    offscreenTexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    offscreenTexDesc.Width = width;
    offscreenTexDesc.Height = height;
    offscreenTexDesc.DepthOrArraySize = 1;
    offscreenTexDesc.MipLevels = 1;
    offscreenTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    offscreenTexDesc.SampleDesc.Count = 1;
    offscreenTexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_CLEAR_VALUE clearValue(DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::Colors::Black);

    device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &offscreenTexDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clearValue,
        IID_PPV_ARGS(&renderTarget));
}
void OffscreenRenderTarget::SetResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
    this->renderTarget = resource; 
}

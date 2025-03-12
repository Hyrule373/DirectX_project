#pragma once
#include "pch.h"

class PostProcessor {
public:
    PostProcessor();
    ~PostProcessor();

    void Initialize(ID3D12Device* device);
    void Draw(ID3D12GraphicsCommandList* commandList);

private:
    struct PostProcessVertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 uv;
    };

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
};
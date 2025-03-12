#pragma once

struct ConstantBufferData {
    DirectX::XMFLOAT4X4 world;          // Matrice monde
    DirectX::XMFLOAT4X4 view;           // Matrice vue
    DirectX::XMFLOAT4X4 projection;     // Matrice projection
};

class ConstantBuffer {
public:
    ConstantBuffer();
    ~ConstantBuffer();

    void Initialize(ID3D12Device* device, UINT size = sizeof(ConstantBufferData));
    void Update(const ConstantBufferData& data) const;

    ID3D12Resource* GetBuffer() const { return constantBuffer.Get(); }
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return constantBuffer->GetGPUVirtualAddress(); }

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer;
    UINT8* pConstantBufferDataBegin = nullptr; // Pointeur vers les données du constant buffer
};
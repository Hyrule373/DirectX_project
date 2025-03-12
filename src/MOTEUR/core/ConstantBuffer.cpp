#include "pch.h"
#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer() = default;

ConstantBuffer::~ConstantBuffer() {
    if (constantBuffer) {
        constantBuffer->Unmap(0, nullptr);
    }
}

void ConstantBuffer::Initialize(ID3D12Device* device, UINT size) {
    // Créer le constant buffer
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&constantBuffer)
    );

    if (FAILED(hr)) {
        throw std::runtime_error("Échec de la création du Constant Buffer");
    }

    // Mapper le constant buffer
    CD3DX12_RANGE readRange(0, 0); // On ne lit pas depuis le CPU
    hr = constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pConstantBufferDataBegin));

    if (FAILED(hr)) {
        throw std::runtime_error("Échec du mapping du Constant Buffer");
    }
}


void ConstantBuffer::Update(const ConstantBufferData& data) const {
    // Copier les données dans le constant buffer
    memcpy(pConstantBufferDataBegin, &data, sizeof(ConstantBufferData));
}
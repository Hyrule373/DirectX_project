#include "pch.h"
#include "PostProcessor.h"

PostProcessor::PostProcessor() {
}

PostProcessor::~PostProcessor() {
}

void PostProcessor::Initialize(ID3D12Device* device) 
{
    // Définir les sommets du quad
    PostProcessVertex quadVertices[] = {
        { {-1.0f, -1.0f, 0.0f }, {0.0f, 1.0f} },
        { {-1.0f,  1.0f, 0.0f }, {0.0f, 0.0f} },
        { { 1.0f, -1.0f, 0.0f }, {1.0f, 1.0f} },
        { { 1.0f,  1.0f, 0.0f }, {1.0f, 0.0f} }
    };

    const UINT quadVertexBufferSize = sizeof(quadVertices);

    // Créer le vertex buffer
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC vbDesc = CD3DX12_RESOURCE_DESC::Buffer(quadVertexBufferSize);
    device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &vbDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertexBuffer));

    // Copier les données des sommets dans le vertex buffer
    UINT8* pVertexData;
    vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pVertexData));
    memcpy(pVertexData, quadVertices, quadVertexBufferSize);
    vertexBuffer->Unmap(0, nullptr);

    // Initialiser la vue du vertex buffer
    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.StrideInBytes = sizeof(PostProcessVertex);
    vertexBufferView.SizeInBytes = quadVertexBufferSize;
}

void PostProcessor::Draw(ID3D12GraphicsCommandList* commandList) {
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    commandList->DrawInstanced(4, 1, 0, 0);
}
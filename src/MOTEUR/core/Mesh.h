#pragma once

struct Vertex {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT4 color;
};

class Mesh {
public:
    Mesh();
    ~Mesh();

    void LoadPyramidGeometry(ID3D12Device* device);
    void LoadCubeGeometry(ID3D12Device* device);
    void LoadSphereGeometry(ID3D12Device* device);
    void LoadTriangleGeometry(ID3D12Device* device);

    bool Create(ID3D12Device* device, const std::vector<Vertex>& vertices, const std::vector<UINT16>& indices);
    void Draw(ID3D12GraphicsCommandList* commandList);

    const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return vertexBufferView; }
    const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const { return indexBufferView; }
    UINT GetIndexCount() const { return static_cast<UINT>(indices.size()); }

private:
    std::vector<Vertex> vertices;
    std::vector<UINT16> indices;

    DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
    D3D12_INDEX_BUFFER_VIEW indexBufferView;

    bool CreateBuffers(Microsoft::WRL::ComPtr<ID3D12Device> device);
};
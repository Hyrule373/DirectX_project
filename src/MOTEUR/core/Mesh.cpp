#include "pch.h"

#include "Renderer.h"
#include "Mesh.h"

Mesh::Mesh() {
}

Mesh::~Mesh() {

}

bool Mesh::Create(ID3D12Device* device, const std::vector<Vertex>& vertices, const std::vector<UINT16>& indices) {
    this->vertices = vertices;
    this->indices = indices;

    if (CreateBuffers(device) == false)
        return false;

    return true;
}

void Mesh::Draw(ID3D12GraphicsCommandList* commandList) 
{
    //Mesh
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
    commandList->IASetIndexBuffer(&indexBufferView);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //Draw call
    commandList->DrawIndexedInstanced(GetIndexCount(), 1, 0, 0, 0);
}

///Creating Objects///

void Mesh::LoadPyramidGeometry(ID3D12Device* device) {

    std::vector<Vertex> pyramidVertices = {
        // Sommet de la pyramide
        { {  0.0f,  0.5f,  0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Vertex 0

        // Base de la pyramide
        { { -0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // Vertex 1
        { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // Vertex 2
        { {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f, 1.0f } }, // Vertex 3
        { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f } }  // Vertex 4
    };

    std::vector<UINT16> pyramidIndices = {
        // Faces latérales
        0, 1, 2,   // Face avant
        0, 2, 3,   // Face droite
        0, 3, 4,   // Face arrière
        0, 4, 1,   // Face gauche

        // Base (2 triangles pour former un carré)
        1, 2, 3,   // Triangle 1 de la base
        1, 3, 4    // Triangle 2 de la base
    };

    Create(device, pyramidVertices, pyramidIndices);
}

void Mesh::LoadCubeGeometry(ID3D12Device* device) {
    const float halfSize = 0.5f;

    std::vector<Vertex> cubeVertices = {
        // Face avant (Z+)
        { {-halfSize, -halfSize,  halfSize}, {0.0f, 0.0f, 1.0f, 1.0f} }, // Bleu
        { { halfSize, -halfSize,  halfSize}, {1.0f, 0.0f, 0.0f, 1.0f} }, // Rouge
        { { halfSize,  halfSize,  halfSize}, {0.0f, 1.0f, 0.0f, 1.0f} }, // Vert
        { {-halfSize,  halfSize,  halfSize}, {1.0f, 1.0f, 0.0f, 1.0f} }, // Jaune

        // Face arri�re (Z-)
        { { halfSize, -halfSize, -halfSize}, {1.0f, 0.0f, 1.0f, 1.0f} }, // Magenta
        { {-halfSize, -halfSize, -halfSize}, {0.0f, 1.0f, 1.0f, 1.0f} }, // Cyan
        { {-halfSize,  halfSize, -halfSize}, {1.0f, 0.5f, 0.0f, 1.0f} }, // Orange
        { { halfSize,  halfSize, -halfSize}, {0.5f, 0.0f, 1.0f, 1.0f} }, // Violet

        // Face droite (X+)
        { { halfSize, -halfSize,  halfSize}, {1.0f, 0.0f, 0.0f, 1.0f} }, // Rouge
        { { halfSize, -halfSize, -halfSize}, {1.0f, 0.0f, 1.0f, 1.0f} }, // Magenta
        { { halfSize,  halfSize, -halfSize}, {0.5f, 0.0f, 1.0f, 1.0f} }, // Violet
        { { halfSize,  halfSize,  halfSize}, {0.0f, 1.0f, 0.0f, 1.0f} }, // Vert

        // Face gauche (X-)
        { {-halfSize, -halfSize, -halfSize}, {0.0f, 1.0f, 1.0f, 1.0f} }, // Cyan
        { {-halfSize, -halfSize,  halfSize}, {0.0f, 0.0f, 1.0f, 1.0f} }, // Bleu
        { {-halfSize,  halfSize,  halfSize}, {1.0f, 1.0f, 0.0f, 1.0f} }, // Jaune
        { {-halfSize,  halfSize, -halfSize}, {1.0f, 0.5f, 0.0f, 1.0f} }, // Orange

        // Face sup�rieure (Y+)
        { {-halfSize,  halfSize,  halfSize}, {1.0f, 1.0f, 0.0f, 1.0f} }, // Jaune
        { { halfSize,  halfSize,  halfSize}, {0.0f, 1.0f, 0.0f, 1.0f} }, // Vert
        { { halfSize,  halfSize, -halfSize}, {0.5f, 0.0f, 1.0f, 1.0f} }, // Violet
        { {-halfSize,  halfSize, -halfSize}, {1.0f, 0.5f, 0.0f, 1.0f} }, // Orange

        // Face inf�rieure (Y-)
        { {-halfSize, -halfSize, -halfSize}, {0.0f, 1.0f, 1.0f, 1.0f} }, // Cyan
        { { halfSize, -halfSize, -halfSize}, {1.0f, 0.0f, 1.0f, 1.0f} }, // Magenta
        { { halfSize, -halfSize,  halfSize}, {1.0f, 0.0f, 0.0f, 1.0f} }, // Rouge
        { {-halfSize, -halfSize,  halfSize}, {0.0f, 0.0f, 1.0f, 1.0f} }  // Bleu
    };

    // D�finir les indices pour dessiner le cube (triangles)
    std::vector<UINT16> cubeIndices = {
        // Face avant
        0, 1, 2,  0, 2, 3,
        // Face arri�re
        4, 5, 6,  4, 6, 7,
        // Face droite
        8, 9, 10, 8, 10, 11,
        // Face gauche
        12, 13, 14, 12, 14, 15,
        // Face sup�rieure
        16, 17, 18, 16, 18, 19,
        // Face inf�rieure
        20, 21, 22, 20, 22, 23
    };

    Create(device, cubeVertices, cubeIndices);
}

void Mesh::LoadSphereGeometry(ID3D12Device* device) {
    const float radius = 1.0f;
    const UINT numSlices = 32;
    const UINT numStacks = 32;

    std::vector<Vertex> vertices;
    std::vector<UINT16> indices;

    // G�n�ration des sommets
    // P�le nord
    vertices.push_back({
        { 0.0f, radius, 0.0f },
        { 1.0f, 0.0f, 0.0f, 1.0f } // Rouge
        });

    // Stacks interm�diaires
    for (UINT stack = 1; stack < numStacks; ++stack) {
        float phi = DirectX::XM_PI * stack / numStacks;
        float y = radius * cos(phi);
        float r = radius * sin(phi);

        for (UINT slice = 0; slice <= numSlices; ++slice) {
            float theta = 2 * DirectX::XM_PI * slice / numSlices;

            Vertex v;
            v.position = {
                r * cos(theta),
                y,
                r * sin(theta)
            };

            // Couleur en fonction de la position
            v.color = {
                (v.position.x + 1.0f) * 0.5f, // R (rouge) varie de 0 � 1
                (v.position.y + 1.0f) * 0.5f, // G (vert) varie de 0 � 1
                (v.position.z + 1.0f) * 0.5f, // B (bleu) varie de 0 � 1
                1.0f // Alpha (opacit�)
            };

            vertices.push_back(v);
        }
    }

    // P�le sud
    vertices.push_back({
        { 0.0f, -radius, 0.0f },
        { 0.0f, 0.0f, 1.0f, 1.0f } // Bleu
        });

    // G�n�ration des indices (identique � la version pr�c�dente)
    // Calotte sup�rieure
    for (UINT i = 1; i <= numSlices; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    // Faces interm�diaires
    for (UINT stack = 0; stack < numStacks - 2; ++stack) {
        UINT start = 1 + stack * (numSlices + 1);

        for (UINT slice = 0; slice < numSlices; ++slice) {
            indices.push_back(start + slice);
            indices.push_back(start + slice + numSlices + 1);
            indices.push_back(start + slice + 1);

            indices.push_back(start + slice + 1);
            indices.push_back(start + slice + numSlices + 1);
            indices.push_back(start + slice + numSlices + 2);
        }
    }

    // Calotte inf�rieure
    UINT bottomIndex = static_cast<UINT>(vertices.size()) - 1;
    UINT start = bottomIndex - (numSlices + 1);

    for (UINT i = 0; i < numSlices; ++i) {
        indices.push_back(bottomIndex);
        indices.push_back(start + i + 1);
        indices.push_back(start + i);
    }

    // Appel � la m�thode existante
    Create(device, vertices, indices);
}

void Mesh::LoadTriangleGeometry(ID3D12Device* device) {
    // Un t�tra�dre comporte 4 sommets et 4 faces (3 faces lat�rales et la base)
    // D�finition des sommets
    std::vector<Vertex> tetraVertices = {
        // Sommet de l'apex (en haut) � couleur rouge
        { {  0.0f,  0.5f,  0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        // Sommets de la base (triangle) � couleurs vari�es
        { { -0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        { {  0.0f, -0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f, 1.0f } }
    };

    // D�finition des indices (chaque face est un triangle)
    // Faces lat�rales
    // Face 1 : apex, sommet 1, sommet 2
    // Face 2 : apex, sommet 2, sommet 3
    // Face 3 : apex, sommet 3, sommet 1
    // Base (face inf�rieure) : sommet 1, sommet 2, sommet 3
    std::vector<UINT16> tetraIndices = {
        0, 1, 2,  // Face lat�rale 1
        0, 2, 3,  // Face lat�rale 2
        0, 3, 1,  // Face lat�rale 3
        1, 2, 3   // Base
    };

    Create(device, tetraVertices, tetraIndices);
}

////////////////////

bool Mesh::CreateBuffers(Microsoft::WRL::ComPtr<ID3D12Device> device)
{
    // Cr�ation du vertex buffer
    UINT vertexBufferSize = static_cast<UINT>(vertices.size()) * sizeof(Vertex);
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

    if (FAILED(device.Get()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertexBuffer)))) 
    {
        OutputDebugStringA("Failed to create vertex buffer");
        return false;
    }

    // Copier les donn�es des sommets dans le vertex buffer
    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, vertices.data(), vertexBufferSize);
    vertexBuffer->Unmap(0, nullptr);

    // Initialiser la vue du vertex buffer
    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.StrideInBytes = sizeof(Vertex);
    vertexBufferView.SizeInBytes = vertexBufferSize;

    // Cr�ation de l'index buffer
    UINT indexBufferSize = static_cast<UINT>(indices.size()) * sizeof(UINT16);
    bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);

    if (FAILED(device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&indexBuffer)))) 
    {
        OutputDebugStringA("Failed to create index buffer");
        return false;
    }

    // Copier les donn�es des indices dans l'index buffer
    UINT8* pIndexDataBegin = nullptr;
    indexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin));
    memcpy(pIndexDataBegin, indices.data(), indexBufferSize);
    indexBuffer->Unmap(0, nullptr);

    // Initialiser la vue de l'index buffer
    indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
    indexBufferView.Format = DXGI_FORMAT_R16_UINT;
    indexBufferView.SizeInBytes = indexBufferSize;

    return true;
}
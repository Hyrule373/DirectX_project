#pragma once
#include "Mesh.h"
#include "ConstantBuffer.h"

struct RenderableObject {
    std::shared_ptr<Mesh> mesh;          // Le mesh à dessiner
    DirectX::XMMATRIX worldMatrix;       // La matrice monde de l'objet
    ConstantBuffer constantBuffer;       // Le constant buffer pour cet objet
};
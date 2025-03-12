#pragma once

class Transform  {
public:
    DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f }; // Position dans la scène
    DirectX::XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f }; // Rotation (en radians)
    DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };    // Échelle

    void SetPosition(float x, float y, float z) {
        position = { x, y, z };
    }

    void SetRotation(float pitch, float yaw, float roll) {
        rotation = { pitch, yaw, roll };
    }

    void SetScale(float sx, float sy, float sz) {
        scale = { sx, sy, sz };
    }

    DirectX::XMMATRIX GetWorldMatrix() const {
        DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
        DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
        DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

        return scaleMatrix * rotationMatrix * translationMatrix;
    }
};
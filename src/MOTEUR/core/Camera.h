#pragma once

class Camera
{
public:
    Camera();
    ~Camera();

    void Update(float aspectRatio);
    void SetPosition(DirectX::XMFLOAT3& position);
    void SetLookAt(DirectX::XMFLOAT3& target);
    void Rotate(float radX, float radY);

    DirectX::XMFLOAT3 GetPosition() const { return m_position; };
    DirectX::XMMATRIX GetViewMatrix() const { return XMLoadFloat4x4(&m_view); }

    DirectX::XMMATRIX GetProjectionMatrix() const { return XMLoadFloat4x4(&m_projection); }

private:

    DirectX::XMFLOAT4X4 m_view;
    DirectX::XMFLOAT4X4 m_projection;
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_target;
    DirectX::XMFLOAT3 m_up;
    float m_fov;
    float m_nearZ;
    float m_farZ;
};
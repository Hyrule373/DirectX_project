#include "pch.h"
#include "Camera.h"
#include <DirectXMath.h>

struct TRANSFORM {

};

using namespace DirectX;

Camera::Camera() :
    m_position(0.0f, 2.0f, -5.0f),
    m_target(0.0f, 0.0f, 0.0f),
    m_up(0.0f, 1.0f, 0.0f),
    m_fov(XM_PIDIV4),
    m_nearZ(0.01f),
    m_farZ(100.0f)
{
}

Camera::~Camera() {

}
#include <Windows.h> // N�cessaire pour les fonctions GetCursorPos, SetCursorPos, ShowCursor

#include <Windows.h> // N�cessaire pour GetCursorPos, SetCursorPos, ShowCursor

void Camera::Update(float aspectRatio)
{
    // Mise � jour de la matrice vue et projection
    XMMATRIX view = XMMatrixLookAtLH(
        XMLoadFloat3(&m_position),
        XMLoadFloat3(&m_target),
        XMLoadFloat3(&m_up)
    );

    XMMATRIX proj = XMMatrixPerspectiveFovLH(
        m_fov,
        aspectRatio,
        m_nearZ,
        m_farZ
    );

    XMStoreFloat4x4(&m_view, view);
    XMStoreFloat4x4(&m_projection, proj);

    // R�cup�rer la position de la souris
    POINT mousePos;
    GetCursorPos(&mousePos);

    // D�finir le seuil pour recadrer la souris au centre de l'�cran (ex: 10 pixels du bord)
    const int borderThreshold = 10;

    // Si la souris d�passe les bords de l'�cran, la recentrer
    if (mousePos.x < borderThreshold || mousePos.x > 1900 - borderThreshold || mousePos.y < borderThreshold || mousePos.y > 1000 - borderThreshold)
    {
        SetCursorPos(1920 / 2, 1080 / 2); // Recentrer la souris au centre de l'�cran
        mousePos.x = 1920 / 2;  // Mettre � jour la position de la souris pour calculer correctement les deltas
        mousePos.y = 1080 / 2;
        return;
    }

    // Calculer le delta de la souris
    static POINT prevMousePos = mousePos; // initialisation lors du premier appel
    float deltaX = static_cast<float>(mousePos.x - prevMousePos.x);
    float deltaY = static_cast<float>(mousePos.y - prevMousePos.y);

    // D�finir la sensibilit� de la rotation
    const float sensitivity = 0.002f;

    // Appliquer la rotation en fonction du mouvement de la souris
    Rotate(deltaY * sensitivity, deltaX * sensitivity);

    // Mettre � jour la position pr�c�dente pour la prochaine frame
    prevMousePos = mousePos;

    // Calculer la direction avant de la cam�ra (en fonction de la direction cible)
    XMVECTOR forward = XMVectorSubtract(XMLoadFloat3(&m_target), XMLoadFloat3(&m_position));
    forward = XMVector3Normalize(forward);

    // Calculer la direction droite de la cam�ra (perpendiculaire � l'axe de la vue)
    XMVECTOR right = XMVector3Cross(XMLoadFloat3(&m_up), forward); // inverser l'ordre du croisement
    right = XMVector3Normalize(right);

    // D�placement avec les touches
    float moveSpeed = 0.1f; // Vitesse de d�placement
    if (GetAsyncKeyState('Z') & 0x8000) {
        // Avancer selon l'orientation actuelle de la cam�ra
        XMVECTOR newPosition = XMVectorAdd(XMLoadFloat3(&m_position), forward * moveSpeed);
        XMStoreFloat3(&m_position, newPosition); // Convertir XMVECTOR en XMFLOAT3
        SetPosition(m_position);
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        // Reculer selon l'orientation actuelle de la cam�ra
        XMVECTOR newPosition = XMVectorSubtract(XMLoadFloat3(&m_position), forward * moveSpeed);
        XMStoreFloat3(&m_position, newPosition); // Convertir XMVECTOR en XMFLOAT3
        SetPosition(m_position);
    }
    if (GetAsyncKeyState('Q') & 0x8000) {
        // Se d�placer � gauche selon l'orientation actuelle (mouvement lat�ral)
        XMVECTOR newPosition = XMVectorSubtract(XMLoadFloat3(&m_position), right * moveSpeed);
        XMStoreFloat3(&m_position, newPosition); // Convertir XMVECTOR en XMFLOAT3
        SetPosition(m_position);
    }
    if (GetAsyncKeyState('D') & 0x8000) {
        // Se d�placer � droite selon l'orientation actuelle (mouvement lat�ral)
        XMVECTOR newPosition = XMVectorAdd(XMLoadFloat3(&m_position), right * moveSpeed);
        XMStoreFloat3(&m_position, newPosition); // Convertir XMVECTOR en XMFLOAT3
        SetPosition(m_position);
    }
	ShowCursor(false); // Cacher le curseur
}





void Camera::SetPosition(XMFLOAT3& position)
{
    m_position = position;
}

void Camera::SetLookAt(XMFLOAT3& target)
{
    m_target = target;
}

void Camera::Rotate(float radX, float radY)
{
    // Calcule la direction actuelle de la cam�ra
    XMVECTOR direction = XMVectorSubtract(XMLoadFloat3(&m_target), XMLoadFloat3(&m_position));
    direction = XMVector3Normalize(direction);

    // Cr�er les matrices de rotation
    XMMATRIX rotationX = XMMatrixRotationX(radX);
    XMMATRIX rotationY = XMMatrixRotationY(radY);

    // Appliquer les rotations � la direction
    direction = XMVector3TransformNormal(direction, rotationX * rotationY);

    XMStoreFloat3(&m_target, XMVectorAdd(XMLoadFloat3(&m_position), direction));
}
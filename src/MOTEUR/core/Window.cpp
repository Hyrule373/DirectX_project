#include "pch.h"
#include "Window.h"

Window::Window(){
}

Window::~Window() {
    if (hwnd) {
        DestroyWindow(hwnd);
    }
}

bool Window::Initialize(HINSTANCE hInstance, int nCmdShow, int width, int height, const std::wstring& title) {
    this->width = width;
    this->height = height;

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"DX12WindowClass";

    if (!RegisterClassEx(&wc)) 
    {
        throw std::runtime_error("Failed to register window class");
        return false;
    }

    RECT windowRect = { 0, 0, width, height };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    hwnd = CreateWindow(
        wc.lpszClassName,
        title.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (!hwnd) {
        throw std::runtime_error("Failed to create window");
        return false;
    }

    ShowWindow(hwnd, nCmdShow);

    return true;
}

LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}
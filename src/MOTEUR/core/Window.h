#pragma once
#include <windows.h>

class Window {
public:
    Window();
    ~Window();

    bool Initialize(HINSTANCE hInstance, int nCmdShow, int width, int height, const std::wstring& title);

    HWND GetHandle() const { return hwnd; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

private:
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    HWND hwnd;
    int width;
    int height;
};
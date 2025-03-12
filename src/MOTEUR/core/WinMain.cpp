#include "pch.h"
#include "Game.h"
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    try {
        Game game;
        if ( game.Initialize(hInstance, nCmdShow) )
            game.Run();
    }
    catch (const std::exception& e) {
        MessageBox(nullptr, std::wstring(e.what(), e.what() + strlen(e.what())).c_str(), L"Error", MB_OK);
        return -1;
    }
    return 0;
}

//Pour faire fonctionner
// -Mettre en entrée Window
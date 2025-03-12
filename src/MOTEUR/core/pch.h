#pragma once

// Librairies Windows et DirectX
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

// Librairies standard C++
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <memory>
#include <typeindex>
#include <stdexcept>
#include "Config.h"

// Librairies utilitaires
#include <wrl.h>
#include "../include2/d3dx12.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DEBUG_NEW
#endif

const UINT kFrameCount = 3;  // Nombre de frames dans la swap chain


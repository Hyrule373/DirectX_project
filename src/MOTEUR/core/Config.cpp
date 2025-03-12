#include "pch.h"
#include "Config.h"

void CreateConfigFile(const std::wstring& filePath) {
    std::wofstream configFile(filePath);
    if (!configFile) {
        return;
    }

#ifdef _DEBUG
    configFile << L"[Paths]\n";
    configFile << L"Icon=../../../res/logo.ico\n";
    configFile << L"Shader=../../../src/MOTEUR/shaders/shaders.hlsl\n";
    configFile << L"PostProcessing=../../../src/MOTEUR/shaders/postprocess.hlsl\n";
#else
    configFile << L"[Paths]\n";
    configFile << L"Icon=assets/images/logo.ico\n";
    configFile << L"Shader=assets/shaders/shaders.hlsl\n";
    configFile << L"PostProcessing=assets/shaders/postprocess.hlsl\n";
#endif

    configFile.close();
}

std::wstring GetConfigValue(const std::wstring& section,
    const std::wstring& key,
    const std::wstring& defaultValue,
    const std::wstring& filePath) {
    if (!std::filesystem::exists(filePath)) {
        CreateConfigFile(filePath);
    }

    wchar_t buffer[512];
    GetPrivateProfileStringW(section.c_str(), key.c_str(), defaultValue.c_str(),
        buffer, 512, filePath.c_str());
    return std::wstring(buffer);
}
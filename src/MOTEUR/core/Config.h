#pragma once

std::wstring GetConfigValue(const std::wstring& section,
    const std::wstring& key,
    const std::wstring& defaultValue,
    const std::wstring& filePath);
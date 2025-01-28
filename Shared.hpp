#pragma once
#include <stdint.h>
#include <Windows.h>
#include <string>
#include <filesystem>
#include <ShlObj.h>

namespace Shared
{
    inline uint32_t DllReferenceCount{};
    inline HMODULE DllHandle{};

    inline constexpr auto ClsidContextMenuHandler = CLSID{ 0x75AC4B8C, 0x74C5, 0x48AD, {0x93, 0x81, 0xAD, 0x1E, 0x80, 0x47, 0x33, 0x5C } };
    inline constexpr auto FriendlyClassName = L"ContextMenuHandler.FriendlyName Class";
    inline constexpr auto FriendlyMenuName = L"ContextMenuHandler.FriendlyName";
    inline constexpr auto ThreadingModel = L"Apartment";
    inline constexpr auto JsonFileName = "FileTransporter.json";

    [[nodiscard]] inline std::wstring GetModulename()
    {
        wchar_t moduleName[MAX_PATH]{};
        if (GetModuleFileNameW(Shared::DllHandle, moduleName, MAX_PATH) == 0)
        {
            return L"";
        }

        return moduleName;
    }

    [[nodiscard]] inline std::filesystem::path GetJsonFilePath()
    {
        wchar_t* AppdataFolderPath = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &AppdataFolderPath)))
        {
            const auto Result = std::filesystem::path(AppdataFolderPath) / "FileTransporter" / JsonFileName;
            CoTaskMemFree(AppdataFolderPath);
            return Result;
        }

        CoTaskMemFree(AppdataFolderPath);
        return {};
    }
}
#pragma once
#include <stdint.h>
#include <Windows.h>
#include <string>

namespace Shared
{
    inline uint32_t DllReferenceCount{};
    inline HMODULE DllHandle{};

    inline constexpr auto ClsidContextMenuHandler = CLSID{ 0x75AC4B8C, 0x74C5, 0x48AD, {0x93, 0x81, 0xAD, 0x1E, 0x80, 0x47, 0x33, 0x5C } };
    inline constexpr auto FriendlyClassName = L"ContextMenuHandler.FriendlyName Class";
    inline constexpr auto FriendlyMenuName = L"ContextMenuHandler.FriendlyName";
    inline constexpr auto ThreadingModel = L"Apartment";
    inline constexpr auto JsonFilePath = L"C:/Users/PC/AppData/Local/FileTransporter/FileTransporter.json";

    inline std::wstring GetModulename()
    {
        wchar_t moduleName[MAX_PATH]{};
        if (GetModuleFileNameW(Shared::DllHandle, moduleName, MAX_PATH) == 0)
        {
            return L"";
        }

        return moduleName;
    }
}
#pragma once
#include <combaseapi.h>
#include <filesystem>
#include <ShlObj.h>
#include <atlbase.h>

namespace FileTransporter
{
    namespace ComUtils
    {
        [[nodiscard]] std::filesystem::path GetShellItemPath(const CComPtr<IShellItem>& ShellItem);
        [[nodiscard]] CComPtr<IShellItem> GetShellItemFromPath(const std::filesystem::path& Path);
        [[nodiscard]] bool MoveToDirectory(const ATL::CComPtr<IShellItemArray>& SelectedElements, const CComPtr<IShellItem>& Directory);
        [[nodiscard]] bool CopyItems(const ATL::CComPtr<IShellItemArray>& SelectedElements, const CComPtr<IShellItem>& Directory);
        [[nodiscard]] CComPtr<IShellItem> GetChoiceDirectory();
        [[nodiscard]] bool Contains(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::filesystem::path& Path);
    }
}
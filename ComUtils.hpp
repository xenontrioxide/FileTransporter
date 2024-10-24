#pragma once
#include <combaseapi.h>
#include <filesystem>
#include <ShlObj.h>
#include <atlbase.h>

namespace FileTransporter
{
    namespace ComUtils
    {
        std::filesystem::path GetShellItemPath(const CComPtr<IShellItem>& ShellItem);
        CComPtr<IShellItem> GetShellItemFromPath(const std::filesystem::path& Path);
        bool MoveToDirectory(const ATL::CComPtr<IShellItemArray>& SelectedElements, const CComPtr<IShellItem>& Directory);
        bool CopyItems(const ATL::CComPtr<IShellItemArray>& SelectedElements, const CComPtr<IShellItem>& Directory);
        CComPtr<IShellItem> GetChoiceDirectory();
        bool Contains(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::filesystem::path& Path);
    }
}
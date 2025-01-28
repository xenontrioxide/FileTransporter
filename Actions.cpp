#include "Actions.hpp"
#include <Windows.h>
#include <atlbase.h>
#include <ShlObj.h>
#include <filesystem>
#include "ComUtils.hpp"
#include "FileOperations.hpp"
#include "Shared.hpp"

namespace FileTransporter
{
    namespace Actions
    {
        bool MoveToParentDirectory(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem)
        {
            if (!SelectedElements || !CurrentItem)
                return false;

            DWORD Count{};
            const auto CountResult = SelectedElements->GetCount(&Count);
            if (!Count)
                return false;

            CComPtr<IShellItem> FirstSelectedItem{};
            const auto GetResult = SelectedElements->GetItemAt(0, &FirstSelectedItem);
            if (!SUCCEEDED(GetResult))
                return false;
            
            const auto SelectedPath = ComUtils::GetShellItemPath(FirstSelectedItem);
            if (!std::filesystem::exists(SelectedPath))
                return false;

            if (!SelectedPath.has_parent_path())
                return false;

            if (!SelectedPath.parent_path().has_parent_path())
                return false;

            std::filesystem::path ParentPath = SelectedPath.parent_path().parent_path();
            if (!std::filesystem::exists(ParentPath))
                return false;

            CComPtr<IShellItem> ParentShellItem = ComUtils::GetShellItemFromPath(ParentPath);
            if (!ParentShellItem)
                return false;

            return ComUtils::MoveToDirectory(SelectedElements, ParentShellItem);
        }

        bool MoveToPinnedFolder(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem)
        {
            if (!SelectedElements || !CurrentItem)
                return false;

            DWORD Count{};
            const auto CountResult = SelectedElements->GetCount(&Count);
            if (!Count)
                return false;

            const auto SelectedPinnedFolderDirectory = std::filesystem::path(CurrentItem->GetMenuItemText());
            if (!std::filesystem::exists(SelectedPinnedFolderDirectory))
                return false;

            CComPtr<IShellItem> SelectedPinnedFolder = ComUtils::GetShellItemFromPath(SelectedPinnedFolderDirectory);
            if (!SelectedPinnedFolder)
                return false;

            return ComUtils::MoveToDirectory(SelectedElements, SelectedPinnedFolder);
        }

        bool MoveToRelativeFolder(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem)
        {
            if (!SelectedElements || !CurrentItem)
                return false;

            DWORD Count{};
            const auto CountResult = SelectedElements->GetCount(&Count);
            if (!Count)
                return false;

            CComPtr<IShellItem> FirstSelectedItem{};
            const auto GetResult = SelectedElements->GetItemAt(0, &FirstSelectedItem);
            if (!SUCCEEDED(GetResult))
                return false;

            const auto SelectedPath = ComUtils::GetShellItemPath(FirstSelectedItem);
            if (!std::filesystem::exists(SelectedPath))
                return false;

            if (!SelectedPath.has_parent_path())
                return false;
            
            std::filesystem::path CurrentDirectoryPath = SelectedPath.parent_path();
            if (!std::filesystem::exists(CurrentDirectoryPath))
                return false;

            std::filesystem::path ChosenDirectory = CurrentDirectoryPath / CurrentItem->GetMenuItemText();
            if (!std::filesystem::exists(CurrentDirectoryPath))
                return false;

            CComPtr<IShellItem> ChosenFolder = ComUtils::GetShellItemFromPath(ChosenDirectory);
            if (!ChosenFolder)
                return false;

            return ComUtils::MoveToDirectory(SelectedElements, ChosenFolder);
        }

        bool MoveToChoiceDirectory(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& /*CurrentItem*/)
        {
            if (!SelectedElements)
                return false;
            const auto ChoiceDirectory = ComUtils::GetChoiceDirectory();
            return ComUtils::MoveToDirectory(SelectedElements, ChoiceDirectory);
        }

        bool BackupSelectedElements(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& /*CurrentItem*/)
        {
            if (!SelectedElements)
                return false;

            DWORD Count{};
            const auto CountResult = SelectedElements->GetCount(&Count);
            if (!Count)
                return false;

            CComPtr<IShellItem> FirstSelectedItem{};
            const auto GetResult = SelectedElements->GetItemAt(0, &FirstSelectedItem);
            if (!SUCCEEDED(GetResult))
                return false;

            const auto SelectedPath = ComUtils::GetShellItemPath(FirstSelectedItem);
            if (!std::filesystem::exists(SelectedPath))
                return false;

            if (!SelectedPath.has_parent_path())
                return false;

            std::filesystem::path CurrentDirectoryPath = SelectedPath.parent_path();
            if (!std::filesystem::exists(CurrentDirectoryPath))
                return false;

            CComPtr<IShellItem> ChosenFolder = ComUtils::GetShellItemFromPath(CurrentDirectoryPath);
            if (!ChosenFolder)
                return false;

            return ComUtils::CopyItems(SelectedElements, ChosenFolder);
        }

        bool PinSelectedDirectory(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& /*CurrentItem*/)
        {
            if (!SelectedElements)
                return false;

            DWORD Count{};
            const auto CountResult = SelectedElements->GetCount(&Count);
            if (!Count)
                return false;

            CComPtr<IShellItem> FirstSelectedItem{};
            const auto GetResult = SelectedElements->GetItemAt(0, &FirstSelectedItem);
            if (!SUCCEEDED(GetResult))
                return false;

            const auto SelectedFolder = ComUtils::GetShellItemPath(FirstSelectedItem);
            if (!std::filesystem::exists(SelectedFolder))
                return false;

            FileTransporter::FileOperations::SavePinnedFolderToJson(SelectedFolder, Shared::GetJsonFilePath());
            return true;
        }

        bool UnpinSelectedDirectory(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& /*CurrentItem*/)
        {
            if (!SelectedElements)
                return false;

            DWORD Count{};
            const auto CountResult = SelectedElements->GetCount(&Count);
            if (!Count)
                return false;

            CComPtr<IShellItem> FirstSelectedItem{};
            const auto GetResult = SelectedElements->GetItemAt(0, &FirstSelectedItem);
            if (!SUCCEEDED(GetResult))
                return false;

            const auto SelectedFolder = ComUtils::GetShellItemPath(FirstSelectedItem);
            if (!std::filesystem::exists(SelectedFolder))
                return false;

            FileTransporter::FileOperations::RemovePinnedFolderFromJson(SelectedFolder, Shared::GetJsonFilePath());
            return true;
        }
    }
}
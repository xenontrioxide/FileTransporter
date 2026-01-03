#pragma once
#include <vector>
#include <string>
#include <atlbase.h>

#include "MenuItem.hpp"



namespace FileTransporter
{
    namespace Actions
    {
        [[nodiscard]] bool MoveToParentDirectory(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem);
        [[nodiscard]] bool MoveToPinnedFolder(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem);
        [[nodiscard]] bool MoveToRelativeFolder(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem);
        [[nodiscard]] bool MoveToChoiceDirectory(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem);
        [[nodiscard]] bool BackupSelectedElements(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem);
        [[nodiscard]] bool PinSelectedDirectory(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem);
        [[nodiscard]] bool UnpinSelectedDirectory(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem);
    }
}
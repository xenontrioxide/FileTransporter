#pragma once
#include <vector>
#include <string>
#include <atlbase.h>

#include "MenuItem.hpp"



namespace FileTransporter
{
    namespace Actions
    {
        bool MoveToParentDirectory(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem);

        bool MoveToPinnedFolder(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem);

        bool MoveToRelativeFolder(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem);

        bool MoveToChoiceDirectory(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem);

        bool BackupSelectedElements(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem);

        bool PinSelectedDirectory(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem);

        bool UnpinSelectedDirectory(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem);
    }
}
#include "MenuItem.hpp"
#include <Windows.h>

namespace FileTransporter
{
    //MenuItem
    MenuItem::MenuItem(const std::wstring& MenuItemText, ActionType Action) : MenuItemText(MenuItemText), Action(Action)
    {

    }

    std::wstring MenuItem::GetMenuItemText()
    {
        return MenuItemText;
    }

    std::vector<std::shared_ptr<MenuItemBase>> MenuItem::GetChildren()
    {
        return { };
    }

    uint32_t MenuItem::GetMask()
    {
        return MIIM_STRING | MIIM_ID;
    }

    uint32_t MenuItem::GetType()
    {
        return 0;
    }

    bool MenuItem::ExecuteAction(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem)
    {
        return Action(SelectedElements, CurrentItem);
    }
    //MenuItem


    //SubMenuItem
    SubMenuItem::SubMenuItem(const std::wstring& MenuItemText) : MenuItemText(MenuItemText)
    {

    }

    void SubMenuItem::AddChild(const std::shared_ptr<MenuItemBase>& Child)
    {
        Children.push_back(Child);
    }

    std::wstring SubMenuItem::GetMenuItemText()
    {
        return MenuItemText;
    }

    std::vector<std::shared_ptr<MenuItemBase>> SubMenuItem::GetChildren()
    {
        return Children;
    }

    uint32_t SubMenuItem::GetMask()
    {
        return MIIM_SUBMENU | MIIM_STRING | MIIM_ID;
    }

    uint32_t SubMenuItem::GetType()
    {
        return 0;
    }

    bool SubMenuItem::ExecuteAction(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem)
    {
        return false;
    }
    //SubMenuItem

    //SeparatorMenuItem
    SeparatorMenuItem::SeparatorMenuItem()
    {

    }

    std::wstring SeparatorMenuItem::GetMenuItemText()
    {
        return L"Null";
    }

    std::vector<std::shared_ptr<MenuItemBase>> SeparatorMenuItem::GetChildren()
    {
        return {};
    }

    uint32_t SeparatorMenuItem::GetMask()
    {
        return MIIM_TYPE;
    }

    uint32_t SeparatorMenuItem::GetType()
    {
        return MFT_SEPARATOR;
    }

    bool SeparatorMenuItem::ExecuteAction(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem)
    {
        return false;
    }
    //SeparatorMenuItem
}
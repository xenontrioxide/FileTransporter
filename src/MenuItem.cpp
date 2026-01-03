#include "MenuItem.hpp"
#include <Windows.h>

namespace FileTransporter
{
    //MenuItem
    MenuItem::MenuItem(const std::wstring& MenuItemText, ActionType Action) : MenuItemText(MenuItemText), Action(Action)
    {

    }

    [[nodiscard]] std::wstring MenuItem::GetMenuItemText() const
    {
        return MenuItemText;
    }

    [[nodiscard]] std::vector<std::shared_ptr<MenuItemBase>> MenuItem::GetChildren() const
    {
        return { };
    }

    [[nodiscard]] uint32_t MenuItem::GetMask() const
    {
        return MIIM_STRING | MIIM_ID;
    }

    [[nodiscard]] uint32_t MenuItem::GetType() const
    {
        return 0;
    }

    [[nodiscard]] bool MenuItem::ExecuteAction(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem)
    {
        return Action(SelectedElements, CurrentItem);
    }

    MenuItem::~MenuItem()
    {

    }
    //MenuItem


    //SubMenuItem
    SubMenuItem::SubMenuItem(const std::wstring& MenuItemText) : MenuItemText(MenuItemText)
    {

    }

    [[nodiscard]] void SubMenuItem::AddChild(const std::shared_ptr<MenuItemBase>& Child)
    {
        Children.push_back(Child);
    }

    [[nodiscard]] std::wstring SubMenuItem::GetMenuItemText() const
    {
        return MenuItemText;
    }

    [[nodiscard]] std::vector<std::shared_ptr<MenuItemBase>> SubMenuItem::GetChildren() const
    {
        return Children;
    }

    [[nodiscard]] uint32_t SubMenuItem::GetMask() const
    {
        return MIIM_SUBMENU | MIIM_STRING | MIIM_ID;
    }

    [[nodiscard]] uint32_t SubMenuItem::GetType() const
    {
        return 0;
    }

    [[nodiscard]] bool SubMenuItem::ExecuteAction(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem)
    {
        return false;
    }

    SubMenuItem::~SubMenuItem()
    {

    }
    //SubMenuItem

    //SeparatorMenuItem
    SeparatorMenuItem::SeparatorMenuItem()
    {

    }

    [[nodiscard]] std::wstring SeparatorMenuItem::GetMenuItemText() const
    {
        return L"Null";
    }

    [[nodiscard]] std::vector<std::shared_ptr<MenuItemBase>> SeparatorMenuItem::GetChildren() const
    {
        return {};
    }

    [[nodiscard]] uint32_t SeparatorMenuItem::GetMask() const
    {
        return MIIM_TYPE;
    }

    [[nodiscard]] uint32_t SeparatorMenuItem::GetType() const
    {
        return MFT_SEPARATOR;
    }

    [[nodiscard]] bool SeparatorMenuItem::ExecuteAction(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem)
    {
        return false;
    }

    SeparatorMenuItem::~SeparatorMenuItem()
    {

    }
    //SeparatorMenuItem
}
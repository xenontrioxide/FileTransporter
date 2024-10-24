#pragma once
#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <atlbase.h>
#include <ShlObj.h>

namespace FileTransporter
{
    struct MenuItemBase
    {
        virtual std::wstring GetMenuItemText() = 0;
        virtual std::vector<std::shared_ptr<MenuItemBase>> GetChildren() = 0;
        virtual uint32_t GetMask() = 0;
        virtual uint32_t GetType() = 0;
        virtual bool ExecuteAction(const ATL::CComPtr<IShellItemArray>&, const std::shared_ptr<MenuItemBase>&) = 0;
    };

    struct MenuItem : MenuItemBase
    {
        using ActionType = std::function<bool(const ATL::CComPtr<IShellItemArray>&, const std::shared_ptr<MenuItemBase>&)>;

        std::wstring MenuItemText{ };
        ActionType Action{ };

        MenuItem(const std::wstring& MenuItemText, ActionType Action = [](const ATL::CComPtr<IShellItemArray>&, const std::shared_ptr<FileTransporter::MenuItemBase>&) { return false; });

        virtual std::wstring GetMenuItemText() override;
        virtual std::vector<std::shared_ptr<MenuItemBase>> GetChildren() override;
        virtual uint32_t GetMask() override;
        virtual uint32_t GetType() override;
        virtual bool ExecuteAction(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem) override;
    };

    struct SubMenuItem : MenuItemBase
    {
        std::wstring MenuItemText{ };
        std::vector<std::shared_ptr<MenuItemBase>> Children{ };

        SubMenuItem(const std::wstring& MenuItemText);

        void AddChild(const std::shared_ptr<MenuItemBase>& Child);

        std::wstring GetMenuItemText() override;
        virtual std::vector<std::shared_ptr<MenuItemBase>> GetChildren() override;
        virtual uint32_t GetMask() override;
        virtual uint32_t GetType() override;
        virtual bool ExecuteAction(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem) override;
    };

    struct SeparatorMenuItem : MenuItemBase
    {
        SeparatorMenuItem();

        virtual std::wstring GetMenuItemText() override;
        virtual std::vector<std::shared_ptr<MenuItemBase>> GetChildren() override;
        virtual uint32_t GetMask() override;
        virtual uint32_t GetType() override;
        virtual bool ExecuteAction(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem) override;
    };
}
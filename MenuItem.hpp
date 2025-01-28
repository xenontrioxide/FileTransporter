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
        [[nodiscard]] virtual std::wstring GetMenuItemText() const = 0;
        [[nodiscard]] virtual std::vector<std::shared_ptr<MenuItemBase>> GetChildren() const = 0;
        [[nodiscard]] virtual uint32_t GetMask() const = 0;
        [[nodiscard]] virtual uint32_t GetType() const = 0;
        [[nodiscard]] virtual bool ExecuteAction(const ATL::CComPtr<IShellItemArray>&, const std::shared_ptr<MenuItemBase>&) = 0;
        virtual ~MenuItemBase() {};
    };

    struct MenuItem : MenuItemBase
    {
        using ActionType = std::function<bool(const ATL::CComPtr<IShellItemArray>&, const std::shared_ptr<MenuItemBase>&)>;

        std::wstring MenuItemText{ };
        ActionType Action{ };

        [[nodiscard]] MenuItem(const std::wstring& MenuItemText, ActionType Action = [](const ATL::CComPtr<IShellItemArray>&, const std::shared_ptr<FileTransporter::MenuItemBase>&) { return false; });
        [[nodiscard]] virtual std::wstring GetMenuItemText() const override;
        [[nodiscard]] virtual std::vector<std::shared_ptr<MenuItemBase>> GetChildren() const override;
        [[nodiscard]] virtual uint32_t GetMask() const override;
        [[nodiscard]] virtual uint32_t GetType() const override;
        [[nodiscard]] virtual bool ExecuteAction(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem) override;
        virtual ~MenuItem();
    };

    struct SubMenuItem : MenuItemBase
    {
        std::wstring MenuItemText{ };
        std::vector<std::shared_ptr<MenuItemBase>> Children{ };

        [[nodiscard]] SubMenuItem(const std::wstring& MenuItemText);
        void AddChild(const std::shared_ptr<MenuItemBase>& Child);
        [[nodiscard]] std::wstring GetMenuItemText() const override;
        [[nodiscard]] virtual std::vector<std::shared_ptr<MenuItemBase>> GetChildren() const override;
        [[nodiscard]] virtual uint32_t GetMask() const override;
        [[nodiscard]] virtual uint32_t GetType() const override;
        [[nodiscard]] virtual bool ExecuteAction(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem) override;
        virtual ~SubMenuItem();
    };

    struct SeparatorMenuItem : MenuItemBase
    {
        [[nodiscard]] SeparatorMenuItem();
        [[nodiscard]] virtual std::wstring GetMenuItemText() const override;
        [[nodiscard]] virtual std::vector<std::shared_ptr<MenuItemBase>> GetChildren() const override;
        [[nodiscard]] virtual uint32_t GetMask() const override;
        [[nodiscard]] virtual uint32_t GetType() const override;
        [[nodiscard]] virtual bool ExecuteAction(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::shared_ptr<MenuItemBase>& CurrentItem) override;
        virtual ~SeparatorMenuItem();
    };
}
#pragma once
#include <windows.h>
#include <shlobj.h>
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <filesystem>
#include <format>

#include "FileOperations.hpp"

#include <atlbase.h>
#include "Shared.hpp"
#include "MenuItem.hpp"

class ContextMenuHandler : public IShellExtInit, public IContextMenu
{
public:
    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IShellExtInit
    IFACEMETHODIMP Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID);
    void CreateMenus();

    // IContextMenu
    IFACEMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pici);
    IFACEMETHODIMP GetCommandString(UINT_PTR idCommand, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax);

    void InsertSubMenu(HMENU hMenu, const std::shared_ptr<FileTransporter::MenuItemBase>& item, UINT& uID, UINT& index);

    ContextMenuHandler();

protected:
    ~ContextMenuHandler();

private:
    // Reference count of component.
    uint32_t RefCount;
    
    std::unordered_map<uint32_t, std::shared_ptr<FileTransporter::MenuItemBase>> Actions{};
    std::shared_ptr<FileTransporter::SubMenuItem> Head;
    ATL::CComPtr<IShellItemArray> SelectedShellItems{};
};
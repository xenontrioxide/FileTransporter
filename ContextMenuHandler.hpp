#pragma once
#include <windows.h>
#include <shlobj.h>
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <filesystem>

#include "FileOperations.hpp"
struct SubMenu
{
    std::wstring Title;
    std::shared_ptr<SubMenu> Parent;
    std::vector<std::shared_ptr<SubMenu>> Children;
    std::uintptr_t HandlerAddress; //Using the address as a key that will be used in the handler later
    bool TrailingSeperator;
    /// <summary>
    /// Creates a SubMenu object
    /// </summary>
    /// <param name="Title">The Title of the Menu</param>
    /// <param name="Parent">The pointer to the parent SubMenu, this is never used and can be removed</param>
    /// <param name="Children">Vector of the Children SubMenus, Supporting nested SubMenus</param>
    /// <param name="HandlerAddress">The Address of the function that will be called when the child is clicked, if the SubMenu has children it can not have a handler</param>
    SubMenu(const std::wstring& Title, const std::shared_ptr<SubMenu>& Parent, const std::vector<std::shared_ptr<SubMenu>>& Children, const std::uintptr_t& HandlerAddress, const bool TrailingSeperator = false) :
        Title(Title), Parent(Parent), Children(Children), HandlerAddress(HandlerAddress), TrailingSeperator(TrailingSeperator)
    {
    }
};

class ContextMenuHandler : public IShellExtInit, public IContextMenu
{
public:
    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IShellExtInit
    IFACEMETHODIMP Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID);

    // IContextMenu
    IFACEMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pici);
    IFACEMETHODIMP GetCommandString(UINT_PTR idCommand, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax);

    ContextMenuHandler();

protected:
    ~ContextMenuHandler();

private:
    // Reference count of component.
    uint32_t RefCount;

    std::vector<std::wstring> SelectedElements{};
    
    std::vector<std::shared_ptr<SubMenu>> RegisteredHandlers{};

    // Initialize menu structure
    std::shared_ptr<SubMenu> MainMenu{};
    std::shared_ptr<SubMenu> BackupMenu{};
    std::shared_ptr<SubMenu> MoveMenu{};
    std::shared_ptr<SubMenu> ParentFolder{};
    std::shared_ptr<SubMenu> ChooseMenu{};
};

inline void ChooseDirectory(const std::vector<std::wstring>& SelectedElements, const std::filesystem::path& dest)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr))
    {
        return;
    }

    IFileDialog* pFileDialog = NULL;
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileDialog, reinterpret_cast<void**>(&pFileDialog));
    if (FAILED(hr))
    {
        CoUninitialize();
        return;
    }

    DWORD dwOptions;
    hr = pFileDialog->GetOptions(&dwOptions);
    if (SUCCEEDED(hr))
    {
        hr = pFileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);
    }

    if (FAILED(hr))
    {
        pFileDialog->Release();
        CoUninitialize();
        return;
    }

    hr = pFileDialog->Show(NULL);
    if (FAILED(hr))
    {
        pFileDialog->Release();
        CoUninitialize();
        return;
    }

    IShellItem* pItem;
    hr = pFileDialog->GetResult(&pItem);
    if (FAILED(hr))
    {
        pFileDialog->Release();
        CoUninitialize();
        return;
    }

    PWSTR pszFilePath = NULL;
    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
    if (FAILED(hr))
    {
        pItem->Release();
        pFileDialog->Release();
        CoUninitialize();
        return;
    }

    CoTaskMemFree(pszFilePath);
    pItem->Release();
    pFileDialog->Release();
    CoUninitialize();

    MoveFiles(SelectedElements, pszFilePath);
}

inline void PinFolder(const std::wstring& SelectedElement)
{
    auto Json = LoadFromDisk(Shared::JsonFilePath);
    std::vector<std::string> PinnedFolders = Json["PinnedFolders"].get<std::vector<std::string>>();
    PinnedFolders.push_back(std::filesystem::path(SelectedElement).generic_string());
    Json["PinnedFolders"] = PinnedFolders;
    SaveToDisk(Shared::JsonFilePath, Json);
}

inline void UnpinFolder(const std::wstring& SelectedElement)
{
    auto Json = LoadFromDisk(Shared::JsonFilePath);
    std::vector<std::string> PinnedFolders = Json["PinnedFolders"].get<std::vector<std::string>>();
    PinnedFolders.push_back(std::filesystem::path(SelectedElement).generic_string());
    auto ToRemove = std::remove_if(PinnedFolders.begin(), PinnedFolders.end(), [&](const std::string& Entry) { return std::filesystem::path(Entry) == std::filesystem::path(SelectedElement); });
    PinnedFolders.erase(ToRemove, PinnedFolders.end());
    Json["PinnedFolders"] = PinnedFolders;
    SaveToDisk(Shared::JsonFilePath, Json);
}

/// <summary>
/// The global Handler where all the buttons will route through
/// </summary>
/// <param name="ID">The ID of the button clicked, this is also the Index for the RegisteredHandlers</param>
/// <param name="SelectedElements">The Folders/Files that were selected</param>
/// <param name="RegisteredHandlers">All of the registered handlers that hold the Submenu details.</param>
/// <returns>S_OK on Success E_FAIL on Failure</returns>
inline uint32_t Handler(const uint32_t ID, std::vector<std::wstring>& SelectedElements, std::vector<std::shared_ptr<SubMenu>>& RegisteredHandlers)
{
    if (ID < 0 || ID >= RegisteredHandlers.size())
        return E_FAIL;

    const auto& RegisteredHandler = RegisteredHandlers[ID];
    if (!RegisteredHandler)
        return E_FAIL;

    if (!SelectedElements.size())
        return E_FAIL;

    if (RegisteredHandler->HandlerAddress == reinterpret_cast<std::uintptr_t>(BackupFiles))
    {
        const auto Dest = GetDestinationParent(SelectedElements[0]);
        BackupFiles(SelectedElements, Dest);
        return S_OK;
    }
    else if (RegisteredHandler->HandlerAddress == reinterpret_cast<std::uintptr_t>(MoveFiles))
    {
        const auto DestParent = GetDestinationParent(SelectedElements[0], RegisteredHandler->Title);
        const auto Dest = DestParent / (RegisteredHandler->Title != L".." ? RegisteredHandler->Title : L"");
        MoveFiles(SelectedElements, Dest);
        return S_OK;
    }
    else if (RegisteredHandler->HandlerAddress == reinterpret_cast<std::uintptr_t>(ChooseDirectory))
    {
        const auto DestParent = GetDestinationParent(SelectedElements[0], RegisteredHandler->Title);
        const auto Dest = DestParent / (RegisteredHandler->Title != L".." ? RegisteredHandler->Title : L"");
        ChooseDirectory(SelectedElements, Dest);
        return S_OK;
    }
    else if (RegisteredHandler->HandlerAddress == reinterpret_cast<std::uintptr_t>(PinFolder))
    {
        PinFolder(SelectedElements[0]);
        return S_OK;
    }
    else if (RegisteredHandler->HandlerAddress == reinterpret_cast<std::uintptr_t>(UnpinFolder))
    {
        UnpinFolder(SelectedElements[0]);
        return S_OK;
    }
    else
    {
        MessageBoxW(0, L"No Handler For This Was Assigned, Take a look at Handler", L"Handler Not Found", MB_OK);
        return E_FAIL;
    }
}
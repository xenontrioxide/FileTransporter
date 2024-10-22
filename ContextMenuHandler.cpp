#include <strsafe.h>
#include <Shlwapi.h>
#include <ShlObj_core.h>
#include <shellapi.h>
#include <format>
#include <fstream>
#include <ostream>

#include "shared.hpp"
#include "ContextMenuHandler.hpp"
#include "nlohmann/json.hpp"
#include <atlbase.h>

#pragma comment(lib, "shlwapi.lib")



ContextMenuHandler::ContextMenuHandler()
    : RefCount(1)
{
    _InterlockedIncrement(&Shared::DllReferenceCount);
}

ContextMenuHandler::~ContextMenuHandler()
{
    _InterlockedDecrement(&Shared::DllReferenceCount);
}

IFACEMETHODIMP ContextMenuHandler::QueryInterface(REFIID riid, void** ppv)
{
    static const QITAB qit[] =
    {
        QITABENT(ContextMenuHandler, IContextMenu),
        QITABENT(ContextMenuHandler, IShellExtInit),
        { 0 },
    };
    return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) ContextMenuHandler::AddRef()
{
    return _InterlockedIncrement(&RefCount);
}

IFACEMETHODIMP_(ULONG) ContextMenuHandler::Release()
{
    ULONG Refs = _InterlockedDecrement(&RefCount);
    if (!Refs && this) //I could not figure out why deleting this was invalid on release && this fixed it.
    {                  //It also only happened when I included ATL::ComPtr<IShellItemArray> and filled it.
        delete this;
    }
    return Refs;
}

bool Contains(const std::vector<std::wstring>& Paths, const std::wstring& FolderPath)
{
    for (const auto& File : Paths)
    {
        if (std::filesystem::path(File) == std::filesystem::path(FolderPath))
            return true;
    }

    return false;
}

bool Contains(const std::vector<std::string>& Paths, const std::wstring& FolderPath)
{
    for (const auto& File : Paths)
    {
        if (std::filesystem::path(File) == std::filesystem::path(FolderPath))
            return true;
    }

    return false;
}

IFACEMETHODIMP ContextMenuHandler::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID)
{
    if (!pDataObj)
    {
        return E_INVALIDARG;
    }

    
    //const auto CreationResult = SHCreateShellItemArrayFromDataObject(pDataObj, IID_PPV_ARGS(&SelectedShellItems));
    //if (!SUCCEEDED(CreationResult))
    //{
    //    return E_FAIL;
    //}

    

    FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM Stm;

    // The pDataObj pointer contains the objects being acted upon. In this 
    // example, we get an HDROP handle for enumerating the selected files and 
    // folders.
    const auto GetDataStatus = pDataObj->GetData(&fe, &Stm);
    if (!SUCCEEDED(GetDataStatus))
    {
        return E_FAIL;
    }

    HDROP HandleDrop = static_cast<HDROP>(GlobalLock(Stm.hGlobal));
    if (!HandleDrop)
    {
        ReleaseStgMedium(&Stm);
        return E_FAIL;
    }

    uint32_t NumberFiles = DragQueryFileW(HandleDrop, 0xFFFFFFFF, NULL, 0);
    if (NumberFiles < 1)
        return E_FAIL;

    //First we collect all the files that are selected
    for (uint32_t i = 0; i < NumberFiles; i++)
    {
        wchar_t FilePath[MAX_PATH]{ 0 };
        if (DragQueryFileW(HandleDrop, i, FilePath, MAX_PATH))
        {
            SelectedElements.push_back(FilePath);
        }
    }

    CreateMenus();

    GlobalUnlock(Stm.hGlobal);
    ReleaseStgMedium(&Stm);



    return S_OK;
}

void ContextMenuHandler::CreateMenus()
{
    MainMenu = std::make_shared<SubMenu>(L"FileTransporter", nullptr, std::vector<std::shared_ptr<SubMenu>>{}, 0x0);
    BackupMenu = std::make_shared<SubMenu>(L"Backup", MainMenu, std::vector<std::shared_ptr<SubMenu>>{}, reinterpret_cast<std::uintptr_t>(BackupFiles));
    MoveMenu = std::make_shared<SubMenu>(L"Move To", MainMenu, std::vector<std::shared_ptr<SubMenu>>{}, 0x0);
    MainMenu->Children.push_back(BackupMenu);
    MainMenu->Children.push_back(MoveMenu);

    // .. operation should only exist if there is a parent path to move to
    const auto SelectedElement = std::filesystem::path(SelectedElements[0]);
    if (SelectedElement.has_parent_path())
    {
        const auto SelectedElementParentPath = SelectedElement.parent_path();
        if (SelectedElementParentPath != SelectedElement.root_path()) //This is super weird check but std::filesystem::path("C:\").parent_path() == "C:\" && std::filesystem::path("C:\").has_parent_path() == true which is not the behavior we want.
        {
            ParentFolder = std::make_shared<SubMenu>(L"..", MoveMenu, std::vector<std::shared_ptr<SubMenu>>{}, reinterpret_cast<std::uintptr_t>(MoveFiles));
            MoveMenu->Children.push_back(ParentFolder);
        }
    }

    //If the config folder doesn't exist, create the folder and the json inside of it with empty data.
    const auto ConfigSaveFolder = std::filesystem::path(Shared::JsonFilePath).parent_path();
    if (!std::filesystem::exists(ConfigSaveFolder))
    {
        std::filesystem::create_directory(ConfigSaveFolder);
        nlohmann::json Json;
        Json["PinnedFolders"] = std::vector<std::string>();
        SaveToDisk(Shared::JsonFilePath, Json);
    }

    const auto Json = LoadFromDisk(Shared::JsonFilePath);
    std::vector<std::string> PinnedFolders = Json["PinnedFolders"].get<std::vector<std::string>>();
    for (const auto& Pin : PinnedFolders)
    {
        MoveMenu->Children.push_back(std::make_shared<SubMenu>(std::filesystem::path(Pin).generic_wstring(), MoveMenu, std::vector<std::shared_ptr<SubMenu>>{}, reinterpret_cast<std::uintptr_t>(MoveFiles)));
    }

    ChooseMenu = std::make_shared<SubMenu>(L"Choose...", MoveMenu, std::vector<std::shared_ptr<SubMenu>>{}, reinterpret_cast<std::uintptr_t>(ChooseDirectory), true);
    MoveMenu->Children.push_back(ChooseMenu);

    if (SelectedElements.size() == 1) // If we have just one item selected
    {
        const auto Json = LoadFromDisk(Shared::JsonFilePath);
        std::vector<std::string> PinnedFolders = Json["PinnedFolders"].get<std::vector<std::string>>();

        //We only want folders for the pinning / unpinning
        const auto& Path = SelectedElements[0];
        if (std::filesystem::is_directory(Path) && Contains(PinnedFolders, Path))
        {
            MainMenu->Children.push_back(std::make_shared<SubMenu>(L"Unpin", MainMenu, std::vector<std::shared_ptr<SubMenu>>{}, reinterpret_cast<std::uintptr_t>(UnpinFolder)));
        }
        else if (std::filesystem::is_directory(Path) && !Contains(PinnedFolders, Path))
        {
            MainMenu->Children.push_back(std::make_shared<SubMenu>(L"Pin", MainMenu, std::vector<std::shared_ptr<SubMenu>>{}, reinterpret_cast<std::uintptr_t>(PinFolder)));
        }
    }

    //We can't send a folder inside of itself so we want to only add the ones that aren't selected
    {
        const auto FolderPath = std::filesystem::path(SelectedElements[0]).parent_path();
        for (auto const& File : std::filesystem::directory_iterator(FolderPath))
        {
            if (std::filesystem::is_directory(File) && !Contains(SelectedElements, File.path().generic_wstring()))
            {
                MoveMenu->Children.push_back(std::make_shared<SubMenu>(File.path().filename(), MoveMenu, std::vector<std::shared_ptr<SubMenu>>{}, reinterpret_cast<std::uintptr_t>(MoveFiles)));
            }
        }
    }
}

void InsertSubMenu(HMENU hMenu, std::shared_ptr<SubMenu> menu, UINT& uID, UINT idCmdFirst, std::vector<std::shared_ptr<SubMenu>>& RegisteredHandlers, bool isMainMenu, UINT& indexMenu)
{
    HMENU hSubMenu = menu->Children.size() ? CreatePopupMenu() : nullptr;

    // Insert child menus first
    for (size_t i = 0; i < menu->Children.size(); i++)
    {
        InsertSubMenu(hSubMenu, menu->Children[i], uID, idCmdFirst, RegisteredHandlers, false, indexMenu);
    }


    // Insert this menu
    MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
    mii.fMask = menu->Children.size() ? MIIM_SUBMENU | MIIM_STRING | MIIM_ID : MIIM_STRING | MIIM_ID;
    mii.wID = menu->Children.size() ? uID : uID++; //If the SubMenu has children it doesn't get an ID
    mii.hSubMenu = hSubMenu;
    mii.dwTypeData = const_cast<wchar_t*>(menu->Title.c_str());

    //If the SubMenu has a Handler(has a function when clicked) we add it to the registered handlers to be acted on in InvokeCommand, the order does matter
    //The index is later used to indentify which handler / button was pushed
    if (menu->HandlerAddress)
    {
        RegisteredHandlers.push_back(menu);
    }

    if (isMainMenu)
    {
        if (!InsertMenuItemW(hMenu, indexMenu++, true, &mii))
        {
            MessageBoxA(nullptr, std::to_string(GetLastError()).c_str(), std::to_string(GetLastError()).c_str(), MB_OK);
        }
    }
    else
    {
        if (!InsertMenuItemW(hMenu, indexMenu++, true, &mii)) //-1 always inserts at the end
        {
            MessageBoxA(nullptr, std::to_string(GetLastError()).c_str(), std::to_string(GetLastError()).c_str(), MB_OK);
        }
    }

    if (menu->TrailingSeperator)
    {
        MENUITEMINFOW sep = { sizeof(sep) };
        sep.fMask = MIIM_TYPE;
        sep.fType = MFT_SEPARATOR;
        if (!InsertMenuItem(hMenu, uID + 1, TRUE, &sep))
        {
        }
    }
}

wchar_t test[] = L"TestVerb";

IFACEMETHODIMP ContextMenuHandler::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
    if (CMF_DEFAULTONLY & uFlags)
    {
        return E_FAIL;
    }

    //DWORD ItemCount{};
    //const auto CountResult = SelectedShellItems->GetCount(&ItemCount);
    //if (!SUCCEEDED(CountResult))
    //    return E_FAIL;
    //
    //for (int i = 0u; i < ItemCount; i++)
    //{
    //    CComPtr<IShellItem> Item;
    //    SelectedShellItems->GetItemAt(i, &Item);
    //
    //    
    //    WCHAR* StringData = (WCHAR*)CoTaskMemAlloc(MAX_PATH * sizeof(WCHAR));
    //    Item->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &StringData);
    //
    //    MessageBoxW(nullptr, StringData, StringData, MB_OK);
    //
    //    CoTaskMemFree(StringData);
    //}

    UINT uID = idCmdFirst;
    InsertSubMenu(hMenu, MainMenu, uID, idCmdFirst, RegisteredHandlers, true, indexMenu);
    const auto Offset = uID - idCmdFirst + 1;
    MessageBoxW(nullptr, std::to_wstring(Offset).c_str(), std::to_wstring(Offset).c_str(), MB_OK);
    return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, Offset);
}

IFACEMETHODIMP ContextMenuHandler::InvokeCommand(LPCMINVOKECOMMANDINFO pici)
{
    if (!pici)
    {
        return E_INVALIDARG;
    }

    const auto Word = LOWORD(pici->lpVerb);
    return Handler(Word, SelectedElements, RegisteredHandlers);
}

IFACEMETHODIMP ContextMenuHandler::GetCommandString(UINT_PTR idCommand, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
{
    return E_FAIL;
}
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

#include "Actions.hpp"
#include "ComUtils.hpp"

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

    GlobalUnlock(Stm.hGlobal);
    ReleaseStgMedium(&Stm);

    //Windows for some reason has a selection of 1 when you just open FileExplorer.
    const auto CreationResult = SHCreateShellItemArrayFromDataObject(pDataObj, IID_PPV_ARGS(&SelectedShellItems));
    if (!SUCCEEDED(CreationResult))
    {
        return E_FAIL;
    }

    DWORD Count{};
    const auto GetCountResult = SelectedShellItems->GetCount(&Count);
    if (!SUCCEEDED(GetCountResult))
        return E_FAIL;

    if (!Count)
        return E_FAIL;

    CreateMenus();
    return S_OK;
}

void ContextMenuHandler::CreateMenus()
{
    DWORD Count{};
    const auto GetCountResult = SelectedShellItems->GetCount(&Count);
    if (!SUCCEEDED(GetCountResult))
        return;

    CComPtr<IShellItem> FirstSelectedItem = nullptr;
    const auto GetItemResult = SelectedShellItems->GetItemAt(0, &FirstSelectedItem);
    if (!SUCCEEDED(GetItemResult))
        return;

    const auto SelectedElement = FileTransporter::ComUtils::GetShellItemPath(FirstSelectedItem);

    Head = std::make_shared<FileTransporter::SubMenuItem>(L"FileTransporter");
    const auto Backup = std::make_shared<FileTransporter::MenuItem>(L"Backup", FileTransporter::Actions::BackupSelectedElements);
    const auto MoveTo = std::make_shared<FileTransporter::SubMenuItem>(L"Move To");
    Head->AddChild(Backup);
    Head->AddChild(MoveTo);

    // .. operation should only exist if there is a parent path to move to
    if (SelectedElement.has_parent_path())
    {
        const auto SelectedElementParentPath = SelectedElement.parent_path();
        if (SelectedElementParentPath != SelectedElement.root_path()) //This is super weird check but std::filesystem::path("C:\").parent_path() == "C:\" && std::filesystem::path("C:\").has_parent_path() == true which is not the behavior we want.
        {
            const auto ParentMenu = std::make_shared<FileTransporter::MenuItem>(L"..", FileTransporter::Actions::MoveToParentDirectory);
            MoveTo->AddChild(ParentMenu);
        }
    }

    //If the config folder doesn't exist, create the folder and the json inside of it with empty data.
    const auto ConfigSaveFolder = std::filesystem::path(Shared::JsonFilePath).parent_path();
    if (!std::filesystem::exists(ConfigSaveFolder))
    {
        std::filesystem::create_directory(ConfigSaveFolder);
        nlohmann::json Json;
        Json["PinnedFolders"] = std::vector<std::string>();
        FileTransporter::FileOperations::SaveJsonToDisk(Shared::JsonFilePath, Json);
    }

    const auto Json = FileTransporter::FileOperations::LoadJsonFromDisk(Shared::JsonFilePath);
    std::vector<std::string> PinnedFolders = Json["PinnedFolders"].get<std::vector<std::string>>();
    for (const auto& Pin : PinnedFolders)
    {
        MoveTo->AddChild(std::make_shared<FileTransporter::MenuItem>(std::filesystem::path(Pin).generic_wstring(), FileTransporter::Actions::MoveToPinnedFolder));
    }

    MoveTo->AddChild(std::make_shared<FileTransporter::MenuItem>(L"Choose...", FileTransporter::Actions::MoveToChoiceDirectory));
    MoveTo->AddChild(std::make_shared<FileTransporter::SeparatorMenuItem>());

    if (Count == 1) // If we have just one item selected
    {
        const auto Json = FileTransporter::FileOperations::LoadJsonFromDisk(Shared::JsonFilePath);
        std::vector<std::string> PinnedFolders = Json["PinnedFolders"].get<std::vector<std::string>>();

        //We only want folders for the pinning / unpinning
        const auto Path = FileTransporter::ComUtils::GetShellItemPath(FirstSelectedItem);
        if (std::filesystem::is_directory(Path) && Contains(PinnedFolders, Path))
        {
            MoveTo->AddChild(std::make_shared<FileTransporter::MenuItem>(L"Unpin", FileTransporter::Actions::UnpinSelectedDirectory));
            MoveTo->AddChild(std::make_shared<FileTransporter::SeparatorMenuItem>());
        }
        else if (std::filesystem::is_directory(Path) && !Contains(PinnedFolders, Path))
        {
            MoveTo->AddChild(std::make_shared<FileTransporter::MenuItem>(L"Pin", FileTransporter::Actions::PinSelectedDirectory));
            MoveTo->AddChild(std::make_shared<FileTransporter::SeparatorMenuItem>());
        }
    }

    //We can't send a folder inside of itself so we want to only add the ones that aren't selected
    {
        const auto FolderPath = FileTransporter::ComUtils::GetShellItemPath(FirstSelectedItem).parent_path();
        for (auto const& File : std::filesystem::directory_iterator(FolderPath))
        {
            if (std::filesystem::is_directory(File) && !FileTransporter::ComUtils::Contains(SelectedShellItems, File.path().generic_wstring()))
            {
                MoveTo->AddChild(std::make_shared<FileTransporter::MenuItem>(File.path().filename(), FileTransporter::Actions::MoveToRelativeFolder));
            }
        }
    }
}

void ContextMenuHandler::InsertSubMenu(HMENU hMenu, const std::shared_ptr<FileTransporter::MenuItemBase>& item, UINT& uID, UINT& index)
{
    HMENU hSubMenu = item->GetChildren().size() ? CreatePopupMenu() : nullptr;

    const auto MenuTitle = item->GetMenuItemText();

    // Insert this menu
    MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
    mii.fMask = item->GetMask();
    mii.fType = item->GetType();
    mii.wID = uID;
    mii.hSubMenu = hSubMenu;
    mii.dwTypeData = const_cast<wchar_t*>(MenuTitle.c_str());

    if (!InsertMenuItemW(hMenu, index, true, &mii))
    {
        //MessageBoxA(nullptr, std::to_string(GetLastError()).c_str(), std::to_string(GetLastError()).c_str(), MB_OK);
    }

    Actions[index] = item;

    index++;
    uID++;

    for (size_t i = 0; i < item->GetChildren().size(); i++)
    {
        InsertSubMenu(hSubMenu, item->GetChildren()[i], uID, index);
    }
}

IFACEMETHODIMP ContextMenuHandler::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
    if (CMF_DEFAULTONLY & uFlags)
    {
        return E_FAIL;
    }

    UINT uID = idCmdFirst;
    InsertSubMenu(hMenu, Head, uID, indexMenu);
    const auto Offset = uID - idCmdFirst + 1;
    return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, Offset);
}

IFACEMETHODIMP ContextMenuHandler::InvokeCommand(LPCMINVOKECOMMANDINFO pici)
{
    if (!pici)
    {
        return E_INVALIDARG;
    }

    //Windows passes verbs in and you're expected to handle them.
    const auto Word = LOWORD(pici->lpVerb);
    if (!Actions.contains(Word))
        return E_FAIL;

    const auto& ChosenMenuItem = Actions[Word];
    if (!ChosenMenuItem)
        return E_FAIL;

    ChosenMenuItem->ExecuteAction(SelectedShellItems, ChosenMenuItem);
    return S_OK;
}

IFACEMETHODIMP ContextMenuHandler::GetCommandString(UINT_PTR idCommand, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
{
    return E_FAIL;
}
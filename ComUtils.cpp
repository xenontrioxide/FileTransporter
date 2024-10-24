#include "ComUtils.hpp"
#include "Shared.hpp"
#include "FileOperations.hpp"

namespace FileTransporter
{
    namespace ComUtils
    {
        std::filesystem::path GetShellItemPath(const CComPtr<IShellItem>& ShellItem)
        {
            LPWSTR SelectedFilePath{};
            const auto GetDisplayNameResult = ShellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &SelectedFilePath);
            if (!SUCCEEDED(GetDisplayNameResult))
            {
                //TODO: Log the error
                return {};
            }

            std::filesystem::path SelectedPath = SelectedFilePath;
            CoTaskMemFree(SelectedFilePath);
            return SelectedPath;
        }

        CComPtr<IShellItem> GetShellItemFromPath(const std::filesystem::path& Path)
        {
            CComPtr<IShellItem> ShellItem = nullptr;
            auto PathCopy = Path; //This is needed because make_preferred only works on non-const, converts path/a/b/c to path\\a\b\\c which this Windows function wants...
            const auto CreateItemResult = SHCreateItemFromParsingName(PathCopy.make_preferred().c_str(), nullptr, IID_PPV_ARGS(&ShellItem));
            return ShellItem;
        }

        bool MoveToDirectory(const ATL::CComPtr<IShellItemArray>& SelectedElements, const CComPtr<IShellItem>& Directory)
        {
            CComPtr<IFileOperation> FileOp; //If you rename this FileOperation, __uuidof(FileOperation) will grab yours and not the struct from Windows causing a failure
            const auto CreateFileOperationResult = CoCreateInstance(__uuidof(FileOperation), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&FileOp));
            if (CreateFileOperationResult != S_OK)
                return false;

            // Perform the move operation with the populated IShellItemArray
            const auto FileOperationMoveResult = FileOp->MoveItems(SelectedElements, Directory);
            if (!SUCCEEDED(FileOperationMoveResult))
                return false;

            const auto FileOperationResult = FileOp->PerformOperations();
            if (!SUCCEEDED(FileOperationResult))
                return false;

            return true;
        }

        bool CopyItems(const ATL::CComPtr<IShellItemArray>& SelectedElements, const CComPtr<IShellItem>& Directory)
        {
            CComPtr<IFileOperation> FileOp; //If you rename this FileOperation, __uuidof(FileOperation) will grab yours and not the struct from Windows causing a failure
            const auto CreateFileOperationResult = CoCreateInstance(__uuidof(FileOperation), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&FileOp));
            if (CreateFileOperationResult != S_OK)
                return false;

            // Perform the move operation with the populated IShellItemArray
            const auto FileOperationMoveResult = FileOp->CopyItems(SelectedElements, Directory);
            if (!SUCCEEDED(FileOperationMoveResult))
                return false;

            FileOp->SetOperationFlags(FOF_RENAMEONCOLLISION);

            const auto FileOperationResult = FileOp->PerformOperations();
            if (!SUCCEEDED(FileOperationResult))
                return false;

            return true;
        }

        CComPtr<IShellItem> GetChoiceDirectory()
        {
            CComPtr<IFileOpenDialog> FileDialog = nullptr;
            const auto CreateInstanceResult = CoCreateInstance(__uuidof(FileOpenDialog), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&FileDialog));
            if (CreateInstanceResult != S_OK)
                return nullptr;

            DWORD FileDialogOptions;
            const auto GetOptionsResult = FileDialog->GetOptions(&FileDialogOptions);
            if (!SUCCEEDED(GetOptionsResult))
            {
                return nullptr;
            }
            const auto SetOptionsResult = FileDialog->SetOptions(FileDialogOptions | FOS_PICKFOLDERS);
            if (!SUCCEEDED(SetOptionsResult))
            {
                return nullptr;
            }

            const auto ShowResult = FileDialog->Show(NULL);
            if (!SUCCEEDED(ShowResult))
            {
                return nullptr;
            }

            CComPtr<IShellItem> ShellItem = nullptr;
            const auto GetResultResult = FileDialog->GetResult(&ShellItem);
            if (!SUCCEEDED(GetResultResult))
            {
                return nullptr;
            }

            return ShellItem;
        }

        bool Contains(const ATL::CComPtr<IShellItemArray>& SelectedElements, const std::filesystem::path& Path)
        {
            DWORD Count{};
            const auto GetCountResult = SelectedElements->GetCount(&Count);
            if (!SUCCEEDED(GetCountResult))
                return false;

            for (auto i = 0u; i < Count; i++)
            {
                CComPtr<IShellItem> CurrentItem = nullptr;
                const auto GetItemResult = SelectedElements->GetItemAt(i, &CurrentItem);
                if (!SUCCEEDED(GetItemResult))
                    continue;

                if (GetShellItemPath(CurrentItem) == Path)
                    return true;
            }

            return false;
        }
    }
}
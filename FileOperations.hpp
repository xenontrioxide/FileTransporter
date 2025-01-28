#pragma once

#include <vector>
#include <memory>
#include <string>
#include <filesystem>
#include "nlohmann/json.hpp"

namespace FileTransporter
{
    namespace FileOperations
    {
        [[nodiscard]] nlohmann::json LoadJsonFromDisk(const std::filesystem::path& path);
        [[nodiscard]] bool SaveJsonToDisk(const std::filesystem::path& path, const nlohmann::json& json);
        void SavePinnedFolderToJson(const std::filesystem::path& SelectedElement, const std::filesystem::path& JsonFile);
        void RemovePinnedFolderFromJson(const std::filesystem::path& SelectedElement, const std::filesystem::path& JsonFile);
    }
}

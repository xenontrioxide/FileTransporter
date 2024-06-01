#pragma once

#include <vector>
#include <memory>
#include <string>
#include <filesystem>
#include "nlohmann/json.hpp"

nlohmann::json LoadFromDisk(const std::filesystem::path& path);
bool SaveToDisk(const std::filesystem::path& path, const nlohmann::json& json);
std::filesystem::path GetDestinationParent(const std::filesystem::path& path1, const std::filesystem::path& path2 = "");
void BackupFiles(const std::vector<std::wstring>& SelectedElements, const std::filesystem::path& dest);
void MoveFiles(const std::vector<std::wstring>& SelectedElements, const std::filesystem::path& dest);
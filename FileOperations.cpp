#include "FileOperations.hpp"
#include <istream>
#include <ostream>
#include <fstream>

namespace FileTransporter
{
    namespace FileOperations
    {
        [[nodiscard]] nlohmann::json LoadJsonFromDisk(const std::filesystem::path& path)
        {
            if (!std::filesystem::exists(path.parent_path()))
            {
                if (!std::filesystem::create_directory(path.parent_path()))
                {
                    //TODO: Add good logging
                    return {};
                }
            }

            std::ifstream file(path);
            if (file.fail() || !file.is_open() || file.peek() == std::ifstream::traits_type::eof())
            {
                //TODO: Add good logging
                return {};
            }

            nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
            if (json.is_discarded())
            {
                //TODO: Add good logging
                return {};
            }

            return json;
        }

        [[nodiscard]] bool SaveJsonToDisk(const std::filesystem::path& path, const nlohmann::json& json)
        {
            if (!std::filesystem::exists(path.parent_path()))
            {
                if (!std::filesystem::create_directory(path.parent_path()))
                {
                    //TODO: Add good logging
                    return false;
                }
            }

            std::ofstream file(path);
            if (file.fail() || !file.is_open())
            {
                //TODO: Add good logging
                return false;
            }

            file << std::setw(4) << json << std::endl;
            return true;
        }

        void SavePinnedFolderToJson(const std::filesystem::path& SelectedElement, const std::filesystem::path& JsonFile)
        {
            std::vector<std::string> PinnedFolders{};
            auto Json = LoadJsonFromDisk(JsonFile);
            if (!Json.is_null())
            {
                PinnedFolders = Json.value("PinnedFolders", std::vector<std::string>{});
            }
            PinnedFolders.push_back(SelectedElement.generic_string());
            Json["PinnedFolders"] = PinnedFolders;
            const auto Saved = SaveJsonToDisk(JsonFile, Json);
            if (!Saved)
            {
                //TODO: Add good logging.
            }
        }

        void RemovePinnedFolderFromJson(const std::filesystem::path& SelectedElement, const std::filesystem::path& JsonFile)
        {
            std::vector<std::string> PinnedFolders{};
            auto Json = LoadJsonFromDisk(JsonFile);
            if (!Json.is_null())
            {
                PinnedFolders = Json.value("PinnedFolders", std::vector<std::string>{});
            }
            PinnedFolders.push_back(SelectedElement.generic_string());
            auto ToRemove = std::remove_if(PinnedFolders.begin(), PinnedFolders.end(), [&](const std::string& Entry) { return std::filesystem::path(Entry) == std::filesystem::path(SelectedElement); });
            PinnedFolders.erase(ToRemove, PinnedFolders.end());
            Json["PinnedFolders"] = PinnedFolders;
            const auto Saved = SaveJsonToDisk(JsonFile, Json);
            if (!Saved)
            {
                //TODO: Add good logging.
            }
        }
    }
}
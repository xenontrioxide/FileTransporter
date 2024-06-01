#include "FileOperations.hpp"
#include <istream>
#include <ostream>
#include <fstream>

nlohmann::json LoadFromDisk(const std::filesystem::path& path)
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

bool SaveToDisk(const std::filesystem::path& path, const nlohmann::json& json)
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

std::filesystem::path GetDestinationParent(const std::filesystem::path& path1, const std::filesystem::path& path2)
{
    //outer\inner\inner ..
    //TODO: Check parent paths;
    if (path2.generic_string() == "..")
        return path1.parent_path().parent_path();

    //outer\inner\file.txt
    //outer\inner\folder

    return path1.parent_path();
}

void BackupFiles(const std::vector<std::wstring>& SelectedElements, const std::filesystem::path& Dest)
{
    const auto GetBackUpFileName = [&](const std::filesystem::path& Source)
        {
            if (std::filesystem::is_directory(Source))
            {
                return std::format("{}(backup)", Source.filename().generic_string());
            }
            else if (std::filesystem::is_regular_file(Source))
            {
                return std::format("{}.bak", Source.filename().generic_string());
            }
            else
            {
                return std::string("");
            }
        };

    for (const auto& File : SelectedElements)
    {
        std::filesystem::path Filepath(File);
        const auto BackupFileName = GetBackUpFileName(Filepath);
        if (BackupFileName.empty())
            return;

        const auto Destination = Dest / BackupFileName;

        std::filesystem::copy(Filepath, Destination, std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
    }
}

void MoveFiles(const std::vector<std::wstring>& SelectedElements, const std::filesystem::path& Dest)
{
    for (const auto& File : SelectedElements)
    {
        auto Desination = Dest;
        std::filesystem::path Filepath(File);
        if (std::filesystem::is_directory(Filepath))
        {
            Desination = Dest / Filepath.filename();
        }
        std::filesystem::copy(Filepath, Desination, std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
    }
}

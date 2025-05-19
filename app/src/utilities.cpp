//
// Created by user on 5/19/25.
//

#include "app/utilities.h"

#include <fstream>
#include <print>

namespace lgl
{
    std::string readAll(const std::filesystem::path& filepath)
    {
        std::ifstream file{ filepath, std::ios::binary };
        if (!file.is_open())
        {
            std::println(stderr, "Failed to open file at '{}'", filepath.string());
            throw std::runtime_error("Could not open file");
        }

        const auto fileSize{ std::filesystem::file_size(filepath) };
        if (fileSize == 0)
        {
            std::println(stderr, "Reading an empty file: {}", filepath.string());
            return {};
        }

        std::string content(fileSize, '\0');
        file.read(content.data(), static_cast<std::streamsize>(content.size()));
        return content;
    }
} // lgl

//
// Created by user on 5/19/25.
//

#include "app/Image.h"

namespace lgl
{
    TextureData loadImageAsTexture(const fs::path& filePath)
    {
        // Check if file exists and is regular file
        if (!fs::exists(filePath) || !fs::is_regular_file(filePath))
        {
            throw std::runtime_error("Path does not exist or is not a regular file");
        }

        // Read file header to detect format
        std::ifstream file{ filePath, std::ios::binary };
        if (!file)
        {
            throw std::runtime_error("Failed to open file");
        }

        std::array<std::byte, 8> header{};
        if (!file.read(reinterpret_cast<char*>(header.data()), header.size()))
        {
            throw std::runtime_error("Failed to read file header");
        }

        // Detect format and read image
        switch (detail::detectImageFormat(header))
        {
            case ImageFormat::JPEG:
                return detail::ImageReader<ImageFormat::JPEG>::read(filePath);
            case ImageFormat::PNG:
                return detail::ImageReader<ImageFormat::PNG>::read(filePath);
            default:
                throw std::runtime_error("Unsupported or unknown image format");
        }
    }
} // lgl

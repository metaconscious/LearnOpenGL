//
// Created by user on 5/19/25.
//

#include "app/Image.h"

#include <ranges>

namespace lgl
{
    bool ImageData::isValid() const noexcept
    {
        return width > 0 && height > 0 && channels > 0 && !data.empty();
    }

    std::span<const std::byte> ImageData::span() const noexcept
    {
        return data;
    }

    ImageData ImageData::flipVertically() const
    {
        if (!isValid())
        {
            return *this; // Nothing to flip
        }

        // Calculate row size in bytes
        const auto row_size{ width * channels };

        return {
            .data = std::views::iota(0u, height)
            | std::views::transform([this, row_size](const uint32_t row_idx)
            {
                // Original row index from bottom to top
                const auto flipped_idx{ height - 1 - row_idx };
                // Get the span for this row
                return std::span<const std::byte>{
                    data.data() + flipped_idx * row_size,
                    row_size
                };
            })
            | std::views::join
            | std::ranges::to<std::vector<std::byte>>(),
            .width = width,
            .height = height,
            .channels = channels,
            .pixelFormat = pixelFormat
        };
    }

    ImageData loadImage(const std::filesystem::path& filePath)
    {
        // Check if file exists and is regular file
        if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath))
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

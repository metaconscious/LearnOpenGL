//
// Created by user on 5/19/25.
//

#ifndef LEARNOPENGL_APP_IMAGE_H
#define LEARNOPENGL_APP_IMAGE_H

#include <boost/gil.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/io/png.hpp>

#include <cstddef>
#include <expected>
#include <filesystem>
#include <fstream>
#include <print>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace lgl
{
    // Image format enum for supported types
    enum class ImageFormat
    {
        Unknown,
        JPEG,
        PNG
        // Extensible for future formats
    };

    // Structure to hold texture data and metadata
    struct TextureData
    {
        std::vector<std::byte> data;
        uint32_t width{ 0 };
        uint32_t height{ 0 };
        uint8_t channels{ 0 };

        // Helper for OpenGL texture creation
        [[nodiscard]] std::span<const std::byte> span() const noexcept
        {
            return data;
        }
    };

    namespace detail
    {
        // Detect image format by examining file headers
        [[nodiscard]] constexpr ImageFormat detectImageFormat(std::span<const std::byte> header)
        {
            // JPEG signature: FF D8
            if (header.size() >= 2 &&
                static_cast<uint8_t>(header[0]) == 0xFF &&
                static_cast<uint8_t>(header[1]) == 0xD8)
            {
                return ImageFormat::JPEG;
            }

            // PNG signature: 89 50 4E 47 0D 0A 1A 0A
            if (header.size() >= 8 &&
                static_cast<uint8_t>(header[0]) == 0x89 &&
                static_cast<uint8_t>(header[1]) == 0x50 &&
                static_cast<uint8_t>(header[2]) == 0x4E &&
                static_cast<uint8_t>(header[3]) == 0x47 &&
                static_cast<uint8_t>(header[4]) == 0x0D &&
                static_cast<uint8_t>(header[5]) == 0x0A &&
                static_cast<uint8_t>(header[6]) == 0x1A &&
                static_cast<uint8_t>(header[7]) == 0x0A)
            {
                return ImageFormat::PNG;
            }

            return ImageFormat::Unknown;
        }

        // Format-specific image reader template base
        template<ImageFormat Format>
        struct ImageReader
        {
            static_assert(Format != ImageFormat::Unknown, "Unknown or unsupported image format");

            static TextureData read([[maybe_unused]] const std::filesystem::path& file_path)
            {
                throw std::runtime_error("Image format not implemented");
            }
        };

        // JPEG specialization
        template<>
        struct ImageReader<ImageFormat::JPEG>
        {
            static TextureData read(const std::filesystem::path& file_path)
            {
                boost::gil::rgb8_image_t img{};
                try
                {
                    boost::gil::read_image(file_path.string(), img, boost::gil::jpeg_tag{});
                }
                catch (const std::exception& e)
                {
                    std::println(stderr, "Failed to read JPEG: {}", e.what());
                    throw;
                }

                TextureData texture{
                    .data = {},
                    .width = static_cast<uint32_t>(img.width()),
                    .height = static_cast<uint32_t>(img.height()),
                    .channels = 3 // RGB
                };

                const auto total_size{ texture.width * texture.height * texture.channels };
                texture.data.resize(total_size);

                const auto view{ boost::gil::view(img) };
                std::size_t i{ 0 };
                for (const auto& pixel : view)
                {
                    texture.data[i++] = static_cast<std::byte>(boost::gil::get_color(pixel, boost::gil::red_t()));
                    texture.data[i++] = static_cast<std::byte>(boost::gil::get_color(pixel, boost::gil::green_t()));
                    texture.data[i++] = static_cast<std::byte>(boost::gil::get_color(pixel, boost::gil::blue_t()));
                }

                return texture;
            }
        };

        // PNG specialization
        template<>
        struct ImageReader<ImageFormat::PNG>
        {
            static TextureData read(const std::filesystem::path& filePath)
            {
                boost::gil::rgba8_image_t img{};
                try
                {
                    boost::gil::read_image(filePath.string(), img, boost::gil::png_tag{});
                }
                catch (const std::exception& e)
                {
                    std::println(stderr, "Failed to read PNG: {}", e.what());
                    throw;
                }

                TextureData texture{
                    .data = {},
                    .width = static_cast<uint32_t>(img.width()),
                    .height = static_cast<uint32_t>(img.height()),
                    .channels = 4
                };

                const auto total_size{ texture.width * texture.height * texture.channels };
                texture.data.resize(total_size);

                const auto view{ boost::gil::view(img) };
                std::size_t i{ 0 };
                for (const auto& pixel : view)
                {
                    texture.data[i++] = static_cast<std::byte>(boost::gil::get_color(pixel, boost::gil::red_t()));
                    texture.data[i++] = static_cast<std::byte>(boost::gil::get_color(pixel, boost::gil::green_t()));
                    texture.data[i++] = static_cast<std::byte>(boost::gil::get_color(pixel, boost::gil::blue_t()));
                    texture.data[i++] = static_cast<std::byte>(boost::gil::get_color(pixel, boost::gil::alpha_t()));
                }

                return texture;
            }
        };
    }

    /**
     * Loads an image file and converts it to a byte array suitable for OpenGL textures
     *
     * @param filePath Path to the image file
     * @return TextureData containing the image as bytes with metadata
     * @throws std::runtime_error if file is not a valid image or cannot be read
     */
    [[nodiscard]] TextureData loadImageAsTexture(const std::filesystem::path& filePath);

    // Helper method for if format is already known
    template<ImageFormat Format>
    [[nodiscard]] TextureData loadImageAsTexture(const std::filesystem::path& file_path)
    {
        if (!std::filesystem::exists(file_path) || !std::filesystem::is_regular_file(file_path))
        {
            throw std::runtime_error("Path does not exist or is not a regular file");
        }

        return detail::ImageReader<Format>::read(file_path);
    }
} // lgl

#endif //LEARNOPENGL_APP_IMAGE_H

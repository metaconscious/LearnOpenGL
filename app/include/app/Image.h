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

    enum class PixelFormat
    {
        Unknown,
        Red, // 1 channel (grayscale)
        RG, // 2 channels (grayscale + alpha)
        RGB, // 3 channels
        RGBA // 4 channels
    };

    // Structure to hold image data and metadata
    struct ImageData
    {
        std::vector<std::byte> data;
        uint32_t width{ 0 };
        uint32_t height{ 0 };
        uint8_t channels{ 0 };
        PixelFormat pixelFormat{ PixelFormat::Unknown }; // Add this
        [[nodiscard]] bool isValid() const noexcept;
        [[nodiscard]] std::span<const std::byte> span() const noexcept;
        [[nodiscard]] ImageData flipVertically() const;

        // Helper to get pixel format from channel count
        [[nodiscard]] static constexpr PixelFormat getPixelFormat(uint8_t channelCount) noexcept;
    };

    constexpr PixelFormat ImageData::getPixelFormat(const uint8_t channelCount) noexcept
    {
        switch (channelCount)
        {
            case 1:
                return PixelFormat::Red;
            case 2:
                return PixelFormat::RG;
            case 3:
                return PixelFormat::RGB;
            case 4:
                return PixelFormat::RGBA;
            default:
                return PixelFormat::Unknown;
        }
    }

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

            static ImageData read([[maybe_unused]] const std::filesystem::path& file_path)
            {
                throw std::runtime_error("Image format not implemented");
            }
        };

        // JPEG specialization
        template<>
        struct ImageReader<ImageFormat::JPEG>
        {
            static ImageData read(const std::filesystem::path& file_path)
            {
                // Use any_image to handle different channel counts
                boost::gil::any_image<
                    boost::gil::gray8_image_t,
                    boost::gil::rgb8_image_t
                > img;

                try
                {
                    boost::gil::read_image(file_path.string(), img, boost::gil::jpeg_tag{});
                }
                catch (const std::exception& e)
                {
                    std::println(stderr, "Failed to read JPEG: {}", e.what());
                    throw;
                }
                ImageData texture;

                // Use variant2::visit instead of apply_operation
                boost::variant2::visit([&texture]<typename T0>(T0&& typed_img)
                {
                    using ImageType = std::decay_t<T0>;
                    using PixelType = typename ImageType::value_type;

                    texture.width = static_cast<uint32_t>(typed_img.width());
                    texture.height = static_cast<uint32_t>(typed_img.height());
                    texture.channels = boost::gil::num_channels<PixelType>::value;
                    texture.pixelFormat = ImageData::getPixelFormat(texture.channels);

                    const auto total_size{ texture.width * texture.height * texture.channels };
                    texture.data.resize(total_size);

                    const auto view{ boost::gil::view(typed_img) };
                    std::size_t i{ 0 };

                    if constexpr (boost::gil::num_channels<PixelType>::value == 1)
                    {
                        // Grayscale
                        for (const auto& pixel : view)
                        {
                            // Extract the gray channel value
                            texture.data[i++] = static_cast<std::byte>(boost::gil::at_c<0>(pixel));
                        }
                    }
                    else if constexpr (boost::gil::num_channels<PixelType>::value == 3)
                    {
                        // RGB
                        for (const auto& pixel : view)
                        {
                            texture.data[i++] = static_cast<std::byte>(
                                boost::gil::get_color(pixel, boost::gil::red_t()));
                            texture.data[i++] = static_cast<std::byte>(boost::gil::get_color(
                                pixel, boost::gil::green_t()));
                            texture.data[i++] = static_cast<std::byte>(boost::gil::get_color(
                                pixel, boost::gil::blue_t()));
                        }
                    }
                }, img);

                return texture;
            }
        };

        // PNG specialization
        template<>
        struct ImageReader<ImageFormat::PNG>
        {
            static ImageData read(const std::filesystem::path& file_path)
            {
                // Use any_image to handle different PNG formats
                boost::gil::any_image<
                    boost::gil::gray8_image_t,
                    boost::gil::gray_alpha8_image_t,
                    boost::gil::rgb8_image_t,
                    boost::gil::rgba8_image_t
                > img;

                try
                {
                    boost::gil::read_image(file_path.string(), img, boost::gil::png_tag{});
                }
                catch (const std::exception& e)
                {
                    std::println(stderr, "Failed to read PNG: {}", e.what());
                    throw;
                }
                ImageData texture;

                // Use variant2::visit instead of apply_operation
                boost::variant2::visit([&texture]<typename T0>(T0&& typed_img)
                {
                    using ImageType = std::decay_t<T0>;
                    using PixelType = typename ImageType::value_type;

                    texture.width = static_cast<uint32_t>(typed_img.width());
                    texture.height = static_cast<uint32_t>(typed_img.height());
                    texture.channels = boost::gil::num_channels<PixelType>::value;
                    texture.pixelFormat = ImageData::getPixelFormat(texture.channels);

                    const auto total_size = texture.width * texture.height * texture.channels;
                    texture.data.resize(total_size);

                    const auto view = boost::gil::view(typed_img);
                    std::size_t i = 0;

                    if constexpr (boost::gil::num_channels<PixelType>::value == 1)
                    {
                        // Grayscale
                        for (const auto& pixel : view)
                        {
                            // Extract the gray channel value
                            texture.data[i++] = static_cast<std::byte>(boost::gil::at_c<0>(pixel));
                        }
                    }
                    else if constexpr (boost::gil::num_channels<PixelType>::value == 2)
                    {
                        // Grayscale + Alpha
                        for (const auto& pixel : view)
                        {
                            texture.data[i++] = static_cast<std::byte>(boost::gil::get_color(
                                pixel, boost::gil::gray_color_t()));
                            texture.data[i++] = static_cast<std::byte>(boost::gil::get_color(
                                pixel, boost::gil::alpha_t()));
                        }
                    }
                    else if constexpr (boost::gil::num_channels<PixelType>::value == 3)
                    {
                        // RGB
                        for (const auto& pixel : view)
                        {
                            texture.data[i++] = static_cast<std::byte>(
                                boost::gil::get_color(pixel, boost::gil::red_t()));
                            texture.data[i++] = static_cast<std::byte>(boost::gil::get_color(
                                pixel, boost::gil::green_t()));
                            texture.data[i++] = static_cast<std::byte>(boost::gil::get_color(
                                pixel, boost::gil::blue_t()));
                        }
                    }
                    else if constexpr (boost::gil::num_channels<PixelType>::value == 4)
                    {
                        // RGBA
                        for (const auto& pixel : view)
                        {
                            texture.data[i++] = static_cast<std::byte>(
                                boost::gil::get_color(pixel, boost::gil::red_t()));
                            texture.data[i++] = static_cast<std::byte>(boost::gil::get_color(
                                pixel, boost::gil::green_t()));
                            texture.data[i++] = static_cast<std::byte>(boost::gil::get_color(
                                pixel, boost::gil::blue_t()));
                            texture.data[i++] = static_cast<std::byte>(boost::gil::get_color(
                                pixel, boost::gil::alpha_t()));
                        }
                    }
                }, img);

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
    [[nodiscard]] ImageData loadImage(const std::filesystem::path& filePath);

    // Helper method for if format is already known
    template<ImageFormat Format>
    [[nodiscard]] ImageData loadImage(const std::filesystem::path& file_path)
    {
        if (!std::filesystem::exists(file_path) || !std::filesystem::is_regular_file(file_path))
        {
            throw std::runtime_error("Path does not exist or is not a regular file");
        }

        return detail::ImageReader<Format>::read(file_path);
    }
} // lgl

#endif //LEARNOPENGL_APP_IMAGE_H

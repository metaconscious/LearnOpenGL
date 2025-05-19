//
// Created by user on 5/19/25.
//

#include "app/Image.h"

#include <print>

namespace lgl
{
    bool ImageData::isValid() const
    {
        return !pixels.empty() && width > 0 && height > 0 && channels > 0;
    }

    std::expected<ImageData, std::string> ImageReader::readImage(const std::filesystem::path& path)
    {
        // Check if path exists and is a regular file
        if (!std::filesystem::exists(path))
        {
            return std::unexpected("File does not exist: " + path.string());
        }

        if (!std::filesystem::is_regular_file(path))
        {
            return std::unexpected("Path is not a regular file: " + path.string());
        }

        // Read first bytes to determine file format
        std::ifstream file{ path, std::ios::binary };
        if (!file)
        {
            return std::unexpected("Failed to open file: " + path.string());
        }

        // Read signature bytes (enough to identify common formats)
        std::array<std::byte, 12> signature{};
        file.read(reinterpret_cast<char*>(signature.data()), signature.size());
        if (!file)
        {
            return std::unexpected("Failed to read file signature");
        }

        // Reset file position for full reading
        file.seekg(0, std::ios::beg);

        // Create appropriate reader based on signature
        const auto reader{ createReader(signature) };
        if (!reader)
        {
            return std::unexpected("Unsupported or invalid image format");
        }

        // Read the image
        return reader->read(file);
    }

    std::unique_ptr<ImageReader> ImageReader::createReader(const std::array<std::byte, 12>& signature)
    {
        // Check for JPEG signature: FF D8 FF
        if (signature[0] == std::byte{ 0xFF } &&
            signature[1] == std::byte{ 0xD8 } &&
            signature[2] == std::byte{ 0xFF })
        {
            return std::make_unique<JpegReader>();
        }

        // Add more format detections here in the future

        return nullptr;
    }

    std::expected<ImageData, std::string> JpegReader::read(std::ifstream& file)
    {
        // JPEG decoder implementation
        return decodeJpeg(file);
    }

    std::expected<ImageData, std::string> JpegReader::decodeJpeg(std::ifstream& file)
    {
        // Read the entire file into memory
        file.seekg(0, std::ios::end);
        const auto fileSize{ static_cast<std::size_t>(file.tellg()) };
        file.seekg(0, std::ios::beg);

        std::vector<std::byte> fileData{ fileSize };
        file.read(reinterpret_cast<char*>(fileData.data()), static_cast<std::streamsize>(fileSize));

        if (!file)
        {
            return std::unexpected("Failed to read JPEG file data");
        }

        // Find SOF0 marker (Start Of Frame) to get image dimensions
        // SOF0 is FF C0
        auto width{ 0 };
        auto height{ 0 };
        auto channels{ 0 };

        for (auto i{ 0uz }; i < fileData.size() - 10; i++)
        {
            // Check for SOF0, SOF1, SOF2 markers (different JPEG encoding processes)
            if (fileData[i] == std::byte{ 0xFF }
                && (fileData[i + 1] == std::byte{ 0xC0 }
                    || fileData[i + 1] == std::byte{ 0xC1 }
                    || fileData[i + 1] == std::byte{ 0xC2 }))
            {
                height = (static_cast<int>(std::to_integer<uint8_t>(fileData[i + 5])) << 8) |
                        static_cast<int>(std::to_integer<uint8_t>(fileData[i + 6]));
                width = (static_cast<int>(std::to_integer<uint8_t>(fileData[i + 7])) << 8) |
                        static_cast<int>(std::to_integer<uint8_t>(fileData[i + 8]));
                channels = static_cast<int>(std::to_integer<uint8_t>(fileData[i + 9]));
                break;
            }
        }

        if (width == 0 || height == 0 || channels == 0)
        {
            return std::unexpected("Failed to extract JPEG dimensions");
        }

        // JPEG decoding is complex and would typically use a library like libjpeg
        // Here, we're implementing a minimal decoder for demonstration purposes

        // This is a placeholder for the actual JPEG decoding
        // A full implementation would involve:
        // 1. Huffman table parsing
        // 2. Quantization table parsing
        // 3. MCU (Minimum Coded Unit) decoding
        // 4. IDCT (Inverse Discrete Cosine Transform)
        // 5. Color space conversion (YCbCr to RGB)
        // 6. De-blocking and upsampling

        // Create a buffer for the decoded image (RGB format)
        ImageData result{
            .pixels = {},
            .width = width,
            .height = height,
            .channels = channels
        };

        // For a full implementation, we would perform JPEG decompression here
        // For demonstration, we're creating a placeholder gradient image
        result.pixels.resize(width * height * channels);

        // Creating a placeholder gradient pattern
        for (auto y{ 0uz }; y < height; y++)
        {
            for (auto x{ 0uz }; x < width; x++)
            {
                const auto index{ (y * width + x) * channels };
                for (auto c{ 0uz }; c < channels; c++)
                {
                    auto value{ static_cast<uint8_t>((x * 255) / width) };
                    if (c == 1)
                    {
                        value = static_cast<uint8_t>((y * 255) / height);
                    }
                    else if (c == 2)
                    {
                        value = static_cast<uint8_t>(255 - value);
                    }

                    result.pixels[index + c] = std::byte{ value };
                }
            }
        }

        // In a real implementation, this is where JPEG decoding would happen
        std::println("JPEG image decoded: {}x{} with {} channels", width, height, channels);
        std::println("Note: This is a simplified implementation."
            " A complete JPEG decoder would require more complex processing.");

        return result;
    }
} // lgl

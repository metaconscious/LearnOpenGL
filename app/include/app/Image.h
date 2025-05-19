//
// Created by user on 5/19/25.
//

#ifndef LEARNOPENGL_APP_IMAGE_H
#define LEARNOPENGL_APP_IMAGE_H

#include <cstddef>
#include <expected>
#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>

namespace lgl
{
    struct ImageData
    {
        std::vector<std::byte> pixels;
        int width = 0;
        int height = 0;
        int channels = 0; // Number of color channels (e.g., 3 for RGB, 4 for RGBA)

        // Convenience method to check if image data is valid
        [[nodiscard]] bool isValid() const;
    };

    class JpegReader;

    class ImageReader
    {
    public:
        virtual ~ImageReader() = default;

        // Main function to read image file into OpenGL-compatible byte array
        static std::expected<ImageData, std::string> readImage(const std::filesystem::path& path);

    protected:
        virtual std::expected<ImageData, std::string> read(std::ifstream& file) = 0;

    private:
        static std::unique_ptr<ImageReader> createReader(const std::array<std::byte, 12>& signature);
    };

    class JpegReader final : public ImageReader
    {
    protected:
        std::expected<ImageData, std::string> read(std::ifstream& file) override;

    private:
        static std::expected<ImageData, std::string> decodeJpeg(std::ifstream& file);
    };
} // lgl

#endif //LEARNOPENGL_APP_IMAGE_H

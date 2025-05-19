//
// Created by user on 5/19/25.
//

#ifndef LEARNOPENGL_APP_UTILITIES_H
#define LEARNOPENGL_APP_UTILITIES_H

#include <filesystem>
#include <string>

namespace lgl
{
    [[nodiscard]] std::string readAll(const std::filesystem::path& filepath);
} // lgl

#endif //LEARNOPENGL_APP_UTILITIES_H

//
// Created by user on 5/25/25.
//

#ifndef LEARNOPENGL_APP_CONSTANTS_H
#define LEARNOPENGL_APP_CONSTANTS_H

#include <glm/vec3.hpp>

namespace lgl
{
    // Constants to replace magic literals
    namespace constants
    {
        // Default camera values
        constexpr auto DEFAULT_YAW{ -90.0f };
        constexpr auto DEFAULT_PITCH{ 0.0f };
        constexpr auto DEFAULT_ROLL{ 0.0f };
        constexpr auto DEFAULT_MOVEMENT_SPEED{ 5.0f };
        constexpr auto DEFAULT_MOUSE_SENSITIVITY{ 0.1f };
        constexpr auto DEFAULT_FOV{ 45.0f };
        constexpr auto DEFAULT_ASPECT_RATIO{ 16.0f / 9.0f };
        constexpr auto DEFAULT_NEAR_PLANE{ 0.1f };
        constexpr auto DEFAULT_FAR_PLANE{ 1000.0f };
        constexpr auto DEFAULT_ORBIT_DISTANCE{ 10.0f };

        // Constraints
        constexpr auto MIN_PITCH{ -89.0f };
        constexpr auto MAX_PITCH{ 89.0f };
        constexpr auto MIN_FOV{ 1.0f };
        constexpr auto MAX_FOV{ 170.0f };
        constexpr auto MIN_ORBIT_DISTANCE{ 0.1f };

        // Vectors
        constexpr glm::vec3 WORLD_UP{ 0.0f, 1.0f, 0.0f };
        constexpr glm::vec3 WORLD_FORWARD{ 0.0f, 0.0f, -1.0f };
        constexpr glm::vec3 WORLD_RIGHT{ 1.0f, 0.0f, 0.0f };
        constexpr glm::vec3 DEFAULT_POSITION{ 0.0f, 0.0f, 3.0f };
    }
}

#endif //LEARNOPENGL_APP_CONSTANTS_H

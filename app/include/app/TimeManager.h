//
// Created by user on 5/28/25.
//

#ifndef LEARNOPENGL_APP_TIMEMANAGER_H
#define LEARNOPENGL_APP_TIMEMANAGER_H

#include <chrono>

namespace lgl
{
    class TimeManager
    {
    public:
        TimeManager();

        void update();
        float getDeltaTime() const;
        float getTotalTime() const;

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_lastFrameTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
        float m_deltaTime{0.0f};
        float m_totalTime{0.0f};
    };
} // namespace lgl

#endif //LEARNOPENGL_APP_TIMEMANAGER_H

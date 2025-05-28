//
// Created by user on 5/28/25.
//

#include "app/TimeManager.h"

namespace lgl
{
    TimeManager::TimeManager()
    {
        m_startTime = m_lastFrameTime = std::chrono::high_resolution_clock::now();
    }

    void TimeManager::update()
    {
        const auto currentTime{ std::chrono::high_resolution_clock::now() };
        m_deltaTime = std::chrono::duration<float>(currentTime - m_lastFrameTime).count();
        m_totalTime = std::chrono::duration<float>(currentTime - m_startTime).count();
        m_lastFrameTime = currentTime;
    }

    float TimeManager::getDeltaTime() const
    {
        return m_deltaTime;
    }

    float TimeManager::getTotalTime() const
    {
        return m_totalTime;
    }
} // namespace lgl

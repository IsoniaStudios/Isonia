#pragma once

// std
#include <iostream>

namespace Isonia::Debug
{
    class PerformanceTracker
    {
    public:
        void LogFrameTime(float frameTime)
        {
            // New frame
            ++frameCount;

            // Calculate and update average frame time
            totalFrameTime += frameTime;
            averageFrameTime = totalFrameTime / frameCount;

            // Update highest frame time
            highestFrameTime = std::max(highestFrameTime, frameTime);

            // Print performance statistics
            std::cout << "Frame Time: " << frameTime << " ms"
                      << " | Average Frame Time: " << averageFrameTime << " ms"
                      << " | Highest Frame Time: " << highestFrameTime << " ms"
                      << std::endl;
        }

    private:
        int frameCount = 0;
        float totalFrameTime = 0.0f;
        float averageFrameTime = 0.0f;
        float highestFrameTime = 0.0f;
    };
}

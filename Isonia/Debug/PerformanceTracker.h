#pragma once

// std
#include <iostream>
#include <iomanip>

namespace Isonia::Debug
{
    class PerformanceTracker
    {
    public:
        void LogFrameTime(float frameTime_s)
        {
            // Convert to ms
            float frameTime = frameTime_s * 1000.0f;

            // New frame
            ++frameCount;

            // Calculate and update average frame time
            totalFrameTime += frameTime;
            averageFrameTime = totalFrameTime / frameCount;

            // Update highest frame time
            highestFrameTime = std::max(highestFrameTime, frameTime);

            // Move the cursor to the beginning of the line
            std::cout << "\r";

            // Print performance statistics
            std::cout << std::fixed << std::setw(8) << std::setprecision(6)
                      << "Frame Time: " << frameTime << " ms"
                      << " | Average Frame Time: " << averageFrameTime << " ms"
                      << " | Highest Frame Time: " << highestFrameTime << " ms"
                      << std::flush;
        }

    private:
        int frameCount = 0;
        float totalFrameTime = 0.0f;
        float averageFrameTime = 0.0f;
        float highestFrameTime = 0.0f;
    };
}

// internal
#include "../Math/Generics.h"

// std
#include <iostream>

namespace Isonia::Debug
{
    PerformanceTracker::logFrameTime(float frame_time_s)
    {
        // Convert frame_time_s to ms
        float frame_time_ms = frame_time_s * 1000.0f;

        // New frame
        ++m_frame_count;

        // Calculate and update average frame time
        m_total_frame_time_ms += frame_time_ms;
        m_average_frame_time_ms = m_total_frame_time_ms / m_frame_count;

        // Update highest frame time
        m_highest_frame_time_ms = Math::Max(highest_frame_time_ms, frame_time_ms);

        // Move the cursor to the beginning of the line
        std::cout << "\r";

        // Print performance statistics
        std::cout << std::fixed << std::setw(8) << std::setprecision(6)
                  << "Frame Time: " << frameTime << " ms"
                  << " | Average Frame Time: " << averageFrameTime << " ms"
                  << " | Highest Frame Time: " << highestFrameTime << " ms"
                  << std::flush;
    }
}

// internal
#include "Debug.h"

// external
#include <iomanip>

namespace Isonia::Debug
{
    char* PerformanceTracker::logFrameTime(float frame_time_s)
    {
        // Convert frame_time_s to ms
        float frame_time_ms = frame_time_s * 1'000.0f;

        // New frame
        m_frame_count++;

        // Calculate and update average frame time
        m_total_frame_time_ms += frame_time_ms;
        m_average_frame_time_ms = m_total_frame_time_ms / m_frame_count;

        // Update highest frame time
        m_highest_frame_time_ms = Math::maxf(m_highest_frame_time_ms, frame_time_ms);

        // Create a buffer to hold the performance statistics
        char buffer[128];

        // Format the string
        int length = sprintf(buffer, "Frame Time: %.4f ms\nAverage Frame Time: %.4f ms\nHighest Frame Time: %.4f ms", frame_time_ms, m_average_frame_time_ms, m_highest_frame_time_ms);

        // Allocate memory for the char* to be returned
        char* cstr = (char*)malloc((length + 1) * sizeof(char));
        if (cstr != NULL)
        {
            strcpy(cstr, buffer);
        }

        // Return the allocated char*
        return cstr;
    }
}

#pragma once

// internal
#include "../Math/Math.h"

namespace Isonia::Debug
{
    struct PerformanceTracker
    {
    public:
        char* logFrameTime(float frame_time_s);

        PerformanceTracker() = default;
        PerformanceTracker(const PerformanceTracker&) = delete;
        PerformanceTracker& operator=(const PerformanceTracker&) = delete;

    private:
        unsigned int m_frame_count = 0;
        float m_total_frame_time_ms = 0.0f;
        float m_average_frame_time_ms = 0.0f;
        float m_highest_frame_time_ms = 0.0f;
    };
}

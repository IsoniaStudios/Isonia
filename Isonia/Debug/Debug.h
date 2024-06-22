// internal
#include "../Math/Math.h"

// std
#include <iostream>
#include <iomanip>

namespace Isonia::Debug
{
    struct PerformanceTracker
    {
    public:
        void logFrameTime(float frame_time_s);

    private:
        unsigned int m_frame_count = 0;
        float m_total_frame_time_ms = 0.0f;
        float m_average_frame_time_ms = 0.0f;
        float m_highest_frame_time_ms = 0.0f;
    };
}

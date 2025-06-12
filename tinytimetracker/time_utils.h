#pragma once
#include "types.h"
#include <sstream>
#include <iomanip>

namespace time_tracker {
    namespace time_utils {
        std::string get_current_timestamp();
        std::string get_date_string();
        std::string get_week_string();
        std::string format_duration(std::chrono::system_clock::duration duration);
        std::string format_time_countdown(std::chrono::system_clock::duration duration);

        // calculate required breaks based on work duration
        std::chrono::system_clock::duration calculate_required_breaks(
            std::chrono::system_clock::duration work_duration);
    }
}

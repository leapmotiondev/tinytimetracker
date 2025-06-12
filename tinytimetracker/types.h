#pragma once
#include "windows_includes.h"
#include <chrono>
#include <string>

namespace time_tracker {
    enum class work_state {
        clocked_out,
        clocked_in,
        on_break
    };

    struct time_entry {
        std::chrono::system_clock::time_point timestamp;
        std::string action;
        bool is_automatic;

        time_entry(const std::string& act, bool auto_flag = false)
            : timestamp(std::chrono::system_clock::now()), action(act), is_automatic(auto_flag) {
        }
    };

    struct work_session {
        std::chrono::system_clock::time_point start_time;
        std::chrono::system_clock::time_point end_time;
        std::chrono::system_clock::duration total_break_time{ 0 };
        bool has_first_break{ false };
        bool has_second_break{ false };
    };
}

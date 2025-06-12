#pragma once
#include "types.h"
#include <mutex>
#include <string>

namespace time_tracker {
    class logger {
    private:
        std::mutex log_mutex_;
        std::string time_log_path_;
        std::string weekly_log_path_;
        std::string session_log_path_;

    public:
        logger();

        void log_time_entry(const std::string& action, bool is_automatic = false);
        void log_session_event(const std::string& action);
        void update_weekly_hours(std::chrono::system_clock::duration work_duration);

        void open_time_log();
        void open_weekly_log();
        void open_session_log();
    };
}

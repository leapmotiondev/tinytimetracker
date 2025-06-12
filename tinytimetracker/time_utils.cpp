#include "time_utils.h"
#include "config.h"

namespace time_tracker {
    namespace time_utils {
        std::string get_current_timestamp() {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);

            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
            return ss.str();
        }

        std::string get_date_string() {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);

            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d");
            return ss.str();
        }

        std::string get_week_string() {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);

            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y-W%U");
            return ss.str();
        }

        std::string format_duration(std::chrono::system_clock::duration duration) {
            auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
            auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration - hours);

            std::stringstream ss;
            ss << hours.count() << "h " << minutes.count() << "m";
            return ss.str();
        }

        std::string format_time_countdown(std::chrono::system_clock::duration duration) {
            auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
            auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration - hours);
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration - hours - minutes);

            std::stringstream ss;
            ss << std::setfill('0') << std::setw(2) << hours.count() << ":"
                << std::setfill('0') << std::setw(2) << minutes.count() << ":"
                << std::setfill('0') << std::setw(2) << seconds.count();
            return ss.str();
        }

        std::chrono::system_clock::duration calculate_required_breaks(
            std::chrono::system_clock::duration work_duration) {

            auto total_breaks = std::chrono::minutes(0);

            // first break after 6 hours
            if (work_duration >= std::chrono::milliseconds(config::FIRST_BREAK_AFTER_MS)) {
                total_breaks += std::chrono::minutes(config::FIRST_BREAK_DURATION_MIN);
            }

            // second break after 9 hours
            if (work_duration >= std::chrono::milliseconds(config::SECOND_BREAK_AFTER_MS)) {
                total_breaks += std::chrono::minutes(config::SECOND_BREAK_DURATION_MIN);
            }

            return total_breaks;
        }
    }
}

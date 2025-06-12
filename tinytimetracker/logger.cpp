#include "logger.h"
#include "time_utils.h"
#include "config.h"
#include <fstream>
#include <vector>

namespace time_tracker {
    logger::logger()
        : time_log_path_(config::TIME_LOG_FILE)
        , weekly_log_path_(config::WEEKLY_LOG_FILE)
        , session_log_path_(config::SESSION_LOG_FILE) {
    }

    void logger::log_time_entry(const std::string& action, bool is_automatic) {
        std::lock_guard<std::mutex> lock(log_mutex_);

        std::ofstream log_file(time_log_path_, std::ios::app);
        if (log_file.is_open()) {
            std::string prefix = is_automatic ? "[AUTO] " : "";
            log_file << time_utils::get_current_timestamp() << " - " << prefix << action << "\n";
            log_file.close();
        }
    }

    void logger::log_session_event(const std::string& action) {
        std::lock_guard<std::mutex> lock(log_mutex_);

        std::ofstream log_file(session_log_path_, std::ios::app);
        if (log_file.is_open()) {
            log_file << time_utils::get_current_timestamp() << " - " << action << "\n";
            log_file.close();
        }
    }

    void logger::update_weekly_hours(std::chrono::system_clock::duration work_duration) {
        std::lock_guard<std::mutex> lock(log_mutex_);

        std::string today = time_utils::get_date_string();
        std::string week = time_utils::get_week_string();

        // read existing data
        std::vector<std::string> lines;
        std::ifstream read_file(weekly_log_path_);
        std::string line;
        bool found_today = false;

        while (std::getline(read_file, line)) {
            if (line.find(today) == 0) {
                found_today = true;
                lines.push_back(today + " - " + time_utils::format_duration(work_duration));
            }
            else {
                lines.push_back(line);
            }
        }
        read_file.close();

        if (!found_today) {
            lines.push_back(today + " - " + time_utils::format_duration(work_duration));
        }

        // write back to file
        std::ofstream write_file(weekly_log_path_);
        for (const auto& l : lines) {
            write_file << l << "\n";
        }
        write_file.close();
    }

    void logger::open_time_log() {
        ShellExecuteA(nullptr, "open", time_log_path_.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }

    void logger::open_weekly_log() {
        ShellExecuteA(nullptr, "open", weekly_log_path_.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }

    void logger::open_session_log() {
        ShellExecuteA(nullptr, "open", session_log_path_.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
}

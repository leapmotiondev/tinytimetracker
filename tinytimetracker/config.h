#pragma once
#include "windows_includes.h"

namespace time_tracker {
    namespace config {
        // work time limits (in milliseconds)
        constexpr DWORD MAX_WORK_HOURS_MS = 10 * 60 * 60 * 1000;  // 10 hours
        constexpr DWORD FIRST_BREAK_AFTER_MS = 6 * 60 * 60 * 1000;  // 6 hours -> 30min break
        constexpr DWORD SECOND_BREAK_AFTER_MS = 9 * 60 * 60 * 1000;  // 9 hours -> 15min break

        // break durations (in minutes)
        constexpr int FIRST_BREAK_DURATION_MIN = 30;
        constexpr int SECOND_BREAK_DURATION_MIN = 15;

        // timer intervals
        constexpr DWORD BREAK_CHECK_INTERVAL_MS = 60000;  // check every minute
        constexpr DWORD STATUS_UPDATE_INTERVAL_MS = 1000;  // update status every second

        // file paths
        constexpr char TIME_LOG_FILE[] = "time_log.txt";
        constexpr char WEEKLY_LOG_FILE[] = "weekly_hours.txt";
        constexpr char SESSION_LOG_FILE[] = "session_log.txt";

        // window messages
        constexpr UINT WM_TRAY_ICON = WM_USER + 1;
        constexpr UINT TRAY_ICON_ID = 1001;

        // timer ids
        constexpr UINT TIMER_ID_AUTO_BREAK = 1002;
        constexpr UINT TIMER_ID_MAX_HOURS = 1003;
        constexpr UINT TIMER_ID_STATUS_UPDATE = 1004;

        // menu ids
        constexpr UINT ID_CLOCK_IN = 2001;
        constexpr UINT ID_CLOCK_OUT = 2002;
        constexpr UINT ID_START_BREAK = 2003;
        constexpr UINT ID_END_BREAK = 2004;
        constexpr UINT ID_VIEW_LOG = 2005;
        constexpr UINT ID_VIEW_WEEKLY = 2006;
        constexpr UINT ID_EXIT = 2007;

        // menu info items (non-clickable)
        constexpr UINT ID_INFO_STATUS = 3001;
        constexpr UINT ID_INFO_WORKING_TIME = 3002;
        constexpr UINT ID_INFO_NEXT_BREAK = 3003;
        constexpr UINT ID_INFO_REMAINING = 3004;
    }
}

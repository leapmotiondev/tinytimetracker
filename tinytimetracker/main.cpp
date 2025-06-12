// ===== main.cpp =====
#include "windows_includes.h"
#include "config.h"
#include "types.h"
#include "time_utils.h"
#include "logger.h"

using namespace time_tracker;

// menu info helper for dynamic menu text
class menu_info {
public:
    static std::wstring working_time_text;
    static std::wstring next_break_text;
    static std::wstring remaining_text;
    static std::wstring status_text;

    static void update_working_time(const std::string& time_str) {
        working_time_text = L"⏰ Working: " + std::wstring(time_str.begin(), time_str.end());
    }

    static void update_next_break(const std::string& time_str) {
        next_break_text = L"☕ Next Break: " + std::wstring(time_str.begin(), time_str.end());
    }

    static void update_remaining_time(const std::string& time_str) {
        remaining_text = L"⏳ Remaining: " + std::wstring(time_str.begin(), time_str.end());
    }

    static void update_status(work_state state) {
        switch (state) {
        case work_state::clocked_in:
            status_text = L"🟢 Status: Working";
            break;
        case work_state::on_break:
            status_text = L"🟡 Status: On Break";
            break;
        default:
            status_text = L"🔴 Status: Clocked Out";
            break;
        }
    }
};

// static member definitions
std::wstring menu_info::working_time_text = L"⏰ Working: 00:00:00";
std::wstring menu_info::next_break_text = L"☕ Next Break: --:--:--";
std::wstring menu_info::remaining_text = L"⏳ Remaining: 08:00:00";
std::wstring menu_info::status_text = L"🔴 Status: Clocked Out";

class time_tracker_app {
private:
    HWND main_window_{ nullptr };
    NOTIFYICONDATA notify_icon_data_{};
    work_state current_state_{ work_state::clocked_out };

    std::chrono::system_clock::time_point clock_in_time_{};
    std::chrono::system_clock::time_point break_start_time_{};

    logger logger_;

    bool first_break_taken_{ false };
    bool second_break_taken_{ false };

    void update_tray_tooltip() {
        std::wstring tooltip;
        switch (current_state_) {
        case work_state::clocked_in:
            tooltip = L"Working";
            break;
        case work_state::on_break:
            tooltip = L"On Break";
            break;
        default:
            tooltip = L"Clocked Out";
            break;
        }

        wcscpy_s(notify_icon_data_.szTip, sizeof(notify_icon_data_.szTip) / sizeof(WCHAR), tooltip.c_str());
        Shell_NotifyIcon(NIM_MODIFY, &notify_icon_data_);
    }

    void show_context_menu(HWND hwnd) {
        POINT cursor_pos;
        GetCursorPos(&cursor_pos);

        HMENU context_menu = CreatePopupMenu();

        // add status info at top (grayed out, non-clickable)
        AppendMenu(context_menu, MF_STRING | MF_GRAYED, config::ID_INFO_STATUS, menu_info::status_text.c_str());

        if (current_state_ != work_state::clocked_out) {
            AppendMenu(context_menu, MF_STRING | MF_GRAYED, config::ID_INFO_WORKING_TIME, menu_info::working_time_text.c_str());
            AppendMenu(context_menu, MF_STRING | MF_GRAYED, config::ID_INFO_NEXT_BREAK, menu_info::next_break_text.c_str());
            AppendMenu(context_menu, MF_STRING | MF_GRAYED, config::ID_INFO_REMAINING, menu_info::remaining_text.c_str());
        }

        AppendMenu(context_menu, MF_SEPARATOR, 0, nullptr);

        // add action items based on current state
        switch (current_state_) {
        case work_state::clocked_out:
            AppendMenu(context_menu, MF_STRING, config::ID_CLOCK_IN, L"🟢 Clock In");
            break;
        case work_state::clocked_in:
            AppendMenu(context_menu, MF_STRING, config::ID_CLOCK_OUT, L"🔴 Clock Out");
            AppendMenu(context_menu, MF_STRING, config::ID_START_BREAK, L"☕ Start Break");
            break;
        case work_state::on_break:
            AppendMenu(context_menu, MF_STRING, config::ID_END_BREAK, L"✅ End Break");
            AppendMenu(context_menu, MF_STRING, config::ID_CLOCK_OUT, L"🔴 Clock Out");
            break;
        }

        AppendMenu(context_menu, MF_SEPARATOR, 0, nullptr);
        AppendMenu(context_menu, MF_STRING, config::ID_VIEW_LOG, L"📄 View Daily Log");
        AppendMenu(context_menu, MF_STRING, config::ID_VIEW_WEEKLY, L"📊 View Weekly Hours");
        AppendMenu(context_menu, MF_SEPARATOR, 0, nullptr);
        AppendMenu(context_menu, MF_STRING, config::ID_EXIT, L"❌ Exit");

        // show menu at cursor position
        SetForegroundWindow(hwnd);
        TrackPopupMenu(context_menu, TPM_RIGHTBUTTON, cursor_pos.x, cursor_pos.y, 0, hwnd, nullptr);
        DestroyMenu(context_menu);
    }

    void clock_in(bool is_automatic = false) {
        current_state_ = work_state::clocked_in;
        clock_in_time_ = std::chrono::system_clock::now();
        first_break_taken_ = false;
        second_break_taken_ = false;

        logger_.log_time_entry("CLOCK IN", is_automatic);
        update_tray_tooltip();
        update_menu_info();

        // start timers for break and max hours checking
        SetTimer(main_window_, config::TIMER_ID_AUTO_BREAK, config::BREAK_CHECK_INTERVAL_MS, nullptr);
        SetTimer(main_window_, config::TIMER_ID_MAX_HOURS, config::BREAK_CHECK_INTERVAL_MS, nullptr);
        SetTimer(main_window_, config::TIMER_ID_STATUS_UPDATE, config::STATUS_UPDATE_INTERVAL_MS, nullptr);

        if (!is_automatic) {
            show_balloon_notification(L"Time Tracker", L"Successfully clocked in! 🟢");
        }
    }

    void clock_out(bool is_automatic = false) {
        if (current_state_ == work_state::clocked_out) return;

        auto now = std::chrono::system_clock::now();
        auto work_duration = now - clock_in_time_;

        // calculate and add automatic legal breaks if not taken
        auto required_breaks = time_utils::calculate_required_breaks(work_duration);
        work_duration -= required_breaks;  // subtract required breaks from work time

        if (required_breaks > std::chrono::minutes(0)) {
            logger_.log_time_entry("AUTO BREAKS ADDED: " + time_utils::format_duration(required_breaks), true);
        }

        current_state_ = work_state::clocked_out;
        logger_.log_time_entry("CLOCK OUT - Net Work Time: " + time_utils::format_duration(work_duration), is_automatic);
        logger_.update_weekly_hours(work_duration);
        update_tray_tooltip();
        update_menu_info();

        // stop all timers
        KillTimer(main_window_, config::TIMER_ID_AUTO_BREAK);
        KillTimer(main_window_, config::TIMER_ID_MAX_HOURS);
        KillTimer(main_window_, config::TIMER_ID_STATUS_UPDATE);

        if (!is_automatic) {
            std::string duration_str = time_utils::format_duration(work_duration);
            std::wstring message = L"Successfully clocked out! 🔴\nNet work time: " +
                std::wstring(duration_str.begin(), duration_str.end());
            show_balloon_notification(L"Time Tracker", message);
        }
    }

    void start_break(bool is_automatic = false) {
        if (current_state_ != work_state::clocked_in) return;

        current_state_ = work_state::on_break;
        break_start_time_ = std::chrono::system_clock::now();

        logger_.log_time_entry("BREAK START", is_automatic);
        update_tray_tooltip();
        update_menu_info();

        if (!is_automatic) {
            show_balloon_notification(L"Time Tracker", L"Break started! ☕");
        }
    }

    void end_break() {
        if (current_state_ != work_state::on_break) return;

        auto now = std::chrono::system_clock::now();
        auto break_duration = now - break_start_time_;

        current_state_ = work_state::clocked_in;
        logger_.log_time_entry("BREAK END - Duration: " + time_utils::format_duration(break_duration));
        update_tray_tooltip();
        update_menu_info();

        std::string duration_str = time_utils::format_duration(break_duration);
        std::wstring message = L"Break ended! ✅\nBreak duration: " +
            std::wstring(duration_str.begin(), duration_str.end());
        show_balloon_notification(L"Time Tracker", message);
    }

    void check_automatic_breaks() {
        if (current_state_ != work_state::clocked_in) return;

        auto now = std::chrono::system_clock::now();
        auto work_duration = now - clock_in_time_;

        // check for first break after 6 hours
        if (!first_break_taken_ && work_duration >= std::chrono::milliseconds(config::FIRST_BREAK_AFTER_MS)) {
            first_break_taken_ = true;
            show_balloon_notification(L"Break Reminder",
                L"Time for mandatory break! ⏰\n30-minute break required after 6 hours.");
        }

        // check for second break after 9 hours
        if (!second_break_taken_ && work_duration >= std::chrono::milliseconds(config::SECOND_BREAK_AFTER_MS)) {
            second_break_taken_ = true;
            show_balloon_notification(L"Second Break Reminder",
                L"Time for second break! ⏰\n15-minute break required after 9 hours.");
        }
    }

    void check_max_work_hours() {
        if (current_state_ == work_state::clocked_out) return;

        auto now = std::chrono::system_clock::now();
        auto total_duration = now - clock_in_time_;

        // auto clock out after 10 hours
        if (total_duration >= std::chrono::milliseconds(config::MAX_WORK_HOURS_MS)) {
            clock_out(true);  // automatic clock out
            show_balloon_notification(L"Auto Clock Out",
                L"Automatic clock out after 10 hours! ⚠️\nFor your health and legal compliance.");
        }
    }

    void update_menu_info() {
        menu_info::update_status(current_state_);

        if (current_state_ == work_state::clocked_out) {
            menu_info::update_working_time("00:00:00");
            menu_info::update_next_break("--:--:--");
            menu_info::update_remaining_time("08:00:00");
            return;
        }

        auto now = std::chrono::system_clock::now();
        auto worked = now - clock_in_time_;

        // current working time (including breaks)
        if (current_state_ == work_state::on_break) {
            auto break_time = now - break_start_time_;
            auto net_worked = worked - break_time;
            menu_info::update_working_time(time_utils::format_time_countdown(net_worked));
        }
        else {
            menu_info::update_working_time(time_utils::format_time_countdown(worked));
        }

        // next break countdown
        std::chrono::system_clock::duration time_to_next_break;
        if (!first_break_taken_ && worked < std::chrono::milliseconds(config::FIRST_BREAK_AFTER_MS)) {
            time_to_next_break = std::chrono::milliseconds(config::FIRST_BREAK_AFTER_MS) - worked;
            menu_info::update_next_break(time_utils::format_time_countdown(time_to_next_break));
        }
        else if (!second_break_taken_ && worked < std::chrono::milliseconds(config::SECOND_BREAK_AFTER_MS)) {
            time_to_next_break = std::chrono::milliseconds(config::SECOND_BREAK_AFTER_MS) - worked;
            menu_info::update_next_break(time_utils::format_time_countdown(time_to_next_break));
        }
        else {
            menu_info::update_next_break("No more breaks");
        }

        // remaining work time (target 8 hours minus worked time plus required breaks)
        auto target_work = std::chrono::hours(8);
        auto required_breaks = time_utils::calculate_required_breaks(worked);
        auto net_worked = worked - required_breaks;
        auto remaining = target_work - net_worked;

        if (remaining.count() > 0) {
            menu_info::update_remaining_time(time_utils::format_time_countdown(remaining));
        }
        else {
            menu_info::update_remaining_time("00:00:00 (Overtime!)");
        }
    }

public:
    time_tracker_app() = default;
    ~time_tracker_app() { cleanup(); }

    bool initialize(HINSTANCE instance, HWND hwnd) {
        main_window_ = hwnd;

        // register for session notifications
        WTSRegisterSessionNotification(hwnd, NOTIFY_FOR_THIS_SESSION);

        // initialize NOTIFYICONDATA structure
        notify_icon_data_.cbSize = sizeof(NOTIFYICONDATA);
        notify_icon_data_.hWnd = hwnd;
        notify_icon_data_.uID = config::TRAY_ICON_ID;
        notify_icon_data_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        notify_icon_data_.uCallbackMessage = config::WM_TRAY_ICON;

        // load default application icon
        notify_icon_data_.hIcon = LoadIcon(nullptr, IDI_APPLICATION);

        // set initial tooltip
        wcscpy_s(notify_icon_data_.szTip, sizeof(notify_icon_data_.szTip) / sizeof(WCHAR), L"Time Tracker - Clocked OUT");

        // initialize menu info
        menu_info::update_status(work_state::clocked_out);

        // add icon to system tray
        return Shell_NotifyIcon(NIM_ADD, &notify_icon_data_);
    }

    void show_balloon_notification(const std::wstring& title, const std::wstring& message) {
        // update notification data
        notify_icon_data_.uFlags |= NIF_INFO;
        notify_icon_data_.dwInfoFlags = NIIF_INFO;
        wcscpy_s(notify_icon_data_.szInfoTitle, sizeof(notify_icon_data_.szInfoTitle) / sizeof(WCHAR), title.c_str());
        wcscpy_s(notify_icon_data_.szInfo, sizeof(notify_icon_data_.szInfo) / sizeof(WCHAR), message.c_str());

        // show balloon notification
        Shell_NotifyIcon(NIM_MODIFY, &notify_icon_data_);

        // reset flags after showing
        notify_icon_data_.uFlags &= ~NIF_INFO;
    }

    void handle_tray_message(WPARAM wparam, LPARAM lparam) {
        if (wparam == config::TRAY_ICON_ID) {
            switch (lparam) {
            case WM_RBUTTONDOWN:
            case WM_CONTEXTMENU:
                show_context_menu(main_window_);
                break;
            case WM_LBUTTONDBLCLK:
                // double-click behavior based on current state
                switch (current_state_) {
                case work_state::clocked_out:
                    clock_in();
                    break;
                case work_state::clocked_in:
                    clock_out();
                    break;
                case work_state::on_break:
                    end_break();
                    break;
                }
                break;
            }
        }
    }

    void handle_command(WORD command_id) {
        switch (command_id) {
        case config::ID_CLOCK_IN:
            clock_in();
            break;
        case config::ID_CLOCK_OUT:
            clock_out();
            break;
        case config::ID_START_BREAK:
            start_break();
            break;
        case config::ID_END_BREAK:
            end_break();
            break;
        case config::ID_VIEW_LOG:
            logger_.open_time_log();
            break;
        case config::ID_VIEW_WEEKLY:
            logger_.open_weekly_log();
            break;
        case config::ID_EXIT:
            PostQuitMessage(0);
            break;
        }
    }


    // Update the timer handler
    void handle_timer(UINT timer_id) {
        switch (timer_id) {
        case config::TIMER_ID_AUTO_BREAK:
            check_automatic_breaks();
            break;
        case config::TIMER_ID_MAX_HOURS:
            check_max_work_hours();
            break;
        case config::TIMER_ID_STATUS_UPDATE:
            update_menu_info();
            break;
        }
    }

    void handle_session_notification(WPARAM wparam, LPARAM lparam) {
        switch (wparam) {
        case WTS_SESSION_LOCK:
            logger_.log_session_event("SCREEN LOCKED");
            break;
        case WTS_SESSION_UNLOCK:
            logger_.log_session_event("SCREEN UNLOCKED");
            break;
        case WTS_SESSION_LOGOFF:
            logger_.log_session_event("USER LOGOFF");
            if (current_state_ != work_state::clocked_out) {
                clock_out(true);  // auto clock out on logoff
            }
            break;
        case WTS_SESSION_LOGON:
            logger_.log_session_event("USER LOGON");
            break;
        }
    }

    void cleanup() {
        // stop all timers
        KillTimer(main_window_, config::TIMER_ID_AUTO_BREAK);
        KillTimer(main_window_, config::TIMER_ID_MAX_HOURS);
        KillTimer(main_window_, config::TIMER_ID_STATUS_UPDATE);

        // unregister session notifications
        WTSUnRegisterSessionNotification(main_window_);

        // remove tray icon
        Shell_NotifyIcon(NIM_DELETE, &notify_icon_data_);
    }
};

// global instance
time_tracker_app g_app;

// window procedure
LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
    case config::WM_TRAY_ICON:
        g_app.handle_tray_message(wparam, lparam);
        return 0;
    case WM_COMMAND:
        g_app.handle_command(LOWORD(wparam));
        return 0;
    case WM_TIMER:
        g_app.handle_timer(static_cast<UINT>(wparam));
        return 0;
    case WM_WTSSESSION_CHANGE:
        g_app.handle_session_notification(wparam, lparam);
        return 0;
    case WM_DESTROY:
        g_app.cleanup();
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }
}

// application entry point
int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int) {
    // register window class
    const wchar_t* class_name = L"TimeTrackerWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = window_proc;
    wc.hInstance = instance;
    wc.lpszClassName = class_name;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClass(&wc)) {
        MessageBox(nullptr, L"Failed to register window class!", L"Error", MB_ICONERROR);
        return 1;
    }

    // create hidden window for message handling
    HWND hwnd = CreateWindow(
        class_name,
        L"Tiny Time Tracker",
        0,
        0, 0, 0, 0,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if (!hwnd) {
        MessageBox(nullptr, L"Failed to create window!", L"Error", MB_ICONERROR);
        return 1;
    }

    // initialize time tracker
    if (!g_app.initialize(instance, hwnd)) {
        MessageBox(nullptr, L"Failed to initialize system tray icon!", L"Error", MB_ICONERROR);
        return 1;
    }

    // message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}
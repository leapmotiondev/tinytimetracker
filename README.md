# 🕐 TinyTimeTracker

A lightweight, professional time tracking tool for Windows with German labor law compliance and clean system tray integration.

## ✨ Features

### 🎯 Core Functionality
- **System Tray Integration** - Runs quietly in the background
- **German Labor Law Compliance** - Automatic break calculations (30min after 6h, 15min after 9h)
- **Smart Clock Management** - 10-hour automatic clock-out protection
- **Session Tracking** - Monitors Windows lock/unlock events
- **Toast Notifications** - Native Windows notifications for all actions

### 📊 Time Management
- **Daily Time Logs** - Detailed logging with timestamps
- **Weekly Hours Tracking** - Automatic weekly summaries
- **Break Management** - Manual breaks with automatic legal break additions
- **Overtime Protection** - Automatic clock-out after 10 hours
- **Net Work Time Calculation** - Automatically subtracts required breaks

### 🔧 Smart Features
- **Context-Aware Menu** - Live status display in right-click menu with timers
- **Double-Click Actions** - Quick clock in/out with double-click
- **Automatic Break Reminders** - Non-intrusive notifications
- **Real-time Updates** - Live working time display in context menu

## 🚀 Quick Start

### Installation
1. Download the latest release from [Releases](../../releases)
2. Extract `TinyTimeTracker.exe` to your preferred location
3. Run the executable - it will appear in your system tray

### Usage
- **Right-click** the tray icon to access the menu with live timers
- **Double-click** for quick clock in/out
- **View live status** in the context menu (working time, next break, remaining time)
- **View logs** through the context menu

## 📋 System Requirements

- Windows 10/11 (64-bit recommended)
- .NET Framework (included in modern Windows)
- ~2MB disk space
- Minimal system resources

## 🎨 Context Menu Features

The right-click menu shows live information:
- **🟢 Status: Working** - Current work state
- **⏰ Working: 02:45:23** - Live timer of current work session
- **☕ Next Break: 03:14:37** - Countdown to next mandatory break
- **⏳ Remaining: 05:14:37** - Time remaining to reach 8-hour target

## 🛠️ Technical Details

### Architecture
- **Modern C++17** - Clean, efficient codebase
- **Native Win32 API** - No external dependencies
- **Modular Design** - Separated concerns (logging, UI, time calculations)
- **Memory Safe** - Proper resource management

### File Structure
```
📁 Project Root
├── 📄 main.cpp - Core application logic
├── 📄 config.h - Configuration constants
├── 📄 types.h - Data structures
├── 📄 time_utils.h/cpp - Time calculation utilities
├── 📄 logger.h/cpp - File logging system
└── 📄 windows_includes.h - Windows API headers
```

### Generated Files
- `time_log.txt` - Daily clock in/out events
- `weekly_hours.txt` - Weekly work summaries
- `session_log.txt` - Windows session events (lock/unlock)

## 🇩🇪 German Labor Law Compliance

TinyTimeTracker automatically ensures compliance with German working time regulations:

- **Maximum 10 hours** per day with automatic clock-out
- **30-minute break** required after 6 hours of work
- **Additional 15-minute break** required after 9 hours
- **Automatic break calculation** - breaks are subtracted from total work time even if not manually taken

## 🎯 Roadmap

### Planned Features
- [ ] **Dashboard Window** with charts and analytics
- [ ] **Microsoft Teams Integration** - Sync presence status
- [ ] **Project Time Tracking** - Multiple project support
- [ ] **Export Functions** - CSV/PDF reports
- [ ] **Cloud Sync** - OneDrive/Google Drive backup
- [ ] **Multi-language Support** - German, English, more
- [ ] **Installer Package** - Professional setup wizard
- [ ] **Visual Progress Indicators** - Enhanced tray icon animations

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

### Development Setup
1. Clone the repository
2. Open in Visual Studio 2022
3. Build configuration: Release x64
4. Required: C++17 standard or higher

### Code Style
- Use `snake_case` for variables and functions
- Use lowercase English comments
- Keep functions focused and modular
- Follow modern C++ best practices

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Built with modern C++ and Win32 API
- Inspired by the need for simple, effective time tracking
- Designed for developers who value clean, minimal tools
- German labor law compliance ensures legal working hours

## 📞 Support

- **Issues**: [GitHub Issues](../../issues)
- **Discussions**: [GitHub Discussions](../../discussions)
- **Email**: [dev@leapmotion.io](mailto:dev@leapmotion.io)

---

**Made with ❤️ by leapmotion - for productive developers**

[⭐ Star this repo](../../stargazers) | [🐛 Report Bug](../../issues) | [💡 Request Feature](../../issues)

#pragma once

#include <LittleFS.h>
#include <Arduino.h>
#include <vector>

// Logs critical events to LittleFS with a timestamp and event code
// (thesis section 3.3). Uses a simple size-capped log file instead of a
// true ring buffer: once the file grows past kMaxBytes, the oldest half of
// the lines is dropped so the file never grows unbounded on flash.
class EventLogger {
public:
    enum class Level { Info, Warn, Error };

    bool begin() {
        ready_ = LittleFS.begin(true);
        return ready_;
    }

    void log(Level level, const char* message) {
        if (!ready_) return;

        File f = LittleFS.open(kLogPath, "a");
        if (!f) return;
        f.printf("%lu,%s,%s\n", millis(), levelTag(level), message);
        f.close();

        if (LittleFS.exists(kLogPath)) {
            File check = LittleFS.open(kLogPath, "r");
            if (check && check.size() > kMaxBytes) {
                check.close();
                trim();
            } else if (check) {
                check.close();
            }
        }
    }

private:
    static constexpr const char* kLogPath = "/events.log";
    static constexpr size_t kMaxBytes = 32768;

    static const char* levelTag(Level level) {
        switch (level) {
            case Level::Info: return "INFO";
            case Level::Warn: return "WARN";
            default: return "ERROR";
        }
    }

    void trim() {
        File f = LittleFS.open(kLogPath, "r");
        if (!f) return;
        std::vector<String> lines;
        while (f.available()) {
            lines.push_back(f.readStringUntil('\n'));
        }
        f.close();

        const size_t keepFrom = lines.size() / 2;
        File out = LittleFS.open(kLogPath, "w");
        if (!out) return;
        for (size_t i = keepFrom; i < lines.size(); ++i) {
            if (lines[i].length() == 0) continue;
            out.print(lines[i]);
            out.print('\n');
        }
        out.close();
    }

    bool ready_ = false;
};

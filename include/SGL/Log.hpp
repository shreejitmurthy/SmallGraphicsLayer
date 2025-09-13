#pragma once

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/std.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace SmallGraphicsLayer {
class Logger {
public:
    static void Init(bool enable = false);
    static bool isEnabled();
    static std::shared_ptr<spdlog::logger> Log();
private:
    static bool enabled;
};
}
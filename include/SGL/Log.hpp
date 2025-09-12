#pragma once

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/std.h>
#include <spdlog/sinks/stdout_color_sinks.h>

inline void log_init() {
    auto logger = spdlog::stdout_color_mt("console");
    logger->set_pattern("RELIC: %^[%Y-%m-%d %H:%M:%S.%e] [%l] %v%$");
}

inline std::shared_ptr<spdlog::logger> log() {
    return spdlog::get("console");
}

#include "SGL/Log.hpp"

bool SmallGraphicsLayer::Logger::enabled = false;

void SmallGraphicsLayer::Logger::Init(bool enable) {
    auto logger = spdlog::stdout_color_mt("sgl");
    logger->set_pattern("\033[1mSGL:\033[0m %^[%H:%M:%S] [%l] %v%$");
    enabled = true;
    logger->set_level(enable ? spdlog::level::info : spdlog::level::warn);
}

bool SmallGraphicsLayer::Logger::isEnabled() {
    return enabled;
}

std::shared_ptr<spdlog::logger> SmallGraphicsLayer::Logger::Log() {
    return spdlog::get("sgl");
}
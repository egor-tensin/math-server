// Copyright (c) 2019 Egor Tensin <egor@tensin.name>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#pragma once

#include <boost/system/error_code.hpp>

#include <exception>
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

namespace math::server::log {
namespace details {

inline std::string get_tid() {
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
}

inline std::string get_timestamp() {
    const auto now = std::chrono::system_clock::now();
    return std::format("{:%F %T%z}", std::chrono::floor<std::chrono::seconds>(now));
}

inline void log(const std::string& msg) {
    std::clog << std::format("{} | {} | {}\n", get_timestamp(), get_tid(), msg);
}

} // namespace details

template <typename... Args>
inline void log(std::format_string<Args...> fmt, Args&&... args) {
    details::log(std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
inline void error(std::format_string<Args...> fmt, Args&&... args) {
    details::log(std::format(fmt, std::forward<Args>(args)...));
}

inline void error(const boost::system::error_code& ec) {
    log("{}", ec.message());
}

inline void error(const std::exception& e) {
    log("{}", e.what());
}

} // namespace math::server::log
